#include "stm32f4xx.h"
#include "leds.h"
#include "mcp3422.h"
#include "batteries.h"
#include "24AA025E48.h"
#include "ethernet_bridge.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

/* таблица декодирования для семисегментного индикатора */
uint8_t const decode_table[decode_tablesize]={0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};   // для надписи Err "E"=0x79, "r"=0x50

WatchTypeDef watch[CHARGERS];
uint8_t LEDsBuffer[DISPLAYS][LEDS]={0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//uint8_t LEDsBuffer[DISPLAYS][LEDS];

extern uint32_t Periph_State;   // регистр состояния периферии (0- модуль исправен, 1- модуль неисправен)
uint8_t curr_err_pos=0;         // номер текущей позиции ошибки

uint8_t ChargeState=0;

//extern int32_t Vbat[BAT_QUANTITY];
extern MCP3424_Meas_TypeDef MCP3424_data;
extern char buf[1024];

extern uint8_t rs485_rxwd_count, rs485_rx_counter, rs485_cs_rx;

extern uint32_t tests_state[CHARGERS];

/* предварительно необходимо включить периферию: GPIOE, GPIOH, GPIOI, SPI2, DMA1 */
void SPI2_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef hspi2;
  DMA_InitTypeDef  DMA_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_GPIOI, ENABLE);
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
  
