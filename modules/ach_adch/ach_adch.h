#ifndef __ACH_ADCH_H
#define __ACH_ADCH_H

#define acha_tx_Pin GPIO_Pin_6
#define acha_tx_GPIO_Port GPIOB
#define acha_rx_Pin GPIO_Pin_7
#define acha_rx_GPIO_Port GPIOB
#define achb_tx_Pin GPIO_Pin_10
#define achb_tx_GPIO_Port GPIOC
#define achb_rx_Pin GPIO_Pin_11
#define achb_rx_GPIO_Port GPIOC
#define adch_tx_Pin GPIO_Pin_5
#define adch_tx_GPIO_Port GPIOD
#define adch_rx_Pin GPIO_Pin_6
#define adch_rx_GPIO_Port GPIOD

#define UART_ACHA       USART1
#define UART_ACHB       UART4
#define UART_ADCH       USART2
#define ACHA_IRQHandler USART1_IRQHandler
#define ACHB_IRQHandler UART4_IRQHandler
#define ADCH_IRQHandler USART2_IRQHandler
#define ACHA_IRQn       USART1_IRQn
#define ACHB_IRQn       UART4_IRQn
#define ADCH_IRQn       USART2_IRQn
#define UART_ACHA_SPEED 9600
#define ACHA_CMD_LEN  ((uint32_t)8)
#define UART_ACHB_SPEED 9600
#define ACHB_CMD_LEN  ((uint32_t)8)
#define UART_ADCH_SPEED 9600
#define ADCH_CMD_LEN   ((uint32_t)8)
#define USART_TX_BUSY   ((uint8_t)0x01)
#define UART_TX_BUSY   USART_TX_BUSY
#define ACHA_DMA_Channel        DMA_Channel_4
#define ACHB_DMA_Channel        DMA_Channel_4
#define ADCH_DMA_Channel        DMA_Channel_4

void ACH_ADCH_Init(void);
uint8_t ACH_Set(uint8_t current);

#endif // ___ACH_ADCH_H