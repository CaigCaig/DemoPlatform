#include "stm32f4xx.h"
#include "ach_adch.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "mb_def.h"
#include "com_handler.h"

uint8_t AchaBufferTxCmd[ACHA_CMD_LEN], AchaBufferRx[ACHA_CMD_LEN];
uint8_t AchbBufferTxCmd[ACHB_CMD_LEN], AchbBufferRx[ACHB_CMD_LEN];
uint8_t AdchBufferTxCmd[ADCH_CMD_LEN], AdchBufferRx[ADCH_CMD_LEN];
uint8_t acha_type_answ[NUM_COM_ACH]={0}, achb_type_answ[NUM_COM_ACH]={0} , adch_type_answ[NUM_COM_ADCH]={0};
uint8_t acha_rxwd_count, acha_rx_counter, acha_cs_rx, acha_com_len, acha_f;
uint8_t achb_rxwd_count, achb_rx_counter, achb_cs_rx, achb_com_len, achb_f;
uint8_t adch_rxwd_count, adch_rx_counter, adch_cs_rx, adch_com_len, adch_f;

extern uint32_t com_sr_acha;            // статусный регистр принятия команд для Analog Charger A
extern uint32_t com_sr_achb;            // статусный регистр принятия команд для Analog Charger B
extern uint32_t com_sr_adch;            // статусный регистр принятия команд для Analog Discharger

void ACH_ADCH_Init(void)
{
  USART_InitTypeDef husart;
  GPIO_InitTypeDef hgpio;
  DMA_InitTypeDef  hdma;
  NVIC_InitTypeDef hnvic;
  
    /** Fix Charger/Discharger GPIO Configuration
    PI4, PI5     ------> select channel
    PI6     ------> select I(0)/O(1)
    PI7     ------> 
    */

  hgpio.GPIO_Pin = acha_tx_Pin | acha_rx_Pin;
  hgpio.GPIO_Speed = GPIO_Speed_2MHz;
  hgpio.GPIO_Mode = GPIO_Mode_AF;
  hgpio.GPIO_OType = GPIO_OType_PP;
  hgpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(acha_tx_GPIO_Port, &hgpio);

  /** USART1 GPIO Configuration (ACHA)
    PB6     ------> USART1_TX
    PB7     ------> USART1_RX 
    */

  hgpio.GPIO_Pin = acha_tx_Pin | acha_rx_Pin;
  hgpio.GPIO_Speed = GPIO_Speed_2MHz;
  hgpio.GPIO_Mode = GPIO_Mode_AF;
  hgpio.GPIO_OType = GPIO_OType_PP;
  hgpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(acha_tx_GPIO_Port, &hgpio);
  
  GPIO_PinAFConfig(acha_tx_GPIO_Port, GPIO_PinSource6, GPIO_AF_USART1);
  GPIO_PinAFConfig(acha_rx_GPIO_Port, GPIO_PinSource7, GPIO_AF_USART1); 

  USART_DeInit(UART_ACHA);
//  UART_ACHA->CR1 |= USART_CR1_OVER8 | USART_CR1_TCIE | USART_CR1_RXNEIE;
  USART1->CR1 |= USART_CR1_TCIE | USART_CR1_RXNEIE;
  husart.USART_BaudRate = UART_ACHA_SPEED;
  husart.USART_WordLength = USART_WordLength_8b;
  husart.USART_StopBits = USART_StopBits_1;
  husart.USART_Parity = USART_Parity_No;
  husart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  husart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(UART_ACHA, &husart);
//  UART_ACHA->CR3 |= USART_CR3_DMAT;
  
  USART_Cmd(UART_ACHA, ENABLE);      
    
// Настройка прерываний для USART1_TX (ACHA)
  hnvic.NVIC_IRQChannel = ACHA_IRQn;
  hnvic.NVIC_IRQChannelPreemptionPriority = 0;
  hnvic.NVIC_IRQChannelSubPriority = 0;
  hnvic.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&hnvic);