//  GPIO_WriteBit(led_CS1_GPIO_Port, led_CS1_Pin, Bit_RESET);
//  GPIO_WriteBit(led_CS2_GPIO_Port, led_CS2_Pin, Bit_RESET);
//  GPIO_WriteBit(led_nOE1_GPIO_Port, led_nOE1_Pin, Bit_SET);
//  GPIO_WriteBit(led_nOE2_GPIO_Port, led_nOE2_Pin, Bit_SET);
//  GPIO_InitStructure.GPIO_Pin = R1_Pin | G1_Pin | B1_Pin | led_CS1_Pin | led_CS2_Pin | led_nOE1_Pin | led_nOE2_Pin;
  GPIO_WriteBit(led_upd_GPIO_Port, led_upd_Pin, Bit_SET);
  GPIO_WriteBit(led_en_GPIO_Port, led_en_Pin, Bit_RESET);
  GPIO_WriteBit(B1_GPIO_Port, R1_Pin | G1_Pin | B1_Pin | R2_Pin | G2_Pin | B2_Pin, Bit_SET);
  GPIO_InitStructure.GPIO_Pin = R1_Pin | G1_Pin | B1_Pin | R2_Pin | G2_Pin | B2_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(B1_GPIO_Port, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = polarity1_Pin | polarity2_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(polarity1_GPIO_Port, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = led_upd_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(led_upd_GPIO_Port, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = led_en_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(led_en_GPIO_Port, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = leds_sck_Pin | leds_mosi_Pin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(leds_sck_GPIO_Port, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_SPI2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);
  
  hspi2.SPI_Mode = SPI_Mode_Master;
  hspi2.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
  hspi2.SPI_Direction = SPI_Direction_1Line_Tx;
  hspi2.SPI_DataSize = SPI_DataSize_8b;
  hspi2.SPI_FirstBit = SPI_FirstBit_MSB;
  hspi2.SPI_CPHA = SPI_CPHA_1Edge;
  hspi2.SPI_CPOL = SPI_CPOL_Low;
  hspi2.SPI_CRCPolynomial = SPI_CRCCalculation_Disable;
  hspi2.SPI_NSS = SPI_NSS_Soft;
  SPI_Init(SPI2, &hspi2);
  SPI_Cmd(SPI2, ENABLE);
  
  DMA_DeInit(DMA1_Stream4);
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(SPI2->DR));
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&LEDsBuffer[CHARGER1][0];
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_BufferSize = LEDS;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA1_Stream4, &DMA_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
    
  DMA_ITConfig(DMA1_Stream4, DMA_IT_TC | DMA_IT_TE, ENABLE);

  SPI_DMACmd(SPI2, SPI_DMAReq_Tx, ENABLE);
//  DMA_Cmd(DMA1_Stream4, DISABLE);

  DMA1_Stream4->CR &= ~((uint32_t) DMA_SxCR_EN | DMA_SxCR_CHSEL);
  DMA1_Stream4->PAR = (uint32_t)(&(SPI2->DR));
  DMA1_Stream4->M0AR =  (uint32_t) &LEDsBuffer[0][0];
  DMA1_Stream4->NDTR = (uint32_t) LEDS*2;
//  DMA1_Stream4->CR |= (uint32_t) DMA_SxCR_EN;
  DMA_Cmd(DMA1_Stream4, ENABLE);
  vTaskDelay(2);

//  GPIO_WriteBit(led_CS1_GPIO_Port, led_CS1_Pin, Bit_SET);
//  GPIO_WriteBit(led_CS1_GPIO_Port, led_CS1_Pin, Bit_RESET);
//  GPIO_WriteBit(led_CS2_GPIO_Port, led_CS2_Pin, Bit_SET);
//  vTaskDelay(1);
//  GPIO_WriteBit(led_CS2_GPIO_Port, led_CS2_Pin, Bit_RESET);
//  GPIO_WriteBit(led_nOE1_GPIO_Port, led_nOE1_Pin, Bit_RESET);
//  GPIO_WriteBit(led_nOE2_GPIO_Port, led_nOE2_Pin, Bit_RESET);

  GPIO_WriteBit(led_upd_GPIO_Port, led_upd_Pin, Bit_RESET);
  vTaskDelay(1);
  GPIO_WriteBit(led_upd_GPIO_Port, led_upd_Pin, Bit_SET);
  GPIO_WriteBit(led_en_GPIO_Port, led_en_Pin, Bit_SET);
  vTaskDelay(1);
  watch[CHARGER1].State=STOP;
  watch[CHARGER2].State=STOP;

}

void ConvDataToLEDarr(uint8_t charger)
{
  int32_t tmp32;
  uint8_t hour, min, sec, numerr, temp;
  float voltage;
  extern TaskHandle_t xADC;
  eTaskState eState;
  
  switch (charger)
  {
  case CHARGER1:
    tmp32 = MCP3424_data.VbatA;
    break;
  case CHARGER2:
    tmp32 = MCP3424_data.VbatB;
    break;
  }
  hour = (watch[charger].Hour/ 10) <<4;
  hour |= watch[charger].Hour % 10;
  min = (watch[charger].Min / 10) <<4;
  min |= watch[charger].Min % 10;
  sec = (watch[charger].Sec / 10) <<4;
  sec |= watch[charger].Sec % 10;
  LEDsBuffer[charger][6]=~decode_table[min&0x0f];
  LEDsBuffer[charger][7]=~decode_table[((min>>4)&0x0f)];
  LEDsBuffer[charger][8]=~decode_table[hour&0x0f];
  LEDsBuffer[charger][9]=~decode_table[((hour>>4)&0x0f)];
  numerr = CountOnes(Periph_State);
  if (!(numerr))
  {
    if (tests_state[charger] & MAIN_CHARGE_pos)
    {
      LEDsBuffer[charger][0]=0xbf;
      LEDsBuffer[charger][1]=~decode_table[T_MAIN_CHARGE];
      LEDsBuffer[charger][2]=0xbf;
    }
    if (tests_state[charger] & OVERCHARGE_pos)
    {
      LEDsBuffer[charger][0]=0xbf;
      LEDsBuffer[charger][1]=~decode_table[T_OVERCHARGE];
      LEDsBuffer[charger][2]=0xbf;
    }
    if (tests_state[charger] & ELECTROLYTE_CHECK_pos)
    {
      LEDsBuffer[charger][0]=0xbf;
      LEDsBuffer[charger][1]=~decode_table[T_ELECTROLYTE_CHECK];
      LEDsBuffer[charger][2]=0xbf;
    }
    if (tests_state[charger] & SPECIAL_TEST_pos)
    {
      LEDsBuffer[charger][0]=0xbf;
      LEDsBuffer[charger][1]=~decode_table[T_SPECIAL_TEST];
      LEDsBuffer[charger][2]=0xbf;
    }
    if (tests_state[charger] & INITIAL_DISCHARGE_pos)
    {
      LEDsBuffer[charger][0]=0xbf;
      LEDsBuffer[charger][1]=~decode_table[T_INITIAL_DISCHARGE];
      LEDsBuffer[charger][2]=0xbf;
    }
    if (tests_state[charger] & CAPACITY_TEST_pos)
    {
      LEDsBuffer[charger][0]=0xbf;
      LEDsBuffer[charger][1]=~decode_table[T_CAPACITY_TEST];
      LEDsBuffer[charger][2]=0xbf;
    }
    if (tests_state[charger] & RESIDUAL_DISCHARGE_pos)
    {
      LEDsBuffer[charger][0]=0xbf;
      LEDsBuffer[charger][1]=~decode_table[T_RESIDUAL_DISCHARGE];
      LEDsBuffer[charger][2]=0xbf;
    }
    
    eState = eTaskGetState(xADC);
    
    if (eState != eSuspended)
    {
      if (tmp32&0x80000000)       // если напряжение отрицательное, то
      {
        voltage=(-tmp32)/100;       // получаем значение напряжение на батарее в десятых долях вольта
        LEDsBuffer[charger][5]=0xbf;      // выводим на экран знак "-"
        temp=(uint8_t)(voltage/100);
        if (temp)
        {
          LEDsBuffer[charger][3]=0xbf;
          LEDsBuffer[charger][4]=0xbf;
        }
        temp = (uint8_t)(voltage/10);
        LEDsBuffer[charger][4]=(~decode_table[temp])&0x7f;
        voltage = voltage-temp*10;
        temp = (uint8_t)voltage;
        LEDsBuffer[charger][3]=~decode_table[temp];
        if (LEDsBuffer[charger][3]==0xc0 && LEDsBuffer[charger][4]==0x40)
          LEDsBuffer[charger][5]=0xff;    // если напряжение -0.0, убираем знак "-"
      }
      else                                // если напряжение положительное, то
      {
        voltage=tmp32/100;    // получаем значение напряжение на батарее в десятых долях вольта
        temp=(uint8_t)(voltage/100);
        if (temp)
        {
          LEDsBuffer[charger][5]=~decode_table[temp];
        }
        else
        {
          LEDsBuffer[charger][5]=0xff;
        }
        voltage = voltage-temp*100;
        temp = (uint8_t)(voltage/10);
        LEDsBuffer[charger][4]=(~decode_table[temp])&0x7f;
        voltage = voltage-temp*10;
        temp = (uint8_t)voltage;
        LEDsBuffer[charger][3]=~decode_table[temp];
      }
    }
    else
    {
      LEDsBuffer[charger][3]=0xbf;
      LEDsBuffer[charger][4]=0xbf;
      LEDsBuffer[charger][5]=0xbf;
    }
  }
  else
  {
    LEDsBuffer[charger][2]=0xff;
    temp=GetNumErrPeriph(Periph_State);
    if (temp<10)
    {
      LEDsBuffer[charger][0]=~decode_table[temp];
      LEDsBuffer[charger][1]=0xff;
    }
    else
    {
      LEDsBuffer[charger][0]=~decode_table[temp%10];
      LEDsBuffer[charger][1]=~decode_table[temp/10];
    }
    LEDsBuffer[charger][3]=~0x50;        // r
    LEDsBuffer[charger][4]=~0x50;        // r
    LEDsBuffer[charger][5]=~0x79;        // E
  }
}

void vLEDs(void *params)
{
  portTickType xLastWakeTime;
  
  xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    if (ChargeState)
    {
      if (ChargeState&BATTERY1)
      {
        ConvDataToLEDarr(CHARGER1);
      }
      else
      {
        uint8_t i;
        for (i=0; i<LEDS; i++)
        {
          LEDsBuffer[CHARGER1][i]=0xFF;
        }
      }
      while (DMA1_Stream4->CR & DMA_SxCR_EN)
      {
        vTaskDelay(1);
      }
      DMA1_Stream4->CR &= ~((uint32_t) DMA_SxCR_EN | DMA_SxCR_CHSEL);
      DMA1_Stream4->PAR = (uint32_t)(&(SPI2->DR));
  //    DMA1_Stream4->CR |= DMA_Channel_0;
      DMA1_Stream4->M0AR =  (uint32_t) &LEDsBuffer[0][0];
      DMA1_Stream4->NDTR = (uint32_t) LEDS*2;
      DMA1_Stream4->CR |= (uint32_t) DMA_SxCR_EN;
      
      vTaskDelay(2);

      GPIO_WriteBit(led_upd_GPIO_Port, led_upd_Pin, Bit_RESET);
      vTaskDelay(1);
      GPIO_WriteBit(led_upd_GPIO_Port, led_upd_Pin, Bit_SET);

      if (ChargeState&BATTERY2)
      {
        ConvDataToLEDarr(CHARGER2);
      }
      else
      {
        uint8_t i;
        for (i=0; i<LEDS; i++)
        {
          LEDsBuffer[CHARGER2][i]=0xFF;
        }
      }
      while (DMA1_Stream4->CR & DMA_SxCR_EN)
      {
        vTaskDelay(1);
      }
/*      DMA1_Stream4->CR &= ~(uint32_t) DMA_SxCR_EN;
      DMA1_Stream4->CR &= ~DMA_SxCR_CHSEL;
      DMA1_Stream4->PAR = (uint32_t)(&(SPI2->DR));
      DMA1_Stream4->M0AR =  (uint32_t) &LEDsBuffer[CHARGER2][0];
      DMA1_Stream4->NDTR = (uint32_t) LEDS;
      DMA1_Stream4->CR |= (uint32_t) DMA_SxCR_EN;

      vTaskDelay(1);
      
      GPIO_WriteBit(led_CS2_GPIO_Port, led_CS2_Pin, Bit_SET);
      vTaskDelay(1);
      GPIO_WriteBit(led_CS2_GPIO_Port, led_CS2_Pin, Bit_RESET);
      */
    }
    else
    {
      NetStateToLED();
    }
//    uint32_t temp_free_size=xPortGetFreeHeapSize();
//    vTaskList(buf);
//    printf(buf);

    vTaskDelayUntil(&xLastWakeTime, 1000);
  }
}

void vWatch1(void *params)
{
  portTickType xLastWakeTime;
  
  xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    if (watch[CHARGER1].State==START)
    {
      if (watch[CHARGER1].Mode==WATCH)
      {
        watch[CHARGER1].Sec++;
        if (watch[CHARGER1].Sec>=60)
        {
          watch[CHARGER1].Sec=0;
          watch[CHARGER1].Min++;
          if (watch[CHARGER1].Min>=60)
          {
            watch[CHARGER1].Min=0;
            watch[CHARGER1].Hour++;
            if (watch[CHARGER1].Hour>=100)
            {
              watch[CHARGER1].Hour--;
              watch[CHARGER1].Min=59;
              watch[CHARGER1].Sec=59;
              watch[CHARGER1].State=0;
            }
          }
        }
      }
      else
      {
        watch[CHARGER1].Sec--;
        if (watch[CHARGER1].Sec>=60)
        {
          watch[CHARGER1].Sec=59;
          watch[CHARGER1].Min--;
          if (watch[CHARGER1].Min>=60)
          {
            watch[CHARGER1].Min=59;
            watch[CHARGER1].Hour--;
            if (watch[CHARGER1].Hour>=100)
            {
              watch[CHARGER1].Hour--;
              watch[CHARGER1].Min=0;
              watch[CHARGER1].Sec=0;
              watch[CHARGER1].State=0;
            }
          }
        }
      }
    }
    vTaskDelayUntil(&xLastWakeTime, 1000);
  }
}

