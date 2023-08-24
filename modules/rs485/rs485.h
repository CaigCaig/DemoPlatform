#ifndef __RS485_H
#define __RS485_H

#define usart6_tx_Pin GPIO_Pin_6
#define usart6_tx_GPIO_Port GPIOC
#define usart6_rx_Pin GPIO_Pin_7
#define usart6_rx_GPIO_Port GPIOC
#define RxTx_Switch_Pin GPIO_Pin_12
#define RxTx_Switch_GPIO_Port GPIOC

//#define USART6_SPEED    10500000 // max 7500000
//#define USART6_SPEED    5250000 // max 7500000
//#define USART6_SPEED    115200
#define USART6_SPEED    1312500
#define USART6_CMD_LEN  ((uint32_t)40)
#define USART6_DATA_LEN ((uint32_t)1500)
#define USART6_CONF_LEN ((uint32_t)12)
#define USART6_ANSW_LEN ((uint32_t)10)
#define RS485_TX_BUSY   ((uint8_t)0x01)
#define RS485_RX_FULL   ((uint8_t)0x02)

void RS485_Init(void);
void vUART_WatchDog(void *params);
void RS485_Send(uint8_t *addr, uint32_t len);

#endif // __RS485_H