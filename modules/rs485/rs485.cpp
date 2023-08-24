#include "stm32f4xx.h"
#include "rs485.h"
#include "FreeRTOS.h"
#include "task.h"
#include "mb_def.h"
#include "hood.h"
#include "com_handler.h"

uint8_t USART6BufferTxCmd[USART6_CMD_LEN], USART6BufferRx[USART6_CMD_LEN];
uint8_t USART6ConfBuf[USART6_CONF_LEN], USART6AnswBuf[USART6_ANSW_LEN];
//uint8_t USART6BufferData[2][USART6_DATA_LEN];
uint8_t cam_type_answ[NUM_COM_CAM]={0};
uint8_t rs485_rxwd_count, rs485_rx_counter, rs485_cs_rx, rs485_com_len, rs485_f;
extern uint32_t com_sr_mh;              // статусный регистр принятия команд для master_hood
extern uint32_t com_sr_h1;              // статусный регистр принятия команд для hood1
extern uint32_t com_sr_h2;              // статусный регистр принятия команд для hood2
extern uint32_t com_sr_cam;             // статусный регистр принятия команд для видеокамер

char USART6FrameBuf[2][FRAME_PACKET_MAXSIZE];
uint8_t cur_buf;
uint16_t Frame_Packets, tim_frame=0;
uint32_t Frame_Packet_Size;
uint32_t USART6_BRR_Copy;

/* предварительно необходимо включить периферию: GPIOC, USART6, DMA2 */
void RS485_Init(void)
{
  USART_InitTypeDef husart;
  GPIO_InitTypeDef hgpio;
  DMA_InitTypeDef  hdma;
  NVIC_InitTypeDef hnvic;
  
    /** USART6_TX GPIO Configuration    
    PC6     ------> USART6_TX
    PC7     ------> USART6_RX 
    */

  hgpio.GPIO_Pin = RxTx_Switch_Pin;
  hgpio.GPIO_Speed = GPIO_Speed_2MHz;
  hgpio.GPIO_Mode = GPIO_Mode_OUT;
  hgpio.GPIO_OType = GPIO_OType_PP;
  hgpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(RxTx_Switch_GPIO_Port, &hgpio);
  GPIO_WriteBit(RxTx_Switch_GPIO_Port, RxTx_Switch_Pin, Bit_RESET);

  hgpio.GPIO_Pin = usart6_tx_Pin | usart6_rx_Pin;
  hgpio.GPIO_Speed = GPIO_Speed_100MHz;
  hgpio.GPIO_Mode = GPIO_Mode_AF;
  hgpio.GPIO_OType = GPIO_OType_PP;
  hgpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(usart6_tx_GPIO_Port, &hgpio);
  
  GPIO_PinAFConfig(usart6_tx_GPIO_Port, GPIO_PinSource6, GPIO_AF_USART6);
  GPIO_PinAFConfig(usart6_rx_GPIO_Port, GPIO_PinSource7, GPIO_AF_USART6); 

  USART_DeInit(USART6);
//  USART6->CR1 |= USART_CR1_OVER8 | USART_CR1_TCIE | USART_CR1_RXNEIE;
  USART6->CR1 |= USART_CR1_TCIE | USART_CR1_RXNEIE;
  husart.USART_BaudRate = USART6_SPEED;
  husart.USART_WordLength = USART_WordLength_8b;
  husart.USART_StopBits = USART_StopBits_1;
  husart.USART_Parity = USART_Parity_No;
  husart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  husart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART6, &husart);
//  USART6->CR3 |= USART_CR3_DMAT;
  USART6_BRR_Copy = USART6->BRR;
  
  USART_Cmd(USART6, ENABLE);      
    
// Настройка прерываний для USART6_TX
  hnvic.NVIC_IRQChannel = USART6_IRQn;
  hnvic.NVIC_IRQChannelPreemptionPriority = 0;
  hnvic.NVIC_IRQChannelSubPriority = 0;
  hnvic.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&hnvic);

