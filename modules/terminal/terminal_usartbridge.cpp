#include "terminal_usartbridge.h"

#include <string.h>
#include <stdio.h>
//#include "macroses.h"

#include "FreeRTOS.h"
#include "task.h"

//Usart bridge object for global terminal
UsartBridgeTerminal terminalUsartBridge;

//Pointer to global terminal object
extern Terminal* pTerminal;

#ifdef __GNUC__    
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)    
#else    
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)    
#endif  

PUTCHAR_PROTOTYPE   
{   
    terminalUsartBridge.uart->DR = ch & 0x01FF;
    while(USART_GetFlagStatus(terminalUsartBridge.uart, USART_FLAG_TXE) == RESET) {}   
    return ch; 
}  

//-----------------------------------------------------------------------------
// Connect usartBridge objectt with ANSI C Interfaces
//-----------------------------------------------------------------------------

extern "C" {

//Task processing usart bridge with infinity loop
void task_usartbridge(void *pvParameters)
{
    terminalUsartBridge.configure();
    while(1) {
        terminalUsartBridge.processing();
    }
}

//Configure usart bridge
void configure_usartbridge() {
    terminalUsartBridge.configure();
}

//Processing usart bridge without infinity loop
void processing_usartbridge() {
    terminalUsartBridge.processing();
}

//Print callback for terminal object
void print_usartbridge(const char * str) {
    terminalUsartBridge.print(str);
}


}

void vTaskUsartBridge(void *params)
{
  for (;;)
  {
    terminalUsartBridge.processing();
    vTaskDelay(10);
  }
}


//-----------------------------------------------------------------------------
// Usart Bridge for terminal
//-----------------------------------------------------------------------------
UsartBridgeTerminal::UsartBridgeTerminal():
    m_sizeUartRecv(0),
    m_sizeCache(0),
    m_readIndex(0),
    m_isTxNoEmpty(false)
{
    memset(&m_recvBuf, 0, sizeof(m_recvBuf));
    memset(&m_recvCache, 0 , sizeof(m_recvCache));
}

UsartBridgeTerminal::~UsartBridgeTerminal()
{
}


//Configure hardware and link with terminal
void UsartBridgeTerminal::configure()
{
  
    //Terminal::configure();
	
    m_sizeUartRecv = 0;
    m_sizeCache = 0;
    m_readIndex = 0;
    m_isTxNoEmpty = false;

    memset(&m_recvBuf, 0, sizeof(m_recvBuf));
    memset(&m_recvCache, 0 , sizeof(m_recvCache));
	
    uart = USART_BRIDGE_USART;
    InitUsart();

    //Add usart print to global terminal object
    pTerminal->addPrint(::print_usartbridge);
}


//Processing usart data and add to terminal
void UsartBridgeTerminal::processing()
{
    memset(&m_recvCache, 0, sizeof(m_recvCache));
    if (m_sizeUartRecv > 0)
        ReadDataToCache();
    
    if (m_sizeCache > 0)
    {
        //ReadDataToCache();
        //pTerminal->print(m_recvCache);
        while (m_readIndex < m_sizeCache) {
            pTerminal->insert(m_recvCache[m_readIndex]);            
            m_readIndex++;
        }
        m_readIndex = 0;
        m_sizeCache = 0;
    }    
}


//print function
void UsartBridgeTerminal::print(const char *str)
{
    int index = 0;
    int len = strlen(str);
    if (len <= 0)
      return;

    while (index < len) {
        while (m_isTxNoEmpty);
        sendByte(str[index++]);
    }
}







//-----------------------------------------------------------------------------
// Hardware usart function
//-----------------------------------------------------------------------------
/*
extern "C" {

void USART_BRIDGE_IRQHandler(void) 
{
    if(USART_GetITStatus(USART_BRIDGE_USART, USART_IT_RXNE) != RESET)
    {
        terminalUsartBridge.recvByte();
    }
    
    if(USART_GetITStatus(USART_BRIDGE_USART, USART_IT_TXE) != RESET)
    {   
        USART_ITConfig(USART_BRIDGE_USART, USART_IT_TXE, DISABLE);
        terminalUsartBridge.clearTxNoEmpty();
    }   
}
}
*/
//Recv byte from usart
void UsartBridgeTerminal::recvByte()
{
    if (m_sizeUartRecv == TERMINAL_RECVBUF_SIZE)
        return;

    uint32_t index = m_sizeUartRecv;
    
    m_recvBuf[index] =
        (char)(uart->DR & (uint16_t)0x01FF);

    m_sizeUartRecv++;
}



//Send byte to usart
void UsartBridgeTerminal::sendByte(uint16_t byte)
{
    m_isTxNoEmpty = true;
    uart->DR = (byte & (uint16_t)0x01FF);
    USART_ITConfig(USART_BRIDGE_USART, USART_IT_TXE, ENABLE);
}



//Clear TX No Empty flag
void UsartBridgeTerminal::clearTxNoEmpty() {
    //Это не нужно....
    //__HAL_UART_CLEAR_FLAG(&uart, UART_FLAG_TXE);
    //__HAL_UART_DISABLE_IT(&uart, UART_IT_TXE);
    m_isTxNoEmpty = false;
}



//Copy cached data to processing buffer
void UsartBridgeTerminal::ReadDataToCache()
{
    do {
        m_sizeCache = m_sizeUartRecv;
        memcpy(m_recvCache, m_recvBuf, m_sizeUartRecv);
    }
    while (!CAS(&m_sizeUartRecv, m_sizeCache, 0));
}



//Atomic compare and swap function
bool UsartBridgeTerminal::CAS(volatile unsigned long* ptr,
    uint32_t value_old, uint32_t value_new)
{
    if(__LDREXW(ptr) == value_old) {
        return __STREXW(value_new, ptr) == 0;
    }
    __CLREX();
    return false;
}


//Init USART
void UsartBridgeTerminal::InitUsart()
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    USART_InitTypeDef   USART_InitStructure;
//    NVIC_InitTypeDef    NVIC_InitStructure;
    
    USART_BRIDGE_PORT_CLK_ENABLE;
    USART_BRIDGE_CLK_ENABLE;
  
    GPIO_InitStructure.GPIO_Pin = USART_BRIDGE_PINS;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(USART_BRIDGE_PORT, &GPIO_InitStructure);
    
    GPIO_PinAFConfig(USART_BRIDGE_PORT, USART_BRIDGE_PIN_SOURCE_1, USART_BRIDGE_ALTERNATE);
    GPIO_PinAFConfig(USART_BRIDGE_PORT, USART_BRIDGE_PIN_SOURCE_2, USART_BRIDGE_ALTERNATE); 
    
    
    USART_InitStructure.USART_BaudRate = USART_BRIDGE_SPEED;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART_BRIDGE_USART, &USART_InitStructure);
    
    USART_Cmd(USART_BRIDGE_USART, ENABLE);      
    
    
//    NVIC_InitStructure.NVIC_IRQChannel = USART_BRIDGE_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
  
//    NVIC_SetPriority (USART_BRIDGE_IRQn, 5); 

    USART_ITConfig(USART_BRIDGE_USART, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART_BRIDGE_USART, USART_IT_TXE, DISABLE);
}




