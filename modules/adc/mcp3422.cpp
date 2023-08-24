#include "stm32f4xx.h"
#include "mcp3422.h"
#include "24AA025E48.h"
#include "leds.h"
#include "ach_adch.h"
#include "periph_watchdog.h"
#include "FreeRTOS.h"
#include "task.h"

//#define DEBUG_PIN_ENABLE

extern uint32_t Periph_State;   // регистр состояния периферии
TaskHandle_t xADC;
uint8_t MCP3422BufferTx[MCP3422_TX_LENGHT];
uint8_t MCP3422BufferRx[MCP3422_DATA18_LENGHT];
uint8_t eeprom_arr[EEPROM_T_SIZE];
uint8_t MAC_24AA025E48[6];
uint8_t eeprom_dhcp=0;
uint8_t i2c_data, i2c_phase;
uint8_t mcp3422_sr=0;   // TIM7_TIMEOUT, DMA_ERROR
uint8_t dma_tc_sr=0;    // данные через DMA приняты/переданы
uint8_t MCP3422_Calc_Fail, MCP3422_Calc_ReInit;
uint8_t MCP3422_State;  // OK или FAIL
int32_t Vbat[BAT_QUANTITY];     // переменные со значением напряжения на батареях
MCP3424_Meas_TypeDef MCP3424_data;      // данные с микросхемы АЦП MCP3424
eeprom_t eeprom_data;

/* предварительно необходимо включить периферию: GPIOB, I2C1, DMA1, TIM7 */
void ADC_Init(void)
{
  I2C_InitTypeDef hi2c1;
  GPIO_InitTypeDef hgpio;
  DMA_InitTypeDef  hdma;
  NVIC_InitTypeDef hnvic;
  
    /**I2C1 GPIO Configuration    
    PB8     ------> I2C1_SCL
    PB9     ------> I2C1_SDA 
    PH9    ------> отладочный GPIO 
    */

#ifdef DEBUG_PIN_ENABLE
  hgpio.GPIO_Pin = GPIO_Pin_9;
  hgpio.GPIO_Mode = GPIO_Mode_OUT;
  hgpio.GPIO_OType = GPIO_OType_PP;
  hgpio.GPIO_Speed = GPIO_Speed_100MHz;
  hgpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOH, &hgpio);
  GPIO_WriteBit(GPIOH, GPIO_Pin_10, Bit_RESET);
#endif

  hgpio.GPIO_Pin = adc_scl_Pin | adc_sda_Pin;
  hgpio.GPIO_Mode = GPIO_Mode_AF;
  hgpio.GPIO_OType = GPIO_OType_OD;
  hgpio.GPIO_Speed = GPIO_Speed_25MHz;
  hgpio.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(adc_scl_GPIO_Port, &hgpio);
  
  GPIO_PinAFConfig(adc_scl_GPIO_Port, GPIO_PinSource8, GPIO_AF_I2C1);
  GPIO_PinAFConfig(adc_sda_GPIO_Port, GPIO_PinSource9, GPIO_AF_I2C1);

  I2C_DeInit(MCP3422_I2C);
  hi2c1.I2C_ClockSpeed = 400000;
  hi2c1.I2C_DutyCycle = I2C_DutyCycle_2;
  hi2c1.I2C_OwnAddress1 = 0;
  hi2c1.I2C_Ack = I2C_Ack_Enable;
  hi2c1.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  hi2c1.I2C_Mode = I2C_Mode_I2C;
  I2C_Init(MCP3422_I2C, &hi2c1);
  I2C_Cmd(MCP3422_I2C, ENABLE);
//  NVIC_EnableIRQ(I2C3_EV_IRQn);
//  NVIC_SetPriority(I2C3_EV_IRQn, 1);
  
// настройка TIM7 для таймаутов
  
  uint32_t temp=(((RCC->CFGR)>>10)&0x00000007); // получаем значение предделителя частоты шины AHB
  uint8_t ppre1=0;
  if ((temp>=4)&&(temp<=7))
  {
    switch (temp)
    {
    case 4:             // если значение равно 4,
      ppre1=2;          // значит частота AHB поделена на 2
      break;
    case 5:             // если значение равно 5,
      ppre1=4;          // значит частота AHB поделена на 4
      break;
    case 6:             // если значение равно 6,
      ppre1=8;          // значит частота AHB поделена на 8
      break;
    case 7:             // если значение равно 7,
      ppre1=16;         // значит частота AHB поделена на 16
      break;
    }
  }
  TIM7->PSC = (uint16_t)(SystemCoreClock / ppre1 / 1000000)-1;  // предделитель счётчика настраиваем на частоту 1 МГц
  TIM7->CR1 = TIM_CR1_OPM | TIM_CR1_URS;        //одноимпульсный режим, прерывание только по переполнению
  TIM7->DIER = TIM_DIER_UIE;
  NVIC_EnableIRQ(TIM7_IRQn);
  NVIC_SetPriority(TIM7_IRQn, 1);

