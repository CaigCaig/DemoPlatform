/*******************************************************************************
*  FILENAME: frtosWrapper.cpp
*
*  DESCRIPTION: реализация класса frtosWrapper
*
*  Copyright (c) 2015 by Sergey Sikharulidze and Sergey Kolody
*
*******************************************************************************/

#include "frtosWrapper.h" 

cRTOS oRTOS;

/*******************************************************************************
* Function:  constructor
* Description: 
* Threading usage and Assumptions:  none
******************************************************************************/
cRTOS::cRTOS(void)
{

}

/*******************************************************************************
* Function:  startScheduler
* Description: Запуск планировщика
* Threading usage and Assumptions:  Все задачи должны быть созданы с помощью 
*  taskCreate() до вызова данной функции
******************************************************************************/
void cRTOS::startScheduler(void) const         
{
   vTaskStartScheduler();
}

/*******************************************************************************
* Function:  taskCreate
* Description: Создание задача, в качестве задачи будет использоваться 
* статический метод run класса cRTOS, в котором будет вызываться реальный метод 
* объекта наследника iActiveObject
* Threading usage and Assumptions:  none
******************************************************************************/
tRtosStatus cRTOS::taskCreate(iActiveObject *pActiveObject, 
                              const tU16 stackDepth, tU32 priority, const char * const name)
{
   tRtosStatus status;   
   const BaseType_t rowStatus = xTaskCreate((TaskFunction_t)cRTOS::run, name, 
                                            stackDepth, pActiveObject, priority,
                                            &pActiveObject->taskHandle);  //lint !e929 тип tTaskFunction используется для совместимости с код стандартом
   
   if(rowStatus == pdTRUE)
   {
      status = RS_pass;
   }
   else
   {
      status = RS_fail;
   }
   return status;
}  //lint !e1762 !e952
   //1762 - функция логически не является const.
   //952 - параметр taskCode логически не является const

/*******************************************************************************
* Function:  taskDelay
* Description: Приостановка задачи
* Threading usage and Assumptions:  none
******************************************************************************/
void cRTOS::taskDelay(const tU32 timeIncrement) const
{
   vTaskDelay((TickType_t)timeIncrement);
}

/*******************************************************************************
* Function:  schedulerDisable
* Description: Остановка планировщика
* Threading usage and Assumptions:  none
******************************************************************************/
void cRTOS::schedulerDisable(void) const
{
   vTaskSuspendAll();
}

/*******************************************************************************
* Function:  schedulerEnable
* Description: Восстановление работы планировщика
* Threading usage and Assumptions:  none
******************************************************************************/
tRtosStatus cRTOS::schedulerEnable(void) const
{
   tRtosStatus status;
   const BaseType_t rowStatus = xTaskResumeAll();
   
   if(rowStatus == pdTRUE)
   {
      status = RS_pass;
   }
   else
   {
      status = RS_fail;
   }
   return status;
}

/*******************************************************************************
* Function:  eventGroupCreate
* Description: Создание event group
* Threading usage and Assumptions:  none
******************************************************************************/
tEventGroupHandle cRTOS::eventGroupCreate(void)
{
   tEventGroupHandle createdEventGroup;
   
   createdEventGroup = (tEventGroupHandle)xEventGroupCreate();       //lint !e925 тип tEventGroupHandle используется для совместимости с код стандартом
   
   return createdEventGroup;
}//lint !e1762 функция логически не является const.

/*******************************************************************************
* Function:  eventGroupSetBits
* Description: Устанавливает биты в event group
* Threading usage and Assumptions:  none
******************************************************************************/
tU32 cRTOS::eventGroupSetBits(tEventGroupHandle eventGroup,
                                     const tU32 bitsToSet)
{
   tU32 bits;

   bits = (tU32)xEventGroupSetBits((EventGroupHandle_t)eventGroup, (EventBits_t)bitsToSet);  //lint !e925 тип tEventGroupHandle используется для совместимости с код стандартом
   
   /* From FreeRTOS documentation:
      There are two reasons why the returned value might have the bits specified
      by the uxBitsToSet parameter cleared:
      1. If setting a bit results in a task that was waiting for the bit leaving
         the	blocked state then it is possible the bit will have been cleared 
         automatically (see the xClearBitOnExit parameter of xEventGroupWaitBits()).
      2. Any unblocked (or otherwise Ready state) task that has a priority above
         that of the task that called xEventGroupSetBits() will execute and may 
         change the event group value before the call to xEventGroupSetBits()
         returns. */
   return bits;
}//lint !e1762 функция логически не является const.

