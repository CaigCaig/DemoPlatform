#include "stm32f4xx.h"
#include "ach_adch.h"
#include "FreeRTOS.h"
#include "task.h"
#include "mb_def.h"
#include "com_handler.h"

uint8_t AdchBufferTx[USART3_CMD_LEN], AdchBufferRx[USART3_CMD_LEN];
uint8_t AchaBufferTx[UART4_CMD_LEN], AchaBufferRx[UART4_CMD_LEN];
uint8_t AchbBufferTx[UART5_CMD_LEN], AchbBufferRx[UART5_CMD_LEN];

uint8_t adch_rxwd_count, adch_rx_counter, adch_cs_rx, adch_com_len, adch_f;
uint8_t acha_rxwd_count, acha_rx_counter, acha_cs_rx, acha_com_len, acha_f;
uint8_t achb_rxwd_count, achb_rx_counter, achb_cs_rx, achb_com_len, achb_f;
extern uint32_t com_sr_adch;            // статусный регистр принятия команд для ADCH
extern uint32_t com_sr_acha;            // статусный регистр принятия команд для ACHA
extern uint32_t com_sr_achb;            // статусный регистр принятия команд для ACHB

void ACH_ADCH_Init(void)
{
  USART_InitTypeDef husart;
  GPIO_InitTypeDef hgpio;
  DMA_InitTypeDef  hdma;
  NVIC_InitTypeDef hnvic;
  
    /** USART3 GPIO Configuration    
    PD8     ------> USART3_TX
    PD9     ------> USART3_RX 
    */

  hgpio.GPIO_Pin = adch_tx_Pin | adch_rx_Pin;
  hgpio.GPIO_Speed = GPIO_Speed_2MHz;
  hgpio.GPIO_Mode = GPIO_Mode_AF;
  hgpio.GPIO_OType = GPIO_OType_PP;
  hgpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(adch_tx_GPIO_Port, &hgpio);
  
  GPIO_PinAFConfig(adch_tx_GPIO_Port, GPIO_PinSource8, GPIO_AF_USART3);
  GPIO_PinAFConfig(adch_rx_GPIO_Port, GPIO_PinSource9, GPIO_AF_USART3); 

  USART_DeInit(USART3);
//  USART6->CR1 |= USART_CR1_OVER8 | USART_CR1_TCIE | USART_CR1_RXNEIE;
  USART3->CR1 |= USART_CR1_TCIE | USART_CR1_RXNEIE;
  husart.USART_BaudRate = USART3_SPEED;
  husart.USART_WordLength = USART_WordLength_8b;
  husart.USART_StopBits = USART_StopBits_1;
  husart.USART_Parity = USART_Parity_No;
  husart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  husart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART3, &husart);
//  USART6->CR3 |= USART_CR3_DMAT;
  
  USART_Cmd(USART3, ENABLE);      
    
// Настройка прерываний для USART3_TX
  hnvic.NVIC_IRQChannel = USART3_IRQn;
  hnvic.NVIC_IRQChannelPreemptionPriority = 0;
  hnvic.NVIC_IRQChannelSubPriority = 0;
  hnvic.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&hnvic);

// Настройка DMA для USART3_TX
  DMA_DeInit(DMA1_Stream3);
  hdma.DMA_Channel = DMA_Channel_4;
  hdma.DMA_PeripheralBaseAddr = (uint32_t)(&(USART3->DR));
  hdma.DMA_Memory0BaseAddr = (uint32_t)&AdchBufferTx[0];
  hdma.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  hdma.DMA_BufferSize = USART3_CMD_LEN;
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
  DMA_Init(DMA1_Stream3, &hdma);
  
  hnvic.NVIC_IRQChannel = DMA1_Stream3_IRQn;
  hnvic.NVIC_IRQChannelPreemptionPriority = 0;
  hnvic.NVIC_IRQChannelSubPriority = 0;
  hnvic.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&hnvic);
    
  DMA_ITConfig(DMA1_Stream3, DMA_IT_TC | DMA_IT_TE, ENABLE);

  USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);

    /** USART4 GPIO Configuration    
    PC10     ------> UART4_TX
    PC11     ------> UART4_RX 
    */

  hgpio.GPIO_Pin = acha_tx_Pin | acha_rx_Pin;
  hgpio.GPIO_Speed = GPIO_Speed_2MHz;
  hgpio.GPIO_Mode = GPIO_Mode_AF;
  hgpio.GPIO_OType = GPIO_OType_PP;
  hgpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(acha_tx_GPIO_Port, &hgpio);
  
  GPIO_PinAFConfig(acha_tx_GPIO_Port, GPIO_PinSource10, GPIO_AF_UART4);
  GPIO_PinAFConfig(acha_rx_GPIO_Port, GPIO_PinSource11, GPIO_AF_UART4); 

  USART_DeInit(UART4);
  UART4->CR1 |= USART_CR1_TCIE | USART_CR1_RXNEIE;
  husart.USART_BaudRate = UART4_SPEED;
  husart.USART_WordLength = USART_WordLength_8b;
  husart.USART_StopBits = USART_StopBits_1;
  husart.USART_Parity = USART_Parity_No;
  husart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  husart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(UART4, &husart);
  
  USART_Cmd(UART4, ENABLE);      
    
// Настройка прерываний для UART4_TX
  hnvic.NVIC_IRQChannel = UART4_IRQn;
  hnvic.NVIC_IRQChannelPreemptionPriority = 0;
  hnvic.NVIC_IRQChannelSubPriority = 0;
  hnvic.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&hnvic);