// Настройка DMA для I2C1_TX
  DMA_DeInit(DMA1_Stream6);
  hdma.DMA_Channel = MCP3422_DMA_Channel;
  hdma.DMA_PeripheralBaseAddr = (uint32_t)(&(MCP3422_I2C->DR));
  hdma.DMA_Memory0BaseAddr = (uint32_t)&MCP3422BufferTx[0];
  hdma.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  hdma.DMA_BufferSize = MCP3422_TX_LENGHT;
  hdma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  hdma.DMA_MemoryInc = DMA_MemoryInc_Enable;
  hdma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  hdma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  hdma.DMA_Mode = DMA_Mode_Normal;
  hdma.DMA_Priority = DMA_Priority_High;
  hdma.DMA_FIFOMode = DMA_FIFOMode_Disable;
  hdma.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  hdma.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  hdma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA1_Stream6, &hdma);
  
  hnvic.NVIC_IRQChannel = DMA1_Stream6_IRQn;
  hnvic.NVIC_IRQChannelPreemptionPriority = 0;
  hnvic.NVIC_IRQChannelSubPriority = 0;
  hnvic.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&hnvic);
    
  DMA_ITConfig(DMA1_Stream6, DMA_IT_TC | DMA_IT_TE, ENABLE);

//  I2C_DMACmd(I2C1, ENABLE);
//  DMA_Cmd(DMA1_Stream6, DISABLE);
  
  MCP3422BufferTx[MCP3422_ADDR] = MCP3422_CH1;
  MCP3422BufferTx[MCP3422_CONF] = MCP3422_CONT_CONV | MCP3422_18BIT | MCP3422_GAIN1;

//  DMA_Cmd(DMA1_Stream4, ENABLE);

// Настройка DMA для I2C1_RX
  DMA_DeInit(DMA1_Stream0);
  hdma.DMA_Channel = MCP3422_DMA_Channel;
  hdma.DMA_PeripheralBaseAddr = (uint32_t)(&(I2C3->DR));
  hdma.DMA_Memory0BaseAddr = (uint32_t)&MCP3422BufferRx[0];
  hdma.DMA_DIR = DMA_DIR_PeripheralToMemory;
  hdma.DMA_BufferSize = MCP3422_DATA12_LENGHT;
  hdma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  hdma.DMA_MemoryInc = DMA_MemoryInc_Enable;
  hdma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  hdma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  hdma.DMA_Mode = DMA_Mode_Normal;
  hdma.DMA_Priority = DMA_Priority_High;
  hdma.DMA_FIFOMode = DMA_FIFOMode_Disable;
  hdma.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  hdma.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  hdma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA1_Stream0, &hdma);
  
  hnvic.NVIC_IRQChannel = DMA1_Stream0_IRQn;
  hnvic.NVIC_IRQChannelPreemptionPriority = 0;
  hnvic.NVIC_IRQChannelSubPriority = 0;
  hnvic.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&hnvic);
    
  DMA_ITConfig(DMA1_Stream0, DMA_IT_TC | DMA_IT_TE, ENABLE);
  
//  I2C_DMACmd(I2C1, ENABLE);
//  DMA_Cmd(DMA1_Stream6, ENABLE);
//  MCP3422_Calc_Fail = 0;        // счётчик оошибок шины I2C
}

ErrorStatus ADC_Config(void)
{
  if (I2C_GetFlagStatus_timer(MCP3422_I2C, I2C_FLAG_BUSY, TIMEOUT_BUS)==ERROR) return ERROR;
  
  /* Send START condition */
  I2C_GenerateSTART(MCP3422_I2C, ENABLE);

  /* Test on EV5 and clear it */
  if (I2C_CheckEvent_timer(MCP3422_I2C, I2C_EVENT_MASTER_MODE_SELECT, TIMEOUT_START)==ERROR) return ERROR;

  /* Send MCP3422 address for write */
  I2C_Send7bitAddress(MCP3422_I2C, MCP3422_DEVICE_WRITE_ADDRESS, I2C_Direction_Transmitter); 
  
  /* Test on EV6 and clear it */
  if (I2C_CheckEvent_timer(MCP3422_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, TIMEOUT_ADDR)==ERROR) return ERROR;
  
  /* Send the MCP3422's configuration byte */
  I2C_SendData(MCP3422_I2C, MCP3422_CH1 | MCP3422_CONT_CONV | MCP3422_18BIT | MCP3422_GAIN1 | MCP3422_START_CONV);
  
  /* Test on EV8 and clear it */
  if (I2C_CheckEvent_timer(MCP3422_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED, TIMEOUT_DATA_TX)==ERROR) return ERROR;
  
  /* Send I2C3 STOP Condition */
  I2C_GenerateSTOP(MCP3422_I2C, ENABLE);
  
  if (I2C_GetFlagStatus_timer(MCP3422_I2C, I2C_FLAG_BUSY, TIMEOUT_STOP)==ERROR) return ERROR;
  
  MCP3422_Calc_Fail = 0;        // обнуляем счётчик оошибок шины I2C
  return SUCCESS;
}