// Настройка DMA для USART6_TX
  DMA_DeInit(DMA2_Stream6);
  hdma.DMA_Channel = DMA_Channel_5;
  hdma.DMA_PeripheralBaseAddr = (uint32_t)(&(USART6->DR));
  hdma.DMA_Memory0BaseAddr = (uint32_t)&USART6BufferTxCmd[0];
  hdma.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  hdma.DMA_BufferSize = USART6_CMD_LEN;
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
  DMA_Init(DMA2_Stream6, &hdma);
  
  hnvic.NVIC_IRQChannel = DMA2_Stream6_IRQn;
  hnvic.NVIC_IRQChannelPreemptionPriority = 0;
  hnvic.NVIC_IRQChannelSubPriority = 0;
  hnvic.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&hnvic);
    
  DMA_ITConfig(DMA2_Stream6, DMA_IT_TC | DMA_IT_TE, ENABLE);

  USART_DMACmd(USART6, USART_DMAReq_Tx, ENABLE);
//  DMA_Cmd(DMA2_Stream6, DISABLE);

//  DMA_Cmd(DMA2_Stream6, ENABLE);

// Настройка DMA для USART6_RX
  DMA_DeInit(DMA2_Stream1);
  hdma.DMA_Channel = DMA_Channel_5;
  hdma.DMA_PeripheralBaseAddr = (uint32_t)(&(USART6->DR));
  hdma.DMA_Memory0BaseAddr = (uint32_t)&USART6BufferRx[0];
  hdma.DMA_DIR = DMA_DIR_PeripheralToMemory;
  hdma.DMA_BufferSize = USART6_CMD_LEN;
  hdma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  hdma.DMA_MemoryInc = DMA_MemoryInc_Enable;
  hdma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  hdma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  hdma.DMA_Mode = DMA_Mode_Circular;
  hdma.DMA_Priority = DMA_Priority_High;
  hdma.DMA_FIFOMode = DMA_FIFOMode_Disable;
  hdma.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  hdma.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  hdma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream1, &hdma);
  
  hnvic.NVIC_IRQChannel = DMA2_Stream1_IRQn;
  hnvic.NVIC_IRQChannelPreemptionPriority = 0;
  hnvic.NVIC_IRQChannelSubPriority = 0;
  hnvic.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&hnvic);
    
  DMA_ITConfig(DMA2_Stream1, DMA_IT_TC | DMA_IT_TE, ENABLE);

//  USART_DMACmd(USART6, USART_DMAReq_Rx, ENABLE);
//  DMA_Cmd(DMA2_Stream1, ENABLE);

}

void RS485_Send(uint8_t *addr, uint32_t len)
{
  GPIO_WriteBit(RxTx_Switch_GPIO_Port, RxTx_Switch_Pin, Bit_SET);     // настраиваем RS485 на передачу
  DMA_ClearITPendingBit(DMA2_Stream6, DMA_IT_TCIF4 | DMA_IT_HTIF4 | DMA_IT_TEIF4 | DMA_IT_DMEIF4 | DMA_IT_FEIF4);
  DMA2_Stream6->M0AR =  (uint32_t) addr;
  DMA2_Stream6->NDTR = (uint32_t) len;
  DMA2_Stream6->CR |= (uint32_t) DMA_SxCR_EN;         // передаём команду в master_hood и далее в hood
}

void vUART_WatchDog(void *params)
{
  for (;;)
  {
    if (rs485_rx_counter!=0)
    {
      rs485_rxwd_count++;
      if (rs485_rxwd_count>=15)
      {
        rs485_rx_counter=0;
        rs485_cs_rx=0;
      }
    }
    vTaskDelay(1);
  }
}