// Настройка DMA для USART1_TX (ACHA)
  DMA_DeInit(DMA2_Stream7);
  hdma.DMA_Channel = DMA_Channel_4;
  hdma.DMA_PeripheralBaseAddr = (uint32_t)(&(UART_ACHA->DR));
  hdma.DMA_Memory0BaseAddr = (uint32_t)&AchaBufferTxCmd[0];
  hdma.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  hdma.DMA_BufferSize = ACHA_CMD_LEN;
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
  DMA_Init(DMA2_Stream7, &hdma);
  
  hnvic.NVIC_IRQChannel = DMA2_Stream7_IRQn;
  hnvic.NVIC_IRQChannelPreemptionPriority = 0;
  hnvic.NVIC_IRQChannelSubPriority = 0;
  hnvic.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&hnvic);
    
  DMA_ITConfig(DMA2_Stream7, DMA_IT_TC | DMA_IT_TE, ENABLE);

  USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);

    /** USART4 GPIO Configuration (ACHB)
    PC10     ------> UART4_TX
    PC11     ------> UART4_RX 
    */

  hgpio.GPIO_Pin = acha_tx_Pin | acha_rx_Pin;
  hgpio.GPIO_Speed = GPIO_Speed_2MHz;
  hgpio.GPIO_Mode = GPIO_Mode_AF;
  hgpio.GPIO_OType = GPIO_OType_PP;
  hgpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(achb_tx_GPIO_Port, &hgpio);
  
  GPIO_PinAFConfig(achb_tx_GPIO_Port, GPIO_PinSource10, GPIO_AF_UART4);
  GPIO_PinAFConfig(achb_rx_GPIO_Port, GPIO_PinSource11, GPIO_AF_UART4); 

  USART_DeInit(UART_ACHB);
  UART_ACHB->CR1 |= USART_CR1_TCIE | USART_CR1_RXNEIE;
  husart.USART_BaudRate = UART_ACHB_SPEED;
  husart.USART_WordLength = USART_WordLength_8b;
  husart.USART_StopBits = USART_StopBits_1;
  husart.USART_Parity = USART_Parity_No;
  husart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  husart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(UART_ACHB, &husart);
  
  USART_Cmd(UART4, ENABLE);      
    
// Настройка прерываний для UART4_TX (ACHB)
  hnvic.NVIC_IRQChannel = ACHB_IRQn;
  hnvic.NVIC_IRQChannelPreemptionPriority = 0;
  hnvic.NVIC_IRQChannelSubPriority = 0;
  hnvic.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&hnvic);

// Настройка DMA для UART4_TX (ACHB)
  DMA_DeInit(DMA1_Stream4);
  hdma.DMA_Channel = DMA_Channel_4;
  hdma.DMA_PeripheralBaseAddr = (uint32_t)(&(UART_ACHB->DR));
  hdma.DMA_Memory0BaseAddr = (uint32_t)&AchbBufferTxCmd[0];
  hdma.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  hdma.DMA_BufferSize = ACHB_CMD_LEN;
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
  DMA_Init(DMA1_Stream4, &hdma);
  
  hnvic.NVIC_IRQChannel = DMA1_Stream4_IRQn;
  hnvic.NVIC_IRQChannelPreemptionPriority = 0;
  hnvic.NVIC_IRQChannelSubPriority = 0;
  hnvic.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&hnvic);
    
  DMA_ITConfig(DMA1_Stream4, DMA_IT_TC | DMA_IT_TE, ENABLE);

  USART_DMACmd(UART_ACHB, USART_DMAReq_Tx, ENABLE);

    /** USART2 GPIO Configuration (ADCH)
    PD5     ------> USART2_TX
    PD6     ------> USART2_RX 
    */

  hgpio.GPIO_Pin = adch_tx_Pin | adch_rx_Pin;
  hgpio.GPIO_Speed = GPIO_Speed_2MHz;
  hgpio.GPIO_Mode = GPIO_Mode_AF;
  hgpio.GPIO_OType = GPIO_OType_PP;
  hgpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(adch_tx_GPIO_Port, &hgpio);
  
  GPIO_PinAFConfig(adch_tx_GPIO_Port, GPIO_PinSource5, GPIO_AF_USART2);
  GPIO_PinAFConfig(adch_rx_GPIO_Port, GPIO_PinSource6, GPIO_AF_USART2); 

  USART_DeInit(UART_ADCH);
  USART2->CR1 |= USART_CR1_TCIE | USART_CR1_RXNEIE;
  husart.USART_BaudRate = UART_ADCH_SPEED;
  husart.USART_WordLength = USART_WordLength_8b;
  husart.USART_StopBits = USART_StopBits_1;
  husart.USART_Parity = USART_Parity_No;
  husart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  husart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(UART_ADCH, &husart);
  
  USART_Cmd(UART_ADCH, ENABLE);      
    
// Настройка прерываний для USART2_TX (ADCH)
  hnvic.NVIC_IRQChannel = ADCH_IRQn;
  hnvic.NVIC_IRQChannelPreemptionPriority = 0;
  hnvic.NVIC_IRQChannelSubPriority = 0;
  hnvic.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&hnvic);

// Настройка DMA для USART2_TX (ADCH)
  DMA_DeInit(DMA1_Stream6);
  hdma.DMA_Channel = DMA_Channel_4;
  hdma.DMA_PeripheralBaseAddr = (uint32_t)(&(UART_ADCH->DR));
  hdma.DMA_Memory0BaseAddr = (uint32_t)&AdchBufferTxCmd[0];
  hdma.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  hdma.DMA_BufferSize = ADCH_CMD_LEN;
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

  USART_DMACmd(UART_ADCH, USART_DMAReq_Tx, ENABLE);

}