ErrorStatus ADC_Set_Channel(uint8_t channel)
{
  if (I2C_GetFlagStatus_timer(MCP3422_I2C, I2C_FLAG_BUSY, TIMEOUT_BUS)==ERROR) return ERROR;
  
  /* Send START condition */
  I2C_GenerateSTART(MCP3422_I2C, ENABLE);

  /* Test on EV5 and clear it */
  if (I2C_CheckEvent_timer(MCP3422_I2C, I2C_EVENT_MASTER_MODE_SELECT, TIMEOUT_START)==ERROR) return ERROR;

  /* Send MCP3422 address for write */
  I2C_Send7bitAddress(MCP3422_I2C, MCP3422_DEVICE_WRITE_ADDRESS, I2C_Direction_Transmitter); 
  
  /* Test on EV6 and clear it */
  if (I2C_CheckEvent_timer(MCP3422_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, TIMEOUT_ADDR)==ERROR) return ERROR;
  
  /* Send the MCP3422's configuration byte */
  I2C_SendData(MCP3422_I2C, channel | MCP3422_CONT_CONV | MCP3422_18BIT | MCP3422_GAIN1 | MCP3422_START_CONV);
  
  /* Test on EV8 and clear it */
  if (I2C_CheckEvent_timer(MCP3422_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED, TIMEOUT_DATA_TX)==ERROR) return ERROR;
  
  /* Send I2C3 STOP Condition */
  I2C_GenerateSTOP(MCP3422_I2C, ENABLE);
  
  if (I2C_GetFlagStatus_timer(MCP3422_I2C, I2C_FLAG_BUSY, TIMEOUT_STOP)==ERROR) return ERROR;
  
  MCP3422_Calc_Fail = 0;        // обнуляем счётчик оошибок шины I2C
  return SUCCESS;
}

ErrorStatus Read_ADC(uint8_t *buffer)
{
    I2C_DMACmd(MCP3422_I2C, DISABLE);
    NVIC_EnableIRQ(I2C1_EV_IRQn);
    
    /* Ждём, когда шина освободится */
    if (I2C_GetFlagStatus_timer(MCP3422_I2C, I2C_FLAG_BUSY, TIMEOUT_BUS)==ERROR) return ERROR;
    
    /* Enable DMA NACK automatic generation */
    I2C_DMALastTransferCmd(MCP3422_I2C, ENABLE);                    //Note this one, very important
    
    /* Send START condition */
    I2C_GenerateSTART(MCP3422_I2C, ENABLE);

    /* Test on EV5 and clear it */
    if (I2C_CheckEvent_timer(MCP3422_I2C, I2C_EVENT_MASTER_MODE_SELECT, TIMEOUT_START)==ERROR) return ERROR;

    /* Send MCP3422 address for read */
    I2C_Send7bitAddress(MCP3422_I2C, MCP3422_DEVICE_READ_ADDRESS, I2C_Direction_Receiver);
    
    /* Test on EV6 and clear it */
    if (I2C_CheckEvent_timer(MCP3422_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, TIMEOUT_ADDR)==ERROR) return ERROR;
    
    NVIC_DisableIRQ(I2C1_EV_IRQn);
    /* Start DMA to receive data from I2C */
    I2C_DMACmd(EEPROM_I2C, ENABLE);
    if (I2C_DMARead_timer(MCP3422_I2C, DMA_Channel_1, MCP3422_DATA18_LENGHT, buffer, TIMEOUT_DATA_RX*MCP3422_DATA18_LENGHT)==ERROR) return ERROR;
      
    /* Send I2C3 STOP Condition */
    I2C_GenerateSTOP(MCP3422_I2C, ENABLE);

    if (I2C_GetFlagStatus_timer(MCP3422_I2C, I2C_FLAG_BUSY, TIMEOUT_ADDR*10)==ERROR) return ERROR;

  
  MCP3422_Calc_Fail = 0;        // обнуляем счётчик оошибок шины I2C
  return SUCCESS;
}

void vADC(void *params)
{
  extern TaskHandle_t xADC;
  for (;;)
  {
    if (ADC_Set_Channel(MCP3422_CH1)==SUCCESS)
    {
    vTaskDelay(300);
      if (Read_ADC(MCP3422BufferRx)==ERROR) vTaskSuspend(xADC);
    }
    else
    {
      vTaskSuspend(xADC);
    }
    
//    vTaskDelay(500);

    if (ADC_Set_Channel(MCP3422_CH2)==SUCCESS)
    {
      vTaskDelay(300);
      if (Read_ADC(MCP3422BufferRx)==ERROR) vTaskSuspend(xADC);
    }
    else
    {
      vTaskSuspend(xADC);
    }

    if (ADC_Set_Channel(MCP3424_CH3)==SUCCESS)
    {
      vTaskDelay(300);
      if (Read_ADC(MCP3422BufferRx)==ERROR) vTaskSuspend(xADC);
    }
    else
    {
      vTaskSuspend(xADC);
    }

    if (ADC_Set_Channel(MCP3424_CH4)==SUCCESS)
    {
      vTaskDelay(300);
      if (Read_ADC(MCP3422BufferRx)==ERROR) vTaskSuspend(xADC);
    }
    else
    {
      vTaskSuspend(xADC);
    }

#ifdef DEBUG_PIN_ENABLE
    GPIOH->BSRRH = GPIO_Pin_9;
#endif
    vTaskDelay(500);
  }
}

