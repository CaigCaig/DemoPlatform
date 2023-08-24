/*******************************************************************************
*  FILENAME: frtosWrapper.h
*
*  DESCRIPTION: обертка для FreeRTOS
*
*  Copyright (c) 2015 by SUSU
*  Author: Sergey Sikharulidze and Sergey Kolody
*******************************************************************************/
#ifndef FRTOSWRAPPER_H
#define FRTOSWRAPPER_H

#include <FreeRTOS.h>      //препроцессорные директивы для настройки компиляции RTOS
#include <task.h>          //планировщик, задачи
#include <event_groups.h>  //события
#include <queue.h>         //очереди
#include <semphr.h>
#include "types.h"         //стандартные типы проекта
#include "iactiveobject.h" //описание интерфейса iactiveobject

#define NO_TICKS_WAIT (tU32)0
//создаем типы, соответствующие кодинг стандарту, которые заменяют стандартные
//типы FreeRTOS                                       
typedef EventGroupHandle_t tEventGroupHandle;
typedef SemaphoreHandle_t tSemaphoreHandle;
typedef QueueHandle_t tQueueHandle;
typedef TaskHandle_t tTaskHandle;
typedef eNotifyAction tNotifyAction;
typedef portBASE_TYPE tBase;

typedef void (*tTaskFunction)( void * );

typedef enum
{
   RS_pass = (tU8)0,
   RS_fail = (tU8)1
}tRtosStatus;

class cRTOS
{
   public:
     explicit cRTOS(void);
     void startScheduler(void) const;            
     //lint -save -e971 Unqualified char types are allowed for strings and single characters only
     //lint -save -e970 matches library prototype
     tRtosStatus taskCreate(iActiveObject *pActiveObject, const tU16 stackDepth, tU32 priority, const char * const name);   
     //lint -restore       
     void taskDelay(const tU32 timeIncrement) const;       
     void schedulerDisable(void) const;        
     tRtosStatus schedulerEnable(void) const;        
     tEventGroupHandle eventGroupCreate(void);       
     tU32 eventGroupSetBits(tEventGroupHandle eventGroup,
                                    const tU32 bitsToSet);       
     tRtosStatus eventGroupSetBitsFromISR(tEventGroupHandle eventGroup,
                                          const tU32 bitsToSet);       
     tU32 eventGroupClearBits(tEventGroupHandle eventGroup,
                                      const tU32 bitsToClear);       
     tU32 eventGroupClearBitsFromISR(tEventGroupHandle eventGroup,
                                             const tU32 bitsToClear);       
     tU32 eventGroupWaitBits(const tEventGroupHandle eventGroup,
                                     const tU32 bitsToWaitFor,
                                     const tBoolean clearOnExit,
                                     const tBoolean waitForAllBits,
                                     tU32 ticksToWait);  
     
     tSemaphoreHandle semaphoreCreate();
     void semaphoreGiveFromISR(tSemaphoreHandle semaphore, tBase * base);  
     void semaphoreTake(tSemaphoreHandle semaphore);
     
     
     tQueueHandle queueCreate( const tU32 queueLength, const tU32 itemSize);
     tRtosStatus queueSend( tQueueHandle queue, const void * pItemToQueue, tU32 ticksToWait);
     tRtosStatus queueSendToFront( tQueueHandle queue, const void * pItemToQueue, tU32 ticksToWait);
     tRtosStatus queueSendToBack( tQueueHandle queue, const void * pItemToQueue, tU32 ticksToWait);
     tBoolean   queueReceive(tQueueHandle queue, void * pBuffer, tU32 ticksToWait);
     tRtosStatus taskNotify(tTaskHandle taskToNotify, tU32 value, tNotifyAction eAction);
     tBoolean taskNotifyWait(tU32 bitsToClearOnEntry, tU32 BitsToClearOnExit, tU32 *value, tU32 ticksToWait);
    
  private: 
    static void run(void *parameters);	
};

extern cRTOS oRTOS;

#endif //FRTOSWRAPPER_H