uint8_t ACH_Set(uint8_t current)
{
  uint8_t range;
  uint32_t DAC_value32;
  if (current>55)
  {
    range = 4;
    DAC_value32 = 0x0000DBFF;   // если разностный ток больше 5,5А, то задаём диапазон 4 и ограничиваем максимальный ток аналогового зарядника 5,5А
  }
  else
  {
    if (current>32)            // определяем диапазон аналогового зарядника и значение для ЦАП, соответствующее необходимому току
    {
      range = 4;                // если разностный ток больше 3,2А, то задаём диапазон 4
//      DAC_value32 = (residual<<16)/64;
      DAC_value32 = current<<10;
    }
    else
    {
      if (current>16)
      {
        range = 3;              // если разностный ток больше 1,6А, то задаём диапазон 3
//        DAC_value32 = (residual<<16)/32;
        DAC_value32 = current<<11;
      }
      else
      {
        if (current>8)
        {
          range = 2;            // если разностный ток больше 0,8А, то задаём диапазон 2
//          DAC_value32 = (residual<<16)/16;
          DAC_value32 = current<<12;
        }
        else
        {
          if (current)
          {
            range = 1;          // если разностный ток больше 0А, но меньше 0,8А то задаём диапазон 1
//            DAC_value32 = (residual<<16)/8;
            DAC_value32 = current<<13;
          }
          else
          {
            range = 0;          // если разностный ток равен 0А, то выключаем аналоговый зарядник
            DAC_value32 = 0;
          }
        }
      }
    }
  }
  AchaBufferTxCmd[0] = ACHA_ID;
  AchaBufferTxCmd[1] = ACH_SET_CUR;
  AchaBufferTxCmd[2] = (uint8_t)DAC_value32;
  AchaBufferTxCmd[3] = (uint8_t)(DAC_value32>>8);
  AchaBufferTxCmd[4] = range;
  AchaBufferTxCmd[5] = ACHA_ID ^ ACH_SET_CUR ^ AchaBufferTxCmd[2] ^ AchaBufferTxCmd[3] ^ AchaBufferTxCmd[4];
  com_sr_acha = 0;
//  acha_conf_answ_f = 0;
  DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TCIF7 | DMA_IT_HTIF7 | DMA_IT_TEIF7 | DMA_IT_DMEIF7 | DMA_IT_FEIF7);
  DMA2_Stream7->M0AR =  (uint32_t)&AchaBufferTxCmd[0];
  DMA2_Stream7->NDTR = (uint32_t) L_ACH_SET_CUR;
  DMA2_Stream7->CR |= (uint32_t) DMA_SxCR_EN;         // передаём команду в Analog Charger A
  uint16_t timeout=0;
  do
  {
    vTaskDelay(1);
    if (com_sr_acha & COM_ACH_SET_CUR)
    {
      com_sr_acha &=~COM_ACH_SET_CUR;
      return OK;
    }
    timeout++;
  } while (timeout<TIMEOUT_ACH);
  com_sr_acha = 0;
  return FAIL;
}

