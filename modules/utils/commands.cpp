#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"

#include "commands.h"
#include "net_bridge.h"
#include "msens_settings.h"
#include "rtc.h"

#include <string.h>

#define COM_COUNT   4
COMMANDS_t commands[COM_COUNT];


// -----------------------------------------------------------------------------
// Command Fandlers

// IP_CONFIG
void NetParams(void)
{
    netBridge.printNetParams();
}

// SET_DEF
void SetDefault(void)
{
    options.setDefault();
    NVIC_SystemReset();
}

// -----------------------------------------------------------------------------
// 

//
void vCommands(void *params)
{
  for (;;)
  {
    vTaskDelay(100);
    COM_Launcher();
  }
}

//
void COM_Launcher(void)
{
  for (uint8_t i = 0; i < COM_COUNT; i++)
  {
    // Если установлен флаг команды вызываем обработчик и снимаем флаг
    if (commands[i].flag) {
      commands[i].flag = false;
      commands[i].handler();
    }  
  }
}

//
void COM_Configure(void)
{  
  commands[IP_CONFIG].flag = false;
  commands[IP_CONFIG].handler = &NetParams;
  
  commands[SET_DEF].flag = false;
  commands[SET_DEF].handler = &SetDefault;
  
  commands[LWIP_STAT].flag = false;
  commands[LWIP_STAT].handler = &PrintLwipStat;
  
  commands[TIME].flag = false;
  commands[TIME].handler = &PrintTime;
}

//
void PrintLwipStat(void)
{
    stats_display();
    printf("\r\n\r\n");
}

//
void PrintTime(void)
{
    TM_RTC_t time;
    
    TM_RTC_GetDateTime(&time, TM_RTC_Format_BIN);
    printf("Time: %02d.%02d.%02d %02d:%02d:%02d:%d\r\n", time.date, time.month, time.year, time.hours, time.minutes, time.seconds, time.subseconds);
}