ErrorStatus EEPROM_Read(uint8_t eeprom_addr, uint8_t buf_len, uint8_t *read_addr)
{
  NVIC_EnableIRQ(I2C1_EV_IRQn);
  if (I2C_GetFlagStatus_timer(EEPROM_I2C, I2C_FLAG_BUSY, TIMEOUT_BUS)==ERROR) return ERROR;
  
  /* Send START condition */
  I2C_GenerateSTART(EEPROM_I2C, ENABLE);

  /* Test on EV5 and clear it */
  if (I2C_CheckEvent_timer(EEPROM_I2C, I2C_EVENT_MASTER_MODE_SELECT, TIMEOUT_START)==ERROR) return ERROR;

  /* Send 24AA025E48 address for write */
  I2C_Send7bitAddress(EEPROM_I2C, EEPROM_DEVICE_WRITE_ADDRESS, I2C_Direction_Transmitter); 
  
  /* Test on EV6 and clear it */
  if (I2C_CheckEvent_timer(EEPROM_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, TIMEOUT_ADDR*100)==ERROR) return ERROR;
  
  /* Send the 24AA025E48's internal address to write to */
  I2C_SendData(EEPROM_I2C, eeprom_addr);
  
  /* Test on EV8 and clear it */
  if (I2C_CheckEvent_timer(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED, TIMEOUT_DATA_TX*3)==ERROR) return ERROR;
  
  /* Enable DMA NACK automatic generation */
  I2C_DMALastTransferCmd(MCP3422_I2C, ENABLE);                    //Note this one, very important
    
  /* Send START condition */
  I2C_GenerateSTART(EEPROM_I2C, ENABLE);

  /* Test on EV5 and clear it */
  if (I2C_CheckEvent_timer(EEPROM_I2C, I2C_EVENT_MASTER_MODE_SELECT, TIMEOUT_START)==ERROR) return ERROR;

  /* Send 24AA025E48 address for read */
  I2C_Send7bitAddress(EEPROM_I2C, EEPROM_DEVICE_READ_ADDRESS, I2C_Direction_Receiver); 
  
  /* Test on EV6 and clear it */
  if (I2C_CheckEvent_timer(EEPROM_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, TIMEOUT_ADDR)==ERROR) return ERROR;
  
  NVIC_DisableIRQ(I2C3_EV_IRQn);
  /* Start DMA to receive data from I2C */
  I2C_DMACmd(EEPROM_I2C, ENABLE);
  if (I2C_DMARead_timer(EEPROM_I2C, DMA_Channel_1, buf_len, read_addr, TIMEOUT_DATA_RX*buf_len)==ERROR) return ERROR;
    
  /* Send I2C1 STOP Condition */
  I2C_GenerateSTOP(EEPROM_I2C, ENABLE);

  if (I2C_GetFlagStatus_timer(EEPROM_I2C, I2C_FLAG_BUSY, TIMEOUT_ADDR*10)==ERROR) return ERROR;

#ifdef DEBUG_PIN_ENABLE
  GPIOH->BSRRH = GPIO_Pin_9;
#endif
  MCP3422_Calc_Fail = 0;        // обнуляем счётчик оошибок шины I2C
  return SUCCESS;
}

ErrorStatus I2C_DMARead_timer(I2C_TypeDef* I2Cx, uint32_t DMA_Channel_x, uint32_t buf_len, uint8_t *read_addr, uint16_t timer)
{
  dma_tc_sr &= ~DMA_RECEIVED;
  DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_TCIF0 | DMA_IT_HTIF0 | DMA_IT_TEIF0 | DMA_IT_DMEIF0 | DMA_IT_FEIF0);
  DMA1_Stream0->CR &= ~((uint32_t) DMA_SxCR_EN | DMA_SxCR_CHSEL);
  DMA1_Stream0->CR |= (uint32_t) DMA_Channel_x;
  DMA1_Stream0->PAR = (uint32_t)(&(I2Cx->DR));
  DMA1_Stream0->M0AR =  (uint32_t) read_addr;
  DMA1_Stream0->NDTR = (uint32_t) buf_len;
  DMA1_Stream0->CR |= (uint32_t) DMA_SxCR_EN;
  Timer_I2C(timer);       // запускаем таймер
  while(!(dma_tc_sr & DMA_RECEIVED))
  {
    if (mcp3422_sr==TIM7_TIMEOUT)
    {
      return ERROR;
    }
  }
  TIM7->CR1 &= ~TIM_CR1_CEN;    // если данные приняты до таймаута, останавливаем таймер
#ifdef DEBUG_PIN_ENABLE
  GPIOH->BSRRH = GPIO_Pin_9;
#endif
  return SUCCESS;
}

