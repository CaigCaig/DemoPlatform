/******************************* (C) LiteMesh **********************************
 * @module  systick
 * @file    systick.h
 * @version 1.0.0
 * @date    29.05.2015
 * $brief   Functions to work with System Timer. 
 *          Module is based on code teplofizik, 2013
 *******************************************************************************
 * @history     Version  Author         Comment
 * 29.05.2015   1.0.0    Telenkov D.A.  First release.
 *******************************************************************************
 */

#ifndef SYSTICK_H
#define SYSTICK_H

#include "stm32f4xx.h"

#define TIMER_HANDLERS    5  // Максимально количество обработчкиков

typedef void (*TTimerHandler)();

/**
  * @brief  Запуск таймера с заданной частотой
  */
void ST_Init(uint32_t Frequency);

/**
  * @brief  Добавить функцию в список вызова. 
  *         Handler будет вызываться с заданной частотой
  */
void ST_AddFunction(uint16_t Frequency, TTimerHandler Handler);

/**
  * @brief  Изменить частоту таймера
  */
void ST_ChangeFrequency(TTimerHandler Handler, uint16_t Frequency);

/**
  * @brief  Возобночить работу задачи
  */
void ST_Resume(TTimerHandler Handler);

/**
  * @brief  Перезапуск задачи
  */
void ST_Restart(TTimerHandler Handler);

/**
  * @brief  Перезапуск задачи. Задача будет вызвана сразу.
  */
void ST_RestartAtOnce(TTimerHandler Handler);

/**
  * @brief  Остановить задачу
  */
void ST_Stop(TTimerHandler Handler);

/**
  * @brief  Функция перебора и вызова актуальных задач. 
  *         Должна вызываться в главном цикле
  */
void ST_Main(void);

/**
  * @brief  
  * @retval Частота тактирования ядра
  */
uint32_t ST_GetSYS(void);

/**
  * @brief  Задержка в миллисекундах
  */
void Delay_ms(__IO uint32_t nTime);

/**
  * @brief  Вспомогательная функция для реализации Delay_ms
  */
void TimingDelay_Decrement(void);

#endif // SYSTICK_H