void vWatch2(void *params)
{
  portTickType xLastWakeTime;
  
  xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    if (watch[CHARGER2].State==SET)
    {
//      watch[CHARGER2].mSec++;
//      if (watch[CHARGER2].mSec>=1000)
//      {
//        watch[CHARGER2].mSec=0;
        watch[CHARGER2].Sec++;
        if (watch[CHARGER2].Sec>=60)
        {
          watch[CHARGER2].Sec=0;
          watch[CHARGER2].Min++;
          if (watch[CHARGER2].Min>=60)
          {
            watch[CHARGER2].Min=0;
            watch[CHARGER2].Hour++;
            if (watch[CHARGER2].Hour>=100)
            {
              watch[CHARGER2].Hour--;
              watch[CHARGER2].Min=59;
              watch[CHARGER2].Sec=59;
              watch[CHARGER2].State=0;
            }
          }
//        }
      }
    }
    vTaskDelayUntil(&xLastWakeTime, 1000);
  }
}

void NetStateToLED(void)
{
  extern eeprom_t eeprom_data;
  uint8_t tmp1=0, tmp2=0, tmp3=0, tmp_ip=0;
  LEDsBuffer[CHARGER1][9]=~0x5E;       // d
  LEDsBuffer[CHARGER1][8]=~0x76;       // H
  LEDsBuffer[CHARGER1][7]=~0x39;       // C
  LEDsBuffer[CHARGER1][6]=~0x73;       // P
  LEDsBuffer[CHARGER2][9]=0xFF;        // ' '
  if (!eeprom_data.eeprom_dhcp)
  {
    LEDsBuffer[CHARGER2][8]=0xFF;       // ' '
    LEDsBuffer[CHARGER2][7]=~0x5C;      // o
    LEDsBuffer[CHARGER2][6]=~0x54;      // n
    
// конвертируем первый байт адреса
    tmp_ip=(uint8_t)(ethernetBridge.m_ip.addr&0x000000ff);
    ByteHexToDec(tmp_ip, tmp1, tmp2, tmp3);
    if (!tmp1)
    {
      LEDsBuffer[CHARGER1][5]=0xFF;
    }
    else
    {
      LEDsBuffer[CHARGER1][5]=~decode_table[tmp1];
    }
    if (!tmp2)
    {
      LEDsBuffer[CHARGER1][4]=0xFF;
    }
    else
    {
      LEDsBuffer[CHARGER1][4]=~decode_table[tmp2];
    }
    LEDsBuffer[CHARGER1][3]=~(decode_table[tmp3]|0x80);

// конвертируем второй байт адреса
    tmp_ip=(ethernetBridge.m_ip.addr&0x0000ff00)>>8;
    ByteHexToDec(tmp_ip, tmp1, tmp2, tmp3);
    if (!tmp1)
    {
      LEDsBuffer[CHARGER2][5]=0xFF;
    }
    else
    {
      LEDsBuffer[CHARGER2][5]=~decode_table[tmp1];
    }
    if (!tmp2)
    {
      LEDsBuffer[CHARGER2][4]=0xFF;
    }
    else
    {
      LEDsBuffer[CHARGER2][4]=~decode_table[tmp2];
    }
    LEDsBuffer[CHARGER2][3]=~(decode_table[tmp3]|0x80);
    
// конвертируем третий байт адреса
    tmp_ip=(ethernetBridge.m_ip.addr&0x00ff0000)>>16;
    ByteHexToDec(tmp_ip, tmp1, tmp2, tmp3);
    if (!tmp1)
    {
      LEDsBuffer[CHARGER1][2]=0xFF;
    }
    else
    {
      LEDsBuffer[CHARGER1][2]=~decode_table[tmp1];
    }
    if (!tmp2)
    {
      LEDsBuffer[CHARGER1][1]=0xFF;
    }
    else
    {
      LEDsBuffer[CHARGER1][1]=~decode_table[tmp2];
    }
    LEDsBuffer[CHARGER1][0]=~(decode_table[tmp3]|0x80);

// конвертируем четвёртый байт адреса
    tmp_ip=(ethernetBridge.m_ip.addr&0xff000000)>>24;
    ByteHexToDec(tmp_ip, tmp1, tmp2, tmp3);
    if (!tmp1)
    {
      LEDsBuffer[CHARGER2][2]=0xFF;
    }
    else
    {
      LEDsBuffer[CHARGER2][2]=~decode_table[tmp1];
    }
    if (!tmp2)
    {
      LEDsBuffer[CHARGER2][1]=0xFF;
    }
    else
    {
      LEDsBuffer[CHARGER2][1]=~decode_table[tmp2];
    }
    LEDsBuffer[CHARGER2][0]=~(decode_table[tmp3]|0x80);
  }
  else
  {
    LEDsBuffer[CHARGER2][8]=~0x5C;      // o
    LEDsBuffer[CHARGER2][7]=~0x71;      // f
    LEDsBuffer[CHARGER2][6]=~0x71;      // f
  }

  vTaskDelay(1);

  DMA1_Stream4->CR &= ~((uint32_t) DMA_SxCR_EN | DMA_SxCR_CHSEL);
  DMA1_Stream4->PAR = (uint32_t)(&(SPI2->DR));
  DMA1_Stream4->M0AR =  (uint32_t) &LEDsBuffer[0][0];
  DMA1_Stream4->NDTR = (uint32_t) LEDS*2;
  DMA1_Stream4->CR |= (uint32_t) DMA_SxCR_EN;

  vTaskDelay(2);

//  GPIO_WriteBit(led_CS1_GPIO_Port, led_CS1_Pin, Bit_SET);
  GPIO_WriteBit(led_upd_GPIO_Port, led_upd_Pin, Bit_RESET);
  vTaskDelay(1);
//  GPIO_WriteBit(led_CS1_GPIO_Port, led_CS1_Pin, Bit_RESET);
  GPIO_WriteBit(led_upd_GPIO_Port, led_upd_Pin, Bit_SET);

/*  DMA1_Stream4->CR &= ~(uint32_t) DMA_SxCR_EN;
  DMA1_Stream4->CR &= ~DMA_SxCR_CHSEL;
  DMA1_Stream4->PAR = (uint32_t)(&(SPI2->DR));
  DMA1_Stream4->M0AR =  (uint32_t) &LEDsBuffer[CHARGER2][0];
  DMA1_Stream4->NDTR = (uint32_t) LEDS;
  DMA1_Stream4->CR |= (uint32_t) DMA_SxCR_EN;

  vTaskDelay(1);
      
  GPIO_WriteBit(led_CS2_GPIO_Port, led_CS2_Pin, Bit_SET);
  vTaskDelay(1);
  GPIO_WriteBit(led_CS2_GPIO_Port, led_CS2_Pin, Bit_RESET);
  */
}