ErrorStatus EEPROM_Write(uint8_t eeprom_addr, uint8_t buf_len, uint8_t *write_addr)
{
  I2C_DMACmd(EEPROM_I2C, DISABLE);
  NVIC_EnableIRQ(I2C1_EV_IRQn);
  if (I2C_GetFlagStatus_timer(EEPROM_I2C, I2C_FLAG_BUSY, TIMEOUT_BUS)==ERROR) return ERROR;
  
  /* Enable DMA NACK automatic generation */
  I2C_DMALastTransferCmd(EEPROM_I2C, ENABLE);                    //Note this one, very important
    
  /* Send START condition */
  I2C_GenerateSTART(EEPROM_I2C, ENABLE);

  /* Test on EV5 and clear it */
  if (I2C_CheckEvent_timer(EEPROM_I2C, I2C_EVENT_MASTER_MODE_SELECT, TIMEOUT_START)==ERROR) return ERROR;

  /* Send 24AA025E48 address for write */
  I2C_Send7bitAddress(EEPROM_I2C, EEPROM_DEVICE_WRITE_ADDRESS, I2C_Direction_Transmitter); 
  
  /* Test on EV6 and clear it */
  if (I2C_CheckEvent_timer(EEPROM_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, TIMEOUT_ADDR*10)==ERROR) return ERROR;
  
  /* Send the 24AA025E48's internal address to write to */
  I2C_SendData(EEPROM_I2C, eeprom_addr);
  
  /* Test on EV8 and clear it */
  if (I2C_CheckEvent_timer(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED, TIMEOUT_DATA_TX*3)==ERROR) return ERROR;
  
  /* Send START condition */
//  I2C_GenerateSTART(EEPROM_I2C, ENABLE);

  /* Test on EV5 and clear it */
//  if (I2C_CheckEvent_timer(EEPROM_I2C, I2C_EVENT_MASTER_MODE_SELECT, TIMEOUT_START)==ERROR) return ERROR;

  /* Send 24AA025E48 address for read */
//  I2C_Send7bitAddress(EEPROM_I2C, EEPROM_DEVICE_READ_ADDRESS, I2C_Direction_Receiver); 
  
  /* Test on EV6 and clear it */
//  if (I2C_CheckEvent_timer(EEPROM_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, TIMEOUT_ADDR)==ERROR) return ERROR;
  
  NVIC_DisableIRQ(I2C1_EV_IRQn);
  /* Start DMA to transmit data to I2C */
  I2C_DMACmd(EEPROM_I2C, ENABLE);
  if (I2C_DMAWrite_timer(EEPROM_I2C, DMA_Channel_1, buf_len, write_addr, TIMEOUT_DATA_TX*buf_len)==ERROR) return ERROR;
    
  /* Test on EV8 and clear it */
  if (I2C_CheckEvent_timer(EEPROM_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED, TIMEOUT_DATA_TX*3)==ERROR) return ERROR;

  /* Send I2C1 STOP Condition */
  I2C_GenerateSTOP(EEPROM_I2C, ENABLE);

  if (I2C_GetFlagStatus_timer(EEPROM_I2C, I2C_FLAG_BUSY, TIMEOUT_ADDR*10)==ERROR) return ERROR;
  
#ifdef DEBUG_PIN_ENABLE
  GPIOH->BSRRH = GPIO_Pin_9;
#endif
  MCP3422_Calc_Fail = 0;        // обнуляем счётчик оошибок шины I2C
  return SUCCESS;
}

ErrorStatus I2C_DMAWrite_timer(I2C_TypeDef* I2Cx, uint32_t DMA_Channel_x, uint32_t buf_len, uint8_t *write_addr, uint16_t timer)
{
  dma_tc_sr &= ~DMA_TRANSMITTED;
  DMA1_Stream6->CR &= ~((uint32_t) DMA_SxCR_EN | DMA_SxCR_CHSEL);
  DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6 | DMA_IT_HTIF6 | DMA_IT_TEIF6 | DMA_IT_DMEIF6 | DMA_IT_FEIF6);
  DMA1_Stream6->CR |= (uint32_t) DMA_Channel_x;
  DMA1_Stream6->PAR = (uint32_t)(&(I2Cx->DR));
  DMA1_Stream6->M0AR =  (uint32_t) write_addr;
  DMA1_Stream6->NDTR = (uint32_t) buf_len;
  DMA1_Stream6->CR |= (uint32_t) DMA_SxCR_EN;
  Timer_I2C(timer);       // запускаем таймер
  while(!(dma_tc_sr & DMA_TRANSMITTED))
  {
    if (mcp3422_sr==TIM7_TIMEOUT)
    {
      return ERROR;
    }
  }
  TIM7->CR1 &= ~TIM_CR1_CEN;    // если данные приняты до таймаута, останавливаем таймер
//  I2C3->CR2 &= (uint16_t)~((uint16_t)I2C_CR2_DMAEN);
//  I2C3->CR1 |= I2C_CR1_STOP;      // Send I2C3 STOP Condition
//  DMA1_Stream4->CR &= (uint32_t)~((uint32_t)DMA_SxCR_EN);   //Disable DMA channel
#ifdef DEBUG_PIN_ENABLE
  GPIOH->BSRRH = GPIO_Pin_9;
#endif
  return SUCCESS;
}

ErrorStatus I2C_GetFlagStatus_timer(I2C_TypeDef* I2Cx, uint32_t I2C_FLAG, uint16_t timer)
{
  Timer_I2C(timer);       // запускаем таймер
  while(I2C_GetFlagStatus(I2Cx, I2C_FLAG))
  {
    if (mcp3422_sr==TIM7_TIMEOUT)
    {
      return ERROR;
    }
  }
  TIM7->CR1 &= ~TIM_CR1_CEN;    // если шина i2c ок, останавливаем таймер
#ifdef DEBUG_PIN_ENABLE
  GPIOH->BSRRH = GPIO_Pin_9;
#endif
  return SUCCESS;
}