extern "C" {
  
  void USART6_IRQHandler()
  {
    uint8_t i;
    __IO uint8_t tmp;
    if (USART6->SR & USART_SR_RXNE)
    {
        USART6BufferRx[rs485_rx_counter] = USART6->DR;
        rs485_rxwd_count=0;
        USART6->SR = ~USART_SR_RXNE;      // rc_w0
        rs485_cs_rx ^= USART6BufferRx[rs485_rx_counter];
        rs485_rx_counter++;
        switch (USART6BufferRx[0])
        {
        case CAM_ID:
          if ((rs485_rx_counter>=3) && (rs485_cs_rx==0))
          {
            switch (USART6BufferRx[1])
            {
            case CAM_ACT:
              if (rs485_rx_counter==L_CAM_ACT)
              {
                com_sr_cam |= COM_CAM_ACT;
              }
              break;
            case CAM_WATCH_WIN:
              if ((rs485_rx_counter==L_CAM_WATCH_WIN_CONF)||(rs485_rx_counter==L_CAM_WATCH_WIN_ANSW))
                {
                  com_sr_cam |= COM_CAM_WATCH_WIN;
                  rs485_com_len = rs485_rx_counter;
                }
              break;
            case CAM_RD_REG:
              if ((rs485_rx_counter==L_CAM_RD_REG_CONF)||(rs485_rx_counter==L_CAM_RD_REG_ANSW))
              {
                switch (cam_type_answ[CAM_RD_REG-1])
                {
                case 0:
                      for (i=0; i<L_CAM_RD_REG_CONF; i++)
                      {
                        USART6ConfBuf[i] = USART6BufferRx[i];
                      }
  //                    cam_type_answ_to_eth[CAM_RD_REG-1]=CAM_TYPE_CONF;
                      com_sr_cam |= COM_CAM_RD_REG;
                      cam_type_answ[CAM_RD_REG-1]|=CAM_TYPE_CONF;
                      break;
                case CAM_TYPE_CONF:
                      for (i=0; i<L_CAM_RD_REG_ANSW; i++)
                      {
                        USART6AnswBuf[i] = USART6BufferRx[i];
                      }
                      cam_type_answ[CAM_RD_REG-1]|=CAM_TYPE_ANSW;
                      break;
                }
                rs485_com_len = rs485_rx_counter;
              }
              break;
            case CAM_WR_REG:
              if ((rs485_rx_counter==L_CAM_WR_REG_CONF)||(rs485_rx_counter==L_CAM_WR_REG_ANSW))
              {
                com_sr_cam |= COM_CAM_WR_REG;
                rs485_com_len = rs485_rx_counter;
              }
              break;
            case CAM_GET_VOLUME:
              if ((rs485_rx_counter==L_CAM_GET_VOLUME_CONF)||(rs485_rx_counter==L_CAM_GET_VOLUME_ANSW))
              {
                com_sr_cam |= COM_CAM_GET_VOLUME;
                rs485_com_len = rs485_rx_counter;
              }
              break;
            case CAM_GET_DENSITY:
              if ((rs485_rx_counter==L_CAM_GET_DENSITY_CONF)||(rs485_rx_counter==L_CAM_GET_DENSITY_ANSW))
              {
                com_sr_cam |= COM_CAM_GET_DENSITY;
                rs485_com_len = rs485_rx_counter;
              }
              break;
            case CAM_FRAME_TO_MEM:
              if ((rs485_rx_counter==L_CAM_FRAME_TO_MEM_CONF)||(rs485_rx_counter==L_CAM_FRAME_TO_MEM_ANSW))
              {
                com_sr_cam |= COM_CAM_FRAME_TO_MEM;
                rs485_com_len = rs485_rx_counter;
              }
              break;
            case CAM_GET_CUR_WIN:
              if (rs485_rx_counter==L_CAM_GET_CUR_WIN_CONF)
                {
                  rs485_com_len = rs485_rx_counter;
                  Frame_Packet_Size = USART6BufferRx[7] + (USART6BufferRx[8]<<8);
                  Frame_Packets = USART6BufferRx[9] + (USART6BufferRx[10]<<8);
                  if (Frame_Packet_Size <= FRAME_PACKET_MAXSIZE)
                  {
//                    if (USART6->SR & USART_SR_ORE)
//                    {
//                      tmp = USART6->DR;
//                    }
                    com_sr_cam |= COM_CAM_GET_CUR_WIN;
//                    USART6->CR3 |= USART_CR3_DMAR;
                    cur_buf = 0;
                  }
                }
              break;
            case CAM_DEF_LABELS:
              if ((rs485_rx_counter==L_CAM_DEF_LABELS_CONF)||(rs485_rx_counter==L_CAM_DEF_LABELS_ANSW))
              {
                com_sr_cam |= COM_CAM_DEF_LABELS;
                rs485_com_len = rs485_rx_counter;
              }
              break;
            }
            rs485_rx_counter=0;
            rs485_cs_rx=0;
          }
          break;
        
        case MH_ID:
          if ((rs485_rx_counter>=3) && (rs485_cs_rx==0))
          {
            if ((USART6BufferRx[1]==hello) && (rs485_rx_counter==l_hello))
              {
                com_sr_mh |= com_hello;
              }
            rs485_rx_counter=0;
            rs485_cs_rx=0;
          }
          break;

        case H1_ID:
          if ((rs485_rx_counter>=3) && (rs485_cs_rx==0))
          {
            switch (USART6BufferRx[1])
            {
            case RESIDUAL_DISCHARGE:
              if (rs485_rx_counter==L_RESIDUAL_DISCHARGE_CONF )
              {
                com_sr_h1 |= COM_RESIDUAL_DISCHARGE;
              }
              break;
            case RD_TIMELEFT:
              if (rs485_rx_counter==L_RD_TIMELEFT_ANSW )
              {
                com_sr_h1 |= COM_RD_TIMELEFT;
              }
              break;
            }
            rs485_rx_counter=0;
            rs485_cs_rx=0;
          }
          break;
        }

        if (rs485_rx_counter>=USART6_CMD_LEN)
        {
          USART6BufferRx[0]=0;
          rs485_rx_counter=0;
          rs485_cs_rx=0;
        }

    }

    if (USART6->SR & USART_SR_TC)        // rc_w0
    {
      USART6->SR = ~USART_SR_TC;        // rc_w0
      GPIO_WriteBit(RxTx_Switch_GPIO_Port, RxTx_Switch_Pin, Bit_RESET);
      rs485_f &= ~RS485_TX_BUSY;
//      USART6->CR1 &= ~USART_CR1_TCIE;
    }
//    if (USART_GetITStatus(USART6, USART_IT_ORE_RX))
//    {
//      tmp = USART6->DR;
//    }
  }
  
  void DMA2_Stream1_IRQHandler()        // USART6_RX
  {
    if (DMA_GetITStatus(DMA2_Stream1, DMA_IT_TCIF1))
    {
        DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);
        rs485_f |= RS485_RX_FULL;
    }
    if (DMA_GetITStatus(DMA2_Stream1, DMA_IT_HTIF1))
    {
        DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_HTIF1);
    }
    if (DMA_GetITStatus(DMA2_Stream1, DMA_IT_TEIF1))
    {
        DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TEIF1);
    }
    if (DMA_GetITStatus(DMA2_Stream1, DMA_IT_FEIF1))
    {
        DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_FEIF1);
    }
    if (DMA_GetITStatus(DMA2_Stream1, DMA_IT_DMEIF1))
    {
        DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_DMEIF1);
    }
  }

  void DMA2_Stream6_IRQHandler()        // USART6_TX
  {
    if (DMA_GetITStatus(DMA2_Stream6, DMA_IT_TCIF6))
    {
        DMA_ClearITPendingBit(DMA2_Stream6, DMA_IT_TCIF6);
//        USART6->CR1 |= USART_CR1_TCIE;
    }
    if (DMA_GetITStatus(DMA2_Stream6, DMA_IT_HTIF6))
    {
        DMA_ClearITPendingBit(DMA2_Stream6, DMA_IT_HTIF6);
    }
    if (DMA_GetITStatus(DMA2_Stream6, DMA_IT_TEIF6))
    {
        DMA_ClearITPendingBit(DMA2_Stream6, DMA_IT_TEIF6);
    }
    if (DMA_GetITStatus(DMA2_Stream6, DMA_IT_FEIF6))
    {
        DMA_ClearITPendingBit(DMA2_Stream6, DMA_IT_FEIF6);
    }
    if (DMA_GetITStatus(DMA2_Stream6, DMA_IT_DMEIF6))
    {
        DMA_ClearITPendingBit(DMA2_Stream6, DMA_IT_DMEIF6);
    }
  }

}