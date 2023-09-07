#include "main.h"
#include "systick.h"
#include "FreeRTOS.h"
#include "task.h"

#include "device_commands.h"
#include "mod_config.h"
#include "msens_settings.h"
//#include "terminal_medsens.h"
//#include "terminal_usartbridge.h"
#include "commands.h"
#include "net_bridge.h"
#include "proto_mqtt.h"
//#include "proto_tcp.h"
//#include "pcm1808.h"
#include "mcp3422.h"
#include "24AA025E48.h"
//#include "stm32_ub_dac_dma.h"
#include "rtc.h"
#include "leds.h"
#include "rs485.h"
#include "ach_adch.h"
#include "ch_dch.h"
#include "batteries.h"
#include "periph_watchdog.h"
#include "httpserver-socket.h"

#include <stdio.h>

// Размещение кучи FreeRTOS в CCRAM 
// Перенос таблицы векторов прерываний осуществляется в функции SystemInit. 
// Смещение определяется также в файле system_stm32f4xx.c
#pragma location = ".sram"
//uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
//TaskHandle_t xADC;
//TaskHandle_t xMProtoMQTT;
extern uint8_t MCP3422_State, MCP3422_Calc_Fail;
char buf[1024]={0};
uint8_t charging_sr;

extern xTaskHandle TaskWatch;

static void vInit(void* params);

void main(void)
{
    
 	__disable_irq();
	NVIC_SetPriorityGrouping(0);
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	__enable_irq();

    
//    ST_Init(1000);
    
    xTaskCreate(vInit, "Init", 6*configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
    
    vTaskStartScheduler();
    
    while (1) {}
}


void vInit(void* params)
{
//    // Random 
//    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
//    RNG_Cmd(ENABLE);
  
    // Настройки
//    options.loadSettings();
    options.setDefault();
  
    // RTC
//    TM_RTC_Init(TM_RTC_ClockSource_External);
//    TM_RTC_Init(TM_RTC_ClockSource_Internal);
    TM_RTC_Init(TM_RTC_ClockSource_HSE);
    xTaskCreate(vRTC, "Watch", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &TaskWatch);
//    vTaskSuspend(TaskWatch);
    
    // Терминал
//    medsensTerminal.configure();
    
//    terminalUsartBridge.configure();
//    xTaskCreate(vDbgMsgTerminal, "Debug_Msn", 2*configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
//    xTaskCreate(vTaskUsartBridge, "UsartBridge_Task", 2*configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
    
    // Команды консоли
//    COM_Configure();
//    xTaskCreate(vCommands, "Commands", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
    
    
    // DAC
//    DAC_InitChannel1();
    
    // Внешний ADC PCM1808
//    adcPCM.configure();

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_DMA1 | RCC_AHB1Periph_DMA2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 | RCC_APB1Periph_UART4 | RCC_APB1Periph_UART5 | RCC_APB1Periph_I2C1 | RCC_APB1Periph_SPI2 | RCC_APB1Periph_TIM7, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_USART6, ENABLE);
    
    // инициализация портов двигателей
    GPIO_InitTypeDef hgpio;
    GPIO_WriteBit(SM1_STEP_GPIO_Port, SM1_STEP_Pin, Bit_RESET);
    GPIO_WriteBit(SM2_DIR_GPIO_Port, SM2_DIR_Pin, Bit_SET);
    GPIO_WriteBit(SM1_ENB_GPIO_Port, SM1_ENB_Pin | SM1_DIR_Pin | SM2_ENB_Pin | SM3_ENB_Pin, Bit_SET);
    GPIO_WriteBit(SM2_STEP_GPIO_Port, SM2_STEP_Pin | SM3_STEP_Pin, Bit_RESET);
    GPIO_WriteBit(SM3_DIR_GPIO_Port, SM3_DIR_Pin, Bit_SET);
    hgpio.GPIO_Pin = SM1_ENB_Pin | SM1_DIR_Pin | SM2_ENB_Pin | SM2_STEP_Pin | SM3_ENB_Pin | SM3_STEP_Pin;
    hgpio.GPIO_Speed = GPIO_Speed_2MHz;
    hgpio.GPIO_Mode = GPIO_Mode_OUT;
    hgpio.GPIO_OType = GPIO_OType_PP;
    hgpio.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(SM1_ENB_GPIO_Port, &hgpio);
    hgpio.GPIO_Pin = SM1_STEP_Pin | SM2_DIR_Pin;
    GPIO_Init(SM1_STEP_GPIO_Port, &hgpio);
    hgpio.GPIO_Pin = SM3_DIR_Pin;
    GPIO_Init(SM3_DIR_GPIO_Port, &hgpio);
    
//    power36v_GPIO_Port->MODER &= ~GPIO_MODER_MODER9;
//    power36v_GPIO_Port->MODER |= GPIO_MODER_MODER9_0;        // порт PE9 для включения RSP-1600-36 устанавливаем в режим OUTPUT
//    
//    // GPIO (Fix Charger, Fix Discharger)
//    CH_DCH_Init();

    // I2C3 (ADC, MAC_IC)
    ADC_Init();
    extern eeprom_t eeprom_data;
    Load_Net_Config();  // чтение сетевых параметров из микросхемы 24AA025E48
    if (eeprom_data.eeprom_key!=EEPROM_KEY)
    {
      eeprom_data.eeprom_dhcp = EEPROM_DHCP;
      eeprom_data.eeprom_ip = EEPROM_IP;
      eeprom_data.eeprom_mask =  EEPROM_MASK;
      eeprom_data.eeprom_gate =  EEPROM_GATE;
      Save_Net_Config();
    }
//    MCP3422_State = OK;
//    extern TaskHandle_t xADC;   // задаём дескриптор задачи для возможности управлению ею
//    if (ADC_Config()!=ERROR)    // если конфигурирование АЦП прошло без ошибок
//    {
//      xTaskCreate(vADC, "ADC", 2*configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xADC );     // создаём задачу циклического опроса АЦП
//    }
//    else        // если конфигурирование АЦП прошло с ошибкой
//    {
//      xTaskCreate(vADC, "ADC", 2*configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xADC );     // создаём задачу циклического опроса АЦП
//      vTaskSuspend(xADC);       // и тут же её приостанавливаем
//    }

    // SPI (2 LED indicators)
//    SPI2_Init();
//    xTaskCreate(vWatch1, "Watch1", 2*configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );
//    xTaskCreate(vWatch2, "Watch2", 2*configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );
//    xTaskCreate(vLEDs, "LEDs", 2*configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL );
    
    // RS485 (MasterHood, Hood1, Hood2)
    RS485_Init();
    // USART3, UART4, UART5 (Analog Charger A, Analog Charger B, Analog Discharger)
//    ACH_ADCH_Init();

    xTaskCreate(vUART_WatchDog, "UART_WD", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );

    // Ethernet
//    vTaskDelay(100);
//    vTaskList(buf);
//    printf(buf);

    netBridge.configure();

//    vTaskList(buf);
//    printf(buf);
    http_server_socket_init();
    
    #ifdef WEBSERVER
    http_server_web_init();
    #endif
//    vTaskList(buf);
//    printf(buf);
    
    // Тесты
//    // Копирование данных с ADC
//    xTaskCreate(vCopyAdcData, "ADC", 2*configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );

    vTaskDelay(1500);
    xTaskCreate(vPERIPH_WATCHDOG, "PERIPH_WATCHDOG", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY, NULL );
    vTaskDelete(NULL);
}