//I2C_CheckEvent_timer
ErrorStatus I2C_CheckEvent_timer(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT, uint16_t timer)
{
  Timer_I2C(timer);       // запускаем таймер
    /* Ждём события */
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT))
  {
    if (mcp3422_sr==TIM7_TIMEOUT)
    {
      return ERROR;
    }
  }
  TIM7->CR1 &= ~TIM_CR1_CEN;    // если шина i2c ок, останавливаем таймер
#ifdef DEBUG_PIN_ENABLE
  GPIOH->BSRRH = GPIO_Pin_9;
#endif
  return SUCCESS;
}

void Timer_I2C(uint16_t timer_us)
{
  TIM7->CNT = 0;
  TIM7->ARR = timer_us;
  mcp3422_sr = RESET;           // сбрасываем флаг срабатывания таймера
#ifdef DEBUG_PIN_ENABLE
  GPIOH->BSRRL = GPIO_Pin_9;
#endif
  TIM7->CR1 |= TIM_CR1_CEN;     // Запускаем таймер
}

void Load_Net_Config(void)
{
  uint8_t temp_cs=0;
  if (EEPROM_Read(EEPROM_START_ADDR_DATA, EEPROM_T_SIZE, eeprom_arr)!=ERROR)
  {
    Periph_State &= ~PS_EEPROM_STATE;
    for (uint8_t i=0; i<EEPROM_T_SIZE-1; i++) temp_cs ^= eeprom_arr[i];
    if (temp_cs == eeprom_arr[EEPROM_T_SIZE-1])
    {
      eeprom_data.eeprom_key = eeprom_arr[0] | (eeprom_arr[1]<<8) | (eeprom_arr[2]<<16) | (eeprom_arr[3]<<24);
      eeprom_data.eeprom_dhcp = eeprom_arr[4];
      eeprom_data.eeprom_ip = eeprom_arr[8] | (eeprom_arr[7]<<8) | (eeprom_arr[6]<<16) | (eeprom_arr[5]<<24);
      eeprom_data.eeprom_mask =  eeprom_arr[12] | (eeprom_arr[11]<<8) | (eeprom_arr[10]<<16) | (eeprom_arr[9]<<24);
      eeprom_data.eeprom_gate =  eeprom_arr[16] | (eeprom_arr[15]<<8) | (eeprom_arr[14]<<16) | (eeprom_arr[13]<<24);
    }
    else
    {
      eeprom_data.eeprom_key = EEPROM_KEY;
      eeprom_data.eeprom_dhcp = EEPROM_DHCP;
      eeprom_data.eeprom_ip = EEPROM_IP;
      eeprom_data.eeprom_mask =  EEPROM_MASK;
      eeprom_data.eeprom_gate =  EEPROM_GATE;
      eeprom_arr[0] = uint8_t (eeprom_data.eeprom_key);
      eeprom_arr[1] = uint8_t (eeprom_data.eeprom_key>>8);
      eeprom_arr[2] = uint8_t (eeprom_data.eeprom_key>>16);
      eeprom_arr[3] = uint8_t (eeprom_data.eeprom_key>>24);
      eeprom_arr[4] = uint8_t (eeprom_data.eeprom_dhcp);
      eeprom_arr[5] = uint8_t (eeprom_data.eeprom_ip);
      eeprom_arr[6] = uint8_t (eeprom_data.eeprom_ip>>8);
      eeprom_arr[7] = uint8_t (eeprom_data.eeprom_ip>>16);
      eeprom_arr[8] = uint8_t (eeprom_data.eeprom_ip>>24);
      eeprom_arr[9] = uint8_t (eeprom_data.eeprom_mask);
      eeprom_arr[10] = uint8_t (eeprom_data.eeprom_mask>>8);
      eeprom_arr[11] = uint8_t (eeprom_data.eeprom_mask>>16);
      eeprom_arr[12] = uint8_t (eeprom_data.eeprom_mask>>24);
      eeprom_arr[13] = uint8_t (eeprom_data.eeprom_gate);
      eeprom_arr[14] = uint8_t (eeprom_data.eeprom_gate>>8);
      eeprom_arr[15] = uint8_t (eeprom_data.eeprom_gate>>16);
      eeprom_arr[16] = uint8_t (eeprom_data.eeprom_gate>>24);
      temp_cs = 0;
      for (uint8_t i=0; i<EEPROM_T_SIZE-1; i++) temp_cs ^= eeprom_arr[i];
      eeprom_arr[17] = temp_cs;
      if (EEPROM_Write(EEPROM_START_ADDR_DATA, 16, eeprom_arr)!=ERROR) Periph_State &= ~PS_EEPROM_STATE;
      else Periph_State |= PS_EEPROM_STATE;
// запись в EEPROM осуществляется пакетами данных максимум по 16 байт
// Время цикла записи не более 5 мс
      vTaskDelay(EEPROM_WRITE_TIME);
      if (EEPROM_Write(EEPROM_START_ADDR_DATA+16, 2, eeprom_arr+16)!=ERROR) Periph_State &= ~PS_EEPROM_STATE;
      else Periph_State |= PS_EEPROM_STATE;
      vTaskDelay(EEPROM_WRITE_TIME);
    }
  }
  else
  {
    Periph_State |= PS_EEPROM_STATE;
  }
  vTaskDelay(1);
  if (EEPROM_Read(EEPROM_START_ADDR_MAC, MAC_LEN, MAC_24AA025E48)==ERROR) Periph_State |= PS_ETH_STATE | PS_EEPROM_STATE;
}

