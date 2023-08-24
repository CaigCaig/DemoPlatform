/******************************* (C) LiteMesh **********************************
 * @module  systick
 * @file    systick.c
 * @version 1.0.0
 * @date    29.05.2015
 * $brief   Functions to work with System Timer. 
 *          Module is based on code teplofizik, 2013
 *******************************************************************************
 * @history     Version  Author         Comment
 * 29.05.2015   1.0.0    Telenkov D.A.  First release.
 *******************************************************************************
 */
   
#include "stm32f4xx.h"
#include "systick.h"
#include <stdbool.h>
#include <string.h>

typedef struct
{
  TTimerHandler Handler;
  uint16_t      Countdown;
  uint16_t      Reload;
  bool          Run;    // Запущено
  bool          Fired;  // Сработало
    
} THandlers;

volatile uint32_t  TimingDelay;
volatile uint8_t   StopDelayFlag = 0;

static THandlers Handlers[TIMER_HANDLERS];
static uint32_t  TimerFrequency;
static int       TimerCount;


/**
  * @brief  Обработчик прерывания системного таймера
  * @retval нет
  */
/*
void SysTick_Handler(void)
{
  int i;
 
  TimingDelay_Decrement();
    
  for(i = 0; i < TIMER_HANDLERS; i++)
  {
    if(Handlers[i].Run)
    {
      if(--Handlers[i].Countdown == 0)
      {
        Handlers[i].Countdown = Handlers[i].Reload;

	// Запишем, что сработало 
	Handlers[i].Fired = true;
      }
    }
  }
}
*/
/**
  * @brief  Запуск таймера с заданной частотой
  * @retval нет
  */
void ST_Init(uint32_t Frequency)
{
  if(!Frequency) return;

  /* Сбросим параметры */
  //TimerCount = 0;
  //TimerFrequency = Frequency;
  //memset(&Handlers[0], 0, sizeof(Handlers));

  /* Настройка частоты */
  SysTick_Config(ST_GetSYS() / Frequency);
}

/**
  * @brief  Добавить функцию в список вызова. 
  *         Handler будет вызываться с заданной частотой
  * @retval нет
  */
void ST_AddFunction(uint16_t Frequency, TTimerHandler Handler)
{
  int i;

  for(i = 0; i < TIMER_HANDLERS; i++)
  {
    /* Найдем пустой слот */
    if(!Handlers[i].Handler)
    {
      /* Обработчик, частота опроса */
      Handlers[i].Run       =  true;
      Handlers[i].Fired     =  false;
      Handlers[i].Handler   =  Handler;
      Handlers[i].Reload    =  Frequency;
      Handlers[i].Countdown =  Handlers[i].Reload;

      TimerCount++;

      return;
    }
  }
}

/**
  * @brief  Изменить частоту таймера
  * @retval нет
  */
void ST_ChangeFrequency(TTimerHandler Handler, uint16_t Frequency)
{
  int i;

  for(i = 0; i < TIMER_HANDLERS; i++)
  {
    if(Handlers[i].Handler == Handler)
    {
      Handlers[i].Reload = (TimerFrequency / Frequency);
      Handlers[i].Countdown = Handlers[i].Reload;
      break;
    }
  }
}

/**
  * @brief  Возобночить работу задачи
  * @retval нет
  */
void ST_Resume(TTimerHandler Handler)
{
  int i;

  for(i = 0; i < TIMER_HANDLERS; i++)
  {
    if(Handlers[i].Handler == Handler)
    {
      Handlers[i].Run = true;
      break;
    }
  }
}

/**
  * @brief  Перезапуск задачи
  * @retval нет
  */
void ST_Restart(TTimerHandler Handler)
{
  int i;

  for(i = 0; i < TIMER_HANDLERS; i++)
  {
    if(Handlers[i].Handler == Handler)
    {
      Handlers[i].Run = true;
      Handlers[i].Countdown = Handlers[i].Reload;
      break;
    }
  }
}

/**
  * @brief  Перезапуск задачи. Задача будет вызвана сразу.
  * @retval нет
  */
void ST_RestartAtOnce(TTimerHandler Handler)
{
  int i;

  for(i = 0; i < TIMER_HANDLERS; i++)
  {
    if(Handlers[i].Handler == Handler)
    {
      Handlers[i].Run = true;
      Handlers[i].Countdown = 1;
      break;
    }
  }
}

/**
  * @brief  Остановить задачу
  * @retval нет
  */
void ST_Stop(TTimerHandler Handler)
{
  int i;

  for(i = 0; i < TIMER_HANDLERS; i++)
  {
    if(Handlers[i].Handler == Handler)
    {
      Handlers[i].Run = false;
      Handlers[i].Fired =  false;
      break;
    }
  }
}

/**
  * @brief  Функция перебора и вызова актуальных задач. 
  *         Должна вызываться в главном цикле
  * @retval нет
  */
void ST_Main(void)
{
  int i;

  for(i = 0; i < TIMER_HANDLERS; i++)
  {
    /* Если сработало - вызовем */
    if(Handlers[i].Fired)
    {
      Handlers[i].Fired = false;
      Handlers[i].Handler();
    }
  }
}

/**
  * @brief  
  * @retval Частота тактирования ядра
  */
uint32_t ST_GetSYS(void)
{
  return SystemCoreClock;
}

/**
  * @brief  Задержка в миллисекундах
  * @retval нет
  */
void Delay_ms(uint32_t nTime)
{
  TimingDelay = nTime;
  while(TimingDelay);
}

/**
  * @brief  Вспомогательная функция для реализации Delay_ms
  * @retval нет
  */
void TimingDelay_Decrement(void) {

  if (TimingDelay)
    TimingDelay--;
}



//******************************** (C) POTEK ***********************************