extern "C" {
  
  void USART1_IRQHandler() //UART_ACHA_IRQHandler()
  {
    if (UART_ACHA->SR & USART_SR_RXNE)
    {
        AchaBufferRx[acha_rx_counter] = UART_ACHA->DR;
        acha_rxwd_count=0;
        UART_ACHA->SR = ~USART_SR_RXNE;      // rc_w0
        acha_cs_rx ^= AchaBufferRx[acha_rx_counter];
        acha_rx_counter++;
        switch (AchaBufferRx[0])
        {
        case ACHA_ID:
          if ((acha_rx_counter>=3) && (acha_cs_rx==0))
          {
            switch (AchaBufferRx[1])
            {
            case ACH_SET_CUR:
              if (acha_rx_counter==L_ACH_SET_CUR_ANSW)
              {
                com_sr_acha |= COM_ACH_SET_CUR;
              }
              break;
/*
            case ACH_WATCH_WIN:
              if ((acha_rx_counter==L_CAM_WATCH_WIN_CONF)||(rs485_rx_counter==L_CAM_WATCH_WIN_ANSW))
                {
                  com_sr_cam |= COM_CAM_WATCH_WIN;
                  acha_com_len = acha_rx_counter;
                }
              break;
*/
            }
            acha_rx_counter=0;
            acha_cs_rx=0;
          }
          break;
        }
        
        if (acha_rx_counter>=ACHA_CMD_LEN)
        {
          AchaBufferRx[0]=0;
          acha_rx_counter=0;
          acha_cs_rx=0;
        }

    }

    if (UART_ACHA->SR & USART_SR_TC)        // rc_w0
    {
      UART_ACHA->SR = ~USART_SR_TC;        // rc_w0
      acha_f &= ~UART_TX_BUSY;
//      UART_ACHA->CR1 &= ~USART_CR1_TCIE;
    }
//    if (USART_GetITStatus(UART_ACHA, USART_IT_ORE_RX))
//    {
//      tmp = UART_ACHA->DR;
//    }
  }
  
  void UART4_IRQHandler() //UART_ACHB_IRQHandler()
  {
    if (UART_ACHB->SR & USART_SR_RXNE)
    {
        AchbBufferRx[acha_rx_counter] = UART_ACHB->DR;
        achb_rxwd_count=0;
        UART_ACHB->SR = ~USART_SR_RXNE;      // rc_w0
        achb_cs_rx ^= AchbBufferRx[achb_rx_counter];
        achb_rx_counter++;
        switch (AchbBufferRx[0])
        {
        case ACHB_ID:
          if ((achb_rx_counter>=3) && (achb_cs_rx==0))
          {
            switch (AchbBufferRx[1])
            {
            case ACH_SET_CUR:
              if (achb_rx_counter==L_ACH_SET_CUR)
              {
                com_sr_achb |= COM_ACH_SET_CUR;
              }
              break;
            }
            achb_rx_counter=0;
            achb_cs_rx=0;
          }
          break;
        }
        
        if (achb_rx_counter>=ACHB_CMD_LEN)
        {
          AchbBufferRx[0]=0;
          achb_rx_counter=0;
          achb_cs_rx=0;
        }

    }

    if (UART_ACHB->SR & USART_SR_TC)        // rc_w0
    {
      UART_ACHB->SR = ~USART_SR_TC;        // rc_w0
      achb_f &= ~UART_TX_BUSY;
    }
  }
  
  void USART2_IRQHandler() //UART_ADCH_IRQHandler()
  {
    if (UART_ADCH->SR & USART_SR_RXNE)
    {
        AdchBufferRx[acha_rx_counter] = UART_ADCH->DR;
        adch_rxwd_count=0;
        UART_ADCH->SR = ~USART_SR_RXNE;      // rc_w0
        adch_cs_rx ^= AdchBufferRx[adch_rx_counter];
        adch_rx_counter++;
        switch (AdchBufferRx[0])
        {
        case ADCH_ID:
          if ((adch_rx_counter>=3) && (adch_cs_rx==0))
          {
            switch (AdchBufferRx[1])
            {
            case ADCH_PING:
              if (adch_rx_counter==L_ADCH_PING)
              {
                com_sr_adch |= COM_ADCH_PING;
              }
              break;
            case ADCH_SET_CUR:
              if (adch_rx_counter==L_ADCH_SET_CUR)
              {
                com_sr_adch |= COM_ADCH_SET_CUR;
              }
              break;
            }
            adch_rx_counter=0;
            adch_cs_rx=0;
          }
          break;
        }
        
        if (adch_rx_counter>=ADCH_CMD_LEN)
        {
          AdchBufferRx[0]=0;
          adch_rx_counter=0;
          adch_cs_rx=0;
        }

    }

    if (UART_ADCH->SR & USART_SR_TC)        // rc_w0
    {
      UART_ADCH->SR = ~USART_SR_TC;        // rc_w0
      adch_f &= ~UART_TX_BUSY;
    }
  }
  
  void DMA2_Stream7_IRQHandler()        // ACHA_TX
  {
    if (DMA_GetITStatus(DMA2_Stream7, DMA_IT_TCIF7))
    {
        DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TCIF7);
//        USART6->CR1 |= USART_CR1_TCIE;
    }
    if (DMA_GetITStatus(DMA2_Stream7, DMA_IT_HTIF7))
    {
        DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_HTIF7);
    }
    if (DMA_GetITStatus(DMA2_Stream7, DMA_IT_TEIF7))
    {
        DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TEIF7);
    }
    if (DMA_GetITStatus(DMA2_Stream7, DMA_IT_FEIF7))
    {
        DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_FEIF7);
    }
    if (DMA_GetITStatus(DMA2_Stream7, DMA_IT_DMEIF7))
    {
        DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_DMEIF7);
    }
  }

  void DMA1_Stream4_IRQHandler()        // ACHB_TX
  {
    if (DMA_GetITStatus(DMA1_Stream4, DMA_IT_TCIF4))
    {
        DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF4);
//        USART6->CR1 |= USART_CR1_TCIE;
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

/*
  void DMA1_Stream6_IRQHandler()        // ADCH_TX
  {
    if (DMA_GetITStatus(DMA1_Stream6, DMA_IT_TCIF6))
    {
        DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6);
//        USART6->CR1 |= USART_CR1_TCIE;
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
*/
}