// Настройка DMA для UART4_TX
  DMA_DeInit(DMA1_Stream4);
  hdma.DMA_Channel = DMA_Channel_4;
  hdma.DMA_PeripheralBaseAddr = (uint32_t)(&(UART4->DR));
  hdma.DMA_Memory0BaseAddr = (uint32_t)&AchaBufferTx[0];
  hdma.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  hdma.DMA_BufferSize = UART4_CMD_LEN;
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

  USART_DMACmd(UART4, USART_DMAReq_Tx, ENABLE);

    /** UART5 GPIO Configuration    
    PC12     ------> UART5_TX
    PD2     ------> UART5_RX 
    */

  hgpio.GPIO_Pin = achb_tx_Pin;
  hgpio.GPIO_Speed = GPIO_Speed_2MHz;
  hgpio.GPIO_Mode = GPIO_Mode_AF;
  hgpio.GPIO_OType = GPIO_OType_PP;
  hgpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(achb_tx_GPIO_Port, &hgpio);
  
  hgpio.GPIO_Pin = achb_rx_Pin;
  GPIO_Init(achb_rx_GPIO_Port, &hgpio);

  GPIO_PinAFConfig(achb_tx_GPIO_Port, GPIO_PinSource12, GPIO_AF_UART5);
  GPIO_PinAFConfig(achb_rx_GPIO_Port, GPIO_PinSource2, GPIO_AF_UART5); 

  USART_DeInit(UART5);
  UART5->CR1 |= USART_CR1_TCIE | USART_CR1_RXNEIE;
  husart.USART_BaudRate = UART5_SPEED;
  husart.USART_WordLength = USART_WordLength_8b;
  husart.USART_StopBits = USART_StopBits_1;
  husart.USART_Parity = USART_Parity_No;
  husart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  husart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(UART5, &husart);
  
  USART_Cmd(UART5, ENABLE);      
    
// Настройка прерываний для UART5_TX
  hnvic.NVIC_IRQChannel = UART5_IRQn;
  hnvic.NVIC_IRQChannelPreemptionPriority = 0;
  hnvic.NVIC_IRQChannelSubPriority = 0;
  hnvic.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&hnvic);

// Настройка DMA для UART5_TX
  DMA_DeInit(DMA1_Stream7);
  hdma.DMA_Channel = DMA_Channel_4;
  hdma.DMA_PeripheralBaseAddr = (uint32_t)(&(UART4->DR));
  hdma.DMA_Memory0BaseAddr = (uint32_t)&AchaBufferTx[0];
  hdma.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  hdma.DMA_BufferSize = UART5_CMD_LEN;
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
  DMA_Init(DMA1_Stream7, &hdma);
  
  hnvic.NVIC_IRQChannel = DMA1_Stream7_IRQn;
  hnvic.NVIC_IRQChannelPreemptionPriority = 0;
  hnvic.NVIC_IRQChannelSubPriority = 0;
  hnvic.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&hnvic);
    
  DMA_ITConfig(DMA1_Stream7, DMA_IT_TC | DMA_IT_TE, ENABLE);

  USART_DMACmd(UART4, USART_DMAReq_Tx, ENABLE);

}

extern "C" {

  void USART3_IRQHandler()
  {
    uint8_t i;
    __IO uint8_t tmp;
    if (USART3->SR & USART_SR_RXNE)
    {
        AdchBufferRx[rs485_rx_counter] = USART6->DR;
        adch_rxwd_count=0;
        USART3->SR = ~USART_SR_RXNE;      // rc_w0
        adch_cs_rx ^= AdchBufferRx[adch_rx_counter];
        adch_rx_counter++;
        if ((AdchBufferRx[0]==ADCH_ID) && (adch_rx_counter>=3) && (adch_cs_rx==0))
        {
          switch (AdchBufferRx[1])
          {
          case CAM_GET_CUR_WIN:
            if (adch_rx_counter==L_CAM_GET_CUR_WIN_CONF)
              {
                com_sr_cam |= COM_CAM_GET_CUR_WIN;
              }
            break;
          }
          adch_rx_counter=0;
          adch_cs_rx=0;
        }
        
//        case ADCH_ID:
//          if ((adch_rx_counter>=3) && (adch_cs_rx==0))
//          {
//            if ((USART3BufferRx[1]==hello) && (adch_rx_counter==l_hello))
//              {
//                com_sr_adch |= com_hello;
//              }
//            adch_rx_counter=0;
//            adch_cs_rx=0;
//          }
//          break;
//        }

        if (adch_rx_counter>=USART3_CMD_LEN)
        {
          USART3BufferRx[0]=0;
          adch_rx_counter=0;
          adch_cs_rx=0;
        }

    }

    if (USART3->SR & USART_SR_TC)        // rc_w0
    {
      USART3->SR = ~USART_SR_TC;        // rc_w0
      adch_f &= ~USART_TX_BUSY;
    }
  }
  
  void DMA1_Stream3_IRQHandler()        // USART6_TX
  {
    if (DMA_GetITStatus(DMA1_Stream3, DMA_IT_TCIF3))
    {
        DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_TCIF3);
//        USART3->CR1 |= USART_CR1_TCIE;
    }
    if (DMA_GetITStatus(DMA1_Stream3, DMA_IT_HTIF3))
    {
        DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_HTIF3);
    }
    if (DMA_GetITStatus(DMA1_Stream3, DMA_IT_TEIF3))
    {
        DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_TEIF3);
    }
    if (DMA_GetITStatus(DMA1_Stream3, DMA_IT_FEIF3))
    {
        DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_FEIF3);
    }
    if (DMA_GetITStatus(DMA1_Stream3, DMA_IT_DMEIF3))
    {
        DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_DMEIF3);
    }
  }

}