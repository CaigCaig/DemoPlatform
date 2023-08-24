#ifndef __USARTBRIDGETERMINAL_H
#define __USARTBRIDGETERMINAL_H

#include "stm32f4xx.h"
#include "terminal.h"


//Usart configuration
/*
#define USART_BRIDGE_USART            USART6
#define USART_BRIDGE_SPEED            460800
#define USART_BRIDGE_IRQn             USART6_IRQn
#define USART_BRIDGE_IRQHandler       USART6_IRQHandler
#define USART_BRIDGE_ALTERNATE        GPIO_AF8_USART6
#define USART_BRIDGE_PINS             GPIO_PIN_6 | GPIO_PIN_7
#define USART_BRIDGE_PORT             GPIOC
#define USART_BRIDGE_PORT_CLK_ENABLE  __GPIOC_CLK_ENABLE
#define USART_BRIDGE_CLK_ENABLE       __USART6_CLK_ENABLE
*/
/*
#define USART_BRIDGE_USART            USART3
#define USART_BRIDGE_SPEED            460800
#define USART_BRIDGE_IRQn             USART3_IRQn
#define USART_BRIDGE_IRQHandler       USART3_IRQHandler
#define USART_BRIDGE_ALTERNATE        GPIO_AF7_USART3
#define USART_BRIDGE_PINS             GPIO_PIN_8 | GPIO_PIN_9
#define USART_BRIDGE_PORT             GPIOD
#define USART_BRIDGE_PORT_CLK_ENABLE  __GPIOD_CLK_ENABLE
#define USART_BRIDGE_CLK_ENABLE       __USART3_CLK_ENABLE
*/

/*
#define USART_BRIDGE_USART            USART3
#define USART_BRIDGE_SPEED            460800
#define USART_BRIDGE_IRQn             USART3_IRQn
#define USART_BRIDGE_IRQHandler       USART3_IRQHandler
#define USART_BRIDGE_PINS             GPIO_Pin_8 | GPIO_Pin_9
#define USART_BRIDGE_PIN_SOURCE_1     GPIO_PinSource8    
#define USART_BRIDGE_PIN_SOURCE_2     GPIO_PinSource9
#define USART_BRIDGE_ALTERNATE        GPIO_AF_USART3
#define USART_BRIDGE_PORT             GPIOD
#define USART_BRIDGE_PORT_CLK_ENABLE  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE)
#define USART_BRIDGE_CLK_ENABLE       RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

#define TERMINAL_RECVBUF_SIZE       20
*/

extern "C" {
//-----------------------------------------------------------------------------
// Connect usartBridge objectt with ANSI C Interfaces
//-----------------------------------------------------------------------------

//Task for freertos function
void task_usartbridge(void *pvParameters);

//Ansi C interface for usartBridge object
void configure_usartbridge();
void processing_usartbridge();
void print_usartbridge(const char * str);

}

void vTaskUsartBridge(void *params);

//-----------------------------------------------------------------------------
// Usart bridge class for terminal
//-----------------------------------------------------------------------------

//Warning! You not need allocate object this class. Object allocated
//once in this module.

class UsartBridgeTerminal
{
public:
    UsartBridgeTerminal();
    ~UsartBridgeTerminal();

    void configure();
    void processing();
    void print(const char *str);


public:
    void recvByte();
    void sendByte(uint16_t byte);
    void clearTxNoEmpty();

private:
    void ReadDataToCache();
    bool CAS(volatile unsigned long * ptr, uint32_t value_old, uint32_t value_new);

private:
    void InitUsart();

public:
    USART_TypeDef *uart;

private:
    char m_recvBuf[TERMINAL_RECVBUF_SIZE];
    volatile unsigned long m_sizeUartRecv;
    char m_recvCache[TERMINAL_RECVBUF_SIZE];
    uint32_t m_sizeCache;
    uint32_t m_readIndex;

    bool m_isTxNoEmpty;
};

extern UsartBridgeTerminal terminalUsartBridge;


#endif // USARTBRIDGETERMINAL_H
