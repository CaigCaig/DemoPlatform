
#include "uart_cli.h"
#include "uart_bridge.h"
#include "port_microrl.h"
#include "systick.h"
#include "ad7415.h"
#include "timers.h"
#include "led.h"
#include "rtc.h"
#include "adc.h"
#include "lcd_hd44780.h"
#include "FreeRTOS.h"
#include "task.h"

#include <stdio.h>

void vTest(void* params);

void main(void)
{
/*  
    CLI_UART_Init();    // Порт для консоли
    MICRORL_Init();     // Консоль
    LED_Init();         // Мигалка "Живой"
    
    // Тесты
    TMR_Init();         // Таймеры для сигналов с ПЛИС
    uRTC_Init();
    uADC_Init();

    ST_Init(1000);
    
    ST_AddFunction(500, &TIMR_Debug);
*/    
    CLI_UART_Init();    // Порт для консоли
    ST_Init(1000);
  
    lcd.configure();
    
    xTaskCreate(vTest, "Test", 2*configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
    
    vTaskStartScheduler();
    
    while (1)
    {
        //ST_Main();
        
    }
}

void vTest(void* params)
{
    static uint16_t i = 0;
    
    for (;;)
    {
        i++;
        printf("Counter: %d\r\n", i);
        lcd.print("Hello world!");
        vTaskDelay(1000);
    }
}