/*******************************************************************************
* Function:  eventGroupSetBitsFromISR
* Description: Версия eventGroupSetBits, вызываемая из обработчика прерывания 
* Threading usage and Assumptions:  none
******************************************************************************/
tRtosStatus cRTOS::eventGroupSetBitsFromISR(tEventGroupHandle eventGroup,
                                          const tU32 bitsToSet)
{
   tRtosStatus status;
   BaseType_t result;
   BaseType_t xHigherPriorityTaskWoken;
   
   xHigherPriorityTaskWoken = (BaseType_t)FALSE;

   result = xEventGroupSetBitsFromISR((EventGroupHandle_t)eventGroup, (EventBits_t)bitsToSet, &xHigherPriorityTaskWoken);  //lint !e925 !e632
                                                                                                                                 // 1. - тип tEventGroupHandle используется для совместимости с код стандартом
                                                                                                                                 // 2. - функция xTimerPendFunctionCallFromISR (см. event_groups.h) имеет 4 аргумента,
                                                                                                                                 // первый из которых имеет тип 'PendedFunction_t
   if( result != pdFAIL )
   {
      /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context
      switch should be requested.  The macro used is port specific and will
      be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - refer to
      the documentation page for the port being used. */
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);       //lint !e923 !e632 приведение типа unsigned long к указателю, не строгое приведение к uint32 было осуществлено в библиотеке STM

      status = RS_pass;
   }
   else
   {
      status = RS_fail;       //timer service queue was full
   }
   
   return status;
}//lint !e1762 функция логически не является const


/*******************************************************************************
* Function:  eventGroupClearBits
* Description: Очищает выбранные биты в event group. Возвращает значение event 
*  group до того как выбранные биты были очищены
* Threading usage and Assumptions:  none
******************************************************************************/
tU32 cRTOS::eventGroupClearBits(tEventGroupHandle eventGroup,
                                      const tU32 bitsToClear)
{
   tU32 bits;
   bits = (tU32)xEventGroupClearBits((EventGroupHandle_t)eventGroup, (EventBits_t)bitsToClear); //lint !e925 тип tEventGroupHandle используется для совместимости с код стандартом
   
   return bits;
}//lint !e1762 функция логически не является const

/*******************************************************************************
* Function:  eventGroupClearBitsFromISR
* Description: Версия eventGroupClearBits, вызываемая из обработчика прерывания.
*  Возвращает значение event group до того как выбранные биты были очищены.
* Threading usage and Assumptions:  none
******************************************************************************/
tU32 cRTOS::eventGroupClearBitsFromISR(tEventGroupHandle eventGroup,
                                      const tU32 bitsToClear)
{
   tU32 bits;
   bits = (tU32)xEventGroupClearBitsFromISR((EventGroupHandle_t)eventGroup, (EventBits_t)bitsToClear);   //lint !e925 !e632
                                                                                                         // 1. - тип tEventGroupHandle используется для совместимости с код стандартом
                                                                                                         // 2. - функция xTimerPendFunctionCallFromISR (см. event_groups.h) имеет 4 аргумента,
                                                                                                         // первый из которых имеет тип 'PendedFunction_t'.
   return bits;
}//lint !e1762 функция логически не является const

/*******************************************************************************
* Function:  eventGroupWaitBits
* Description: Ожидает установки выбранных битов в event group и держит задачу в
               блокированном состоянии в течение времени ticksToWait
* Threading usage and Assumptions:  none
******************************************************************************/
tU32 cRTOS::eventGroupWaitBits(const tEventGroupHandle eventGroup,
                                      const tU32 bitsToWaitFor,
                                      const tBoolean clearOnExit,
                                      const tBoolean waitForAllBits,
                                      tU32 ticksToWait)
{
   tU32 uxBits;
   uxBits = (tU32)xEventGroupWaitBits((EventGroupHandle_t)eventGroup, (EventBits_t)bitsToWaitFor,                    //lint !e925 тип tEventGroupHandle используется для совместимости с код стандартом
                                (BaseType_t)clearOnExit, (BaseType_t)waitForAllBits, (TickType_t)ticksToWait);
   
   return uxBits;
}//lint !e1762 функция логически не является const

/*******************************************************************************
* Function:  
* Description: 
* Threading usage and Assumptions:  
******************************************************************************/
tSemaphoreHandle cRTOS::semaphoreCreate()
{  
  return xSemaphoreCreateBinary();
}