/*
 ===============================================================================
Преобразование 16-ричного числа (байт) в десятичное (3 байта- сотни, десятки, единицы)                  
 ===============================================================================  
 */
void ByteHexToDec(uint8_t hex, uint8_t& dech, uint8_t& dect, uint8_t& deco)
{
  dech = (uint8_t)(hex/100);
  dect = (uint8_t)((hex-dech*100)/10);
  deco = (uint8_t)(hex-dech*100-dect*10);
}

/*
 ===============================================================================
                          Подсчёт количества бит                   
 ===============================================================================  
 */
uint8_t CountOnes(uint32_t n)
{
  n -= (n>>1) & 0x55555555;
  n = ((n>>2) & 0x33333333 ) + (n & 0x33333333);
  n = ((((n>>4) + n) & 0x0F0F0F0F) * 0x01010101) >> 24;
  return n;     // Здесь происходит неявное обрезание по 8 младшим битам.
}

/*
 ===============================================================================
                          Получение номера ошибки                   
 ===============================================================================  
 */
uint8_t GetNumErrPeriph(uint32_t n)
{
  uint8_t i=0;
  while (i<33)
  {
    if (n & (1<<curr_err_pos))
    {
      n=curr_err_pos+1;
      if (curr_err_pos>=31) curr_err_pos=0;
      else curr_err_pos++;
      return n; // Здесь происходит неявное обрезание по 8 младшим битам.
    }
    i++;
    if (curr_err_pos>=31) curr_err_pos=0;
    else curr_err_pos++;
  }
  return 255;
}

extern "C" {

/*  
void DMA1_Stream4_IRQHandler()
{
    if (DMA_GetITStatus(DMA1_Stream4, DMA_IT_TCIF4))
    {
        DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF4);
    }
    if (DMA_GetITStatus(DMA1_Stream4, DMA_IT_HTIF4))
    {
        DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_HTIF4);
    }
    if (DMA_GetITStatus(DMA1_Stream4, DMA_IT_TEIF4))
    {
        DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TEIF4);
    }
    if (DMA_GetITStatus(DMA1_Stream4, DMA_IT_FEIF4))
    {
        DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_FEIF4);
    }
    if (DMA_GetITStatus(DMA1_Stream4, DMA_IT_DMEIF4))
    {
        DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_DMEIF4);
    }
}  
*/

}