ErrorStatus Save_Net_Config(void)
{
  uint8_t temp_cs=0;
  eeprom_data.eeprom_key = EEPROM_KEY;
  eeprom_arr[0] = uint8_t (eeprom_data.eeprom_key);
  eeprom_arr[1] = uint8_t (eeprom_data.eeprom_key>>8);
  eeprom_arr[2] = uint8_t (eeprom_data.eeprom_key>>16);
  eeprom_arr[3] = uint8_t (eeprom_data.eeprom_key>>24);
  eeprom_arr[4] = uint8_t (eeprom_data.eeprom_dhcp);
  eeprom_arr[5] = uint8_t (eeprom_data.eeprom_ip);
  eeprom_arr[6] = uint8_t (eeprom_data.eeprom_ip>>8);
  eeprom_arr[7] = uint8_t (eeprom_data.eeprom_ip>>16);
  eeprom_arr[8] = uint8_t (eeprom_data.eeprom_ip>>24);
  eeprom_arr[9] = uint8_t (eeprom_data.eeprom_mask);
  eeprom_arr[10] = uint8_t (eeprom_data.eeprom_mask>>8);
  eeprom_arr[11] = uint8_t (eeprom_data.eeprom_mask>>16);
  eeprom_arr[12] = uint8_t (eeprom_data.eeprom_mask>>24);
  eeprom_arr[13] = uint8_t (eeprom_data.eeprom_gate);
  eeprom_arr[14] = uint8_t (eeprom_data.eeprom_gate>>8);
  eeprom_arr[15] = uint8_t (eeprom_data.eeprom_gate>>16);
  eeprom_arr[16] = uint8_t (eeprom_data.eeprom_gate>>24);
  temp_cs = 0;
  for (uint8_t i=0; i<EEPROM_T_SIZE-1; i++)
  {
    temp_cs ^= eeprom_arr[i];
  }
  eeprom_arr[17] = temp_cs;
// запись в EEPROM осуществляется пакетами данных максимум по 16 байт
// Время цикла записи не более 5 мс
  if (EEPROM_Write(EEPROM_START_ADDR_DATA, 16, eeprom_arr) == ERROR) return ERROR;
  vTaskDelay(EEPROM_WRITE_TIME);
  if (EEPROM_Write(EEPROM_START_ADDR_DATA+16, 2, eeprom_arr+16) == ERROR) return ERROR;
  vTaskDelay(EEPROM_WRITE_TIME);
  return SUCCESS;
}

extern "C" {

void DMA1_Stream0_IRQHandler()
{
  uint8_t n;
  int32_t tmp32;
  float tmp_f;
    if (DMA_GetITStatus(DMA1_Stream0, DMA_IT_TCIF0))
    {
        DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_TCIF0);
        dma_tc_sr |= DMA_RECEIVED;
        MCP3422_Calc_Fail = 0;          // обнуляем счётчик оошибок шины I2C
        if (!(MCP3422BufferRx[3]&0x80))
        {
//          n = 1-((uint8_t)(MCP3422BufferRx[3]&0x20)>>5);
          n = (MCP3422BufferRx[3]&0x60)>>5;
//          Vbat[n]=MCP3422BufferRx[2] | (MCP3422BufferRx[1]<<8) | (MCP3422BufferRx[0]<<16) | ((MCP3422BufferRx[0]&0x80)<<24);
          tmp32 = MCP3422BufferRx[2] | (MCP3422BufferRx[1]<<8) | (MCP3422BufferRx[0]<<16) | ((MCP3422BufferRx[0]&0x80)<<24);
          if (MCP3422BufferRx[0]&0x02)
          {
//            Vbat[n]|=0xfffe0000;
            tmp32 |= 0xfffe0000;
          }
//          else Vbat[n]&=0x0001ffff;
          else tmp32 &= 0x0001ffff;
//          Vbat[n]=(int32_t)((MCP3422_BIAS+(Vbat[n]>>6))*C_DIV); // (V=REF+N/2^6)
          tmp_f = tmp32*C_DIV*MCP3422_VLSB/1000000-MCP3422_BIAS*40;
//          tmp_f = tmp32*C_DIV;
//          tmp_f = tmp_f*MCP3422_VLSB;
//          tmp_f = tmp_f/1000000000;
//          tmp_f = tmp_f-MCP3422_BIAS*40;
          tmp32 = (int32_t)(tmp_f); // (V=REF+N/2^6)
//          if (Vbat[n]>=0)         // округляем до 0.1 В
//            Vbat[n]+=50;
//          else
//            Vbat[n]-=50;
          if (tmp32>=0)         // округляем до 0.1 В
            tmp32 += 50;
          else
            tmp32 -= 50;
        }
        switch (n)
        {
        case 0:
          MCP3424_data.V_PS1 = tmp32;
          break;
        case 1:
          MCP3424_data.V_PS2 = tmp32;
          break;
        case 2:
          MCP3424_data.VbatA = tmp32;
          break;
        case 3:
          MCP3424_data.VbatB = tmp32;
          break;
        }
        
    }
    if (DMA_GetITStatus(DMA1_Stream0, DMA_IT_HTIF0))
    {
        DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_HTIF0);
    }
    if (DMA_GetITStatus(DMA1_Stream0, DMA_IT_TEIF0))
    {
        DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_TEIF0);
        MCP3422_I2C->CR2 &= (uint16_t)~((uint16_t)I2C_CR2_DMAEN);
        MCP3422_I2C->CR1 |= I2C_CR1_STOP;      // Send I2C1 STOP Condition
        DMA1_Stream0->CR &= (uint32_t)~((uint32_t)DMA_SxCR_EN);   //Disable DMA channel
        MCP3422_Calc_Fail++;
    }
    if (DMA_GetITStatus(DMA1_Stream0, DMA_IT_FEIF0))
    {
        DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_FEIF0);
    }
    if (DMA_GetITStatus(DMA1_Stream0, DMA_IT_DMEIF0))
    {
        DMA_ClearITPendingBit(DMA1_Stream0, DMA_IT_DMEIF0);
    }
}