void cRTOS::semaphoreGiveFromISR(tSemaphoreHandle semaphore, tBase * base)
{
  xSemaphoreGiveFromISR((SemaphoreHandle_t)semaphore, base);
}

void cRTOS::semaphoreTake(tSemaphoreHandle semaphore)
{
  xSemaphoreTake((SemaphoreHandle_t)semaphore, NULL);
}

/*******************************************************************************
* Function:  queueCreate
* Description: Создает очередь, в случае нехватки памяти возвращает NULL
* Threading usage and Assumptions:  none
******************************************************************************/
tQueueHandle cRTOS::queueCreate( const tU32 queueLength, const tU32 itemSize)
{
  tQueueHandle queueHandle =  NULL;
  queueHandle = (tQueueHandle)xQueueCreate(queueLength, itemSize);
  return queueHandle;                
}

/*******************************************************************************
* Function:  queueSend
* Description: Добавление элемента в очередь
* Threading usage and Assumptions:  none
******************************************************************************/
tRtosStatus cRTOS::queueSend(tQueueHandle queue, const void * pItemToQueue, tU32 ticksToWait)
{
  tRtosStatus status;
  status = this->queueSendToBack(queue,pItemToQueue, ticksToWait);
  return status;                
}

/*******************************************************************************
* Function:  queueSend
* Description: Добавление элемента в начало очереди
* Threading usage and Assumptions:  none
******************************************************************************/
tRtosStatus cRTOS::queueSendToFront(tQueueHandle queue, const void * pItemToQueue, tU32 ticksToWait)
{
  tRtosStatus status;
  status = (tRtosStatus)xQueueSendToFront((QueueHandle_t)queue, pItemToQueue, (TickType_t)ticksToWait);
  return status;                
}

/*******************************************************************************
* Function:  queueSend
* Description: Добавление элемента в очередь
* Threading usage and Assumptions:  none
******************************************************************************/
tRtosStatus cRTOS::queueSendToBack(tQueueHandle queue, const void * pItemToQueue, tU32 ticksToWait)
{
  tRtosStatus status;
  status = (tRtosStatus)xQueueSend((QueueHandle_t)queue, pItemToQueue, (TickType_t)ticksToWait);
  return status;                
}

/*******************************************************************************
* Function:  queueReceive
* Description: Прием из очереди
* Threading usage and Assumptions:  none
******************************************************************************/
tBoolean cRTOS::queueReceive(tQueueHandle queue, void * pBuffer, tU32 ticksToWait)
{
  tBoolean status;
  status = (tBoolean)xQueueReceive((QueueHandle_t)queue, pBuffer, (TickType_t)ticksToWait);
  return status;                
}


/*******************************************************************************
* Function:  taskNotify
* Description: Нотификация нужной задачи
* Threading usage and Assumptions:  none
******************************************************************************/
tRtosStatus cRTOS::taskNotify(tTaskHandle taskToNotify, tU32 value, tNotifyAction eAction)
{
  tRtosStatus status = RS_fail;
  const BaseType_t rowStatus = xTaskNotify(taskToNotify, value, eAction);
  if(rowStatus == pdTRUE)
   {
      status = RS_pass;
   }
   else
   {
      status = RS_fail;
   }
   return status;                
}


/*******************************************************************************
* Function:  taskNotifyWait
* Description: Ожидание нотификации от других задач
* Threading usage and Assumptions:  none
******************************************************************************/
tBoolean cRTOS::taskNotifyWait(tU32 bitsToClearOnEntry, tU32 BitsToClearOnExit, tU32 *pValue, tU32 ticksToWait)
{
  tBoolean status;
  status = xTaskNotifyWait(bitsToClearOnEntry, BitsToClearOnExit, (uint32_t *)pValue, (TickType_t)ticksToWait);
  return status;                
}

/*******************************************************************************
* Function:  run
* Description: Функция являющаяся задачей в операционной системе, она статическая
*              потому что при создании задачи передается указатель на конкретную
*              функцию. Сама функция вызывает метод run объекта который наследует
*              интерфейс iActiveObject. Любой объект, который хочет быть задачей
*              должен унаследовать этот интерфейс и реализовать функцию run
* Threading usage and Assumptions:  none
******************************************************************************/
void cRTOS::run(void *parameters)
{
  
  ((iActiveObject*)parameters)->run();
	
}