void DMA1_Stream6_IRQHandler()
{
//  uint8_t channel = uint8_t (((DMA1_Stream4->CR & DMA_SxCR_CHSEL)>>25) &0x07);
  uint32_t channel = uint32_t (DMA1_Stream6->CR & DMA_SxCR_CHSEL);
  if (channel == LED_DMA_Channel)
  {
    if (DMA_GetITStatus(DMA1_Stream6, DMA_IT_TCIF6))
    {
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6);
    }
    if (DMA_GetITStatus(DMA1_Stream6, DMA_IT_HTIF6))
    {
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_HTIF6);
    }
    if (DMA_GetITStatus(DMA1_Stream6, DMA_IT_TEIF6))
    {
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TEIF6);
    }
    if (DMA_GetITStatus(DMA1_Stream6, DMA_IT_FEIF6))
    {
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_FEIF6);
    }
    if (DMA_GetITStatus(DMA1_Stream6, DMA_IT_DMEIF6))
    {
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_DMEIF6);
    }
  }
  if (channel == MCP3422_DMA_Channel)
  {
    if (DMA_GetITStatus(DMA1_Stream6, DMA_IT_TCIF6))
    {
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6);
        dma_tc_sr |= DMA_TRANSMITTED;
        MCP3422_Calc_Fail = 0;          // обнуляем счётчик оошибок шины I2C
    }
    if (DMA_GetITStatus(DMA1_Stream6, DMA_IT_HTIF6))
    {
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_HTIF6);
    }
    if (DMA_GetITStatus(DMA1_Stream6, DMA_IT_TEIF6))
    {
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TEIF6);
        MCP3422_I2C->CR2 &= (uint16_t)~((uint16_t)I2C_CR2_DMAEN);
        MCP3422_I2C->CR1 |= I2C_CR1_STOP;      // Send I2C1 STOP Condition
        DMA1_Stream6->CR &= (uint32_t)~((uint32_t)DMA_SxCR_EN);   //Disable DMA channel
        MCP3422_Calc_Fail++;
    }
    if (DMA_GetITStatus(DMA1_Stream6, DMA_IT_FEIF6))
    {
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_FEIF6);
    }
    if (DMA_GetITStatus(DMA1_Stream6, DMA_IT_DMEIF6))
    {
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_DMEIF6);
    }
  }
  
  if (channel == ADCH_DMA_Channel)
  {
    if (DMA_GetITStatus(DMA1_Stream6, DMA_IT_TCIF6))
    {
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6);
    }
    if (DMA_GetITStatus(DMA1_Stream6, DMA_IT_HTIF6))
    {
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_HTIF6);
    }
    if (DMA_GetITStatus(DMA1_Stream6, DMA_IT_TEIF6))
    {
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TEIF6);
    }
    if (DMA_GetITStatus(DMA1_Stream4, DMA_IT_FEIF6))
    {
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_FEIF6);
    }
    if (DMA_GetITStatus(DMA1_Stream6, DMA_IT_DMEIF6))
    {
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_DMEIF6);
    }
  }
}
/*
void I2C3_EV_IRQHandler()
{
    volatile uint32_t sr1 = I2C3 ->SR1, sr2 = I2C3 ->SR2;
    if( sr1 & I2C_SR1_SB )
    {
        I2C3->DR = i2c_data;
    }
    if(sr1 & I2C_SR1_TXE)
    {
        I2C3->DR = i2c_data;
        I2C3->CR1 |= I2C_CR1_STOP; 
    }
}
*/
void TIM7_IRQHandler(void)
{
  TIM7->SR = ~TIM_SR_UIF; // rc_w0
  
#ifdef DEBUG_PIN_ENABLE
  GPIOH->BSRRH = GPIO_Pin_9;
#endif

  mcp3422_sr = TIM7_TIMEOUT;
  MCP3422_Calc_Fail++;
}

}

