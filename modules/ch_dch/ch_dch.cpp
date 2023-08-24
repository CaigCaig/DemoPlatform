#include "stm32f4xx.h"
#include "ch_dch.h"
#include "FreeRTOS.h"
#include "task.h"
#include "mb_def.h"
#include "com_handler.h"

void CH_DCH_Init(void)
{
  GPIO_InitTypeDef hgpio;
  
    /** Fix Charger/Discharger GPIO Configuration
    PI4, PI5     ------> select channel (chdch_select_ch0_Pin, chdch_select_ch1_Pin)
    PI6     ------> select I(0)/O(1) (chdch_select_io_Pin)
    PI7     ------> enable IO (0) (chdch_en_Pin)
    PF12    ------> charger enable (0) (ch_en_Pin)
    PF13    ------> charger switch (A-"0", B-"1") (ch_A_B_Pin)
    PF14    ------> discharger enable (0) (ch_en_Pin)
    PF15    ------> discharger switch (A-"0", B-"1") (dch_A_B_Pin)
    */

  GPIO_WriteBit(fan_en_GPIO_Port, fan_en_Pin, Bit_RESET);
  
  GPIO_WriteBit(chdch_en_GPIO_Port, chdch_en_Pin, Bit_SET);
  GPIO_WriteBit(chdch_select_ch0_GPIO_Port, chdch_select_ch0_Pin | chdch_select_ch1_Pin | chdch_select_io_Pin, Bit_RESET);

  GPIO_WriteBit(chdch_D_GPIO_Port, chdch_D0_Pin | chdch_D1_Pin | chdch_D2_Pin | chdch_D3_Pin | chdch_D4_Pin | chdch_D5_Pin | chdch_D6_Pin | chdch_D7_Pin, Bit_SET);
  GPIO_WriteBit(chdch_A_B_GPIO_Port, chdch_A_B_Pin, Bit_SET);
  GPIO_WriteBit(ch_en_GPIO_Port, ch_en_Pin | dch_en_Pin, Bit_RESET);

  hgpio.GPIO_Pin = fan_en_Pin;
  hgpio.GPIO_Speed = GPIO_Speed_2MHz;
  hgpio.GPIO_Mode = GPIO_Mode_OUT;
  hgpio.GPIO_OType = GPIO_OType_PP;
  hgpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(fan_en_GPIO_Port, &hgpio);

  hgpio.GPIO_Pin = fan_state_Pin;
  hgpio.GPIO_Mode = GPIO_Mode_IN;
  hgpio.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(fan_state_GPIO_Port, &hgpio);

  hgpio.GPIO_Pin = chdch_select_ch0_Pin | chdch_select_ch1_Pin | chdch_select_io_Pin;
  hgpio.GPIO_Speed = GPIO_Speed_2MHz;
  hgpio.GPIO_Mode = GPIO_Mode_OUT;
  hgpio.GPIO_OType = GPIO_OType_PP;
  hgpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(chdch_select_ch0_GPIO_Port, &hgpio);
  hgpio.GPIO_Pin = chdch_en_Pin;
  GPIO_Init(chdch_en_GPIO_Port, &hgpio);

  hgpio.GPIO_Pin = chdch_D0_Pin | chdch_D1_Pin | chdch_D2_Pin | chdch_D3_Pin | chdch_D4_Pin | chdch_D5_Pin | chdch_D6_Pin | chdch_D7_Pin;
  hgpio.GPIO_Speed = GPIO_Speed_2MHz;
  hgpio.GPIO_Mode = GPIO_Mode_IN;
  hgpio.GPIO_OType = GPIO_OType_PP;
  hgpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(chdch_D_GPIO_Port, &hgpio);

  hgpio.GPIO_Pin = chdch_A_B_Pin | dch_en_Pin | ch_en_Pin;
  hgpio.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(chdch_A_B_GPIO_Port, &hgpio);

  GPIO_WriteBit(chdch_select_ch1_GPIO_Port, chdch_select_ch1_Pin | chdch_select_io_Pin, Bit_SET);
  GPIO_WriteBit(chdch_select_ch0_GPIO_Port, chdch_select_ch0_Pin, Bit_RESET);
  GPIO_WriteBit(chdch_en_GPIO_Port, chdch_en_Pin, Bit_RESET);
  vTaskDelay(1);
  GPIO_WriteBit(chdch_en_GPIO_Port, chdch_en_Pin, Bit_SET);
  vTaskDelay(1);

  GPIO_WriteBit(chdch_select_ch1_GPIO_Port, chdch_select_ch1_Pin, Bit_RESET);
  vTaskDelay(1);
  GPIO_WriteBit(chdch_en_GPIO_Port, chdch_en_Pin, Bit_RESET);
  vTaskDelay(1);
  GPIO_WriteBit(chdch_en_GPIO_Port, chdch_en_Pin, Bit_SET);
  GPIO_WriteBit(chdch_en_GPIO_Port, chdch_select_io_Pin, Bit_RESET);
  vTaskDelay(1);

  chdch_D_GPIO_Port->MODER &= ~(GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0 | GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0);

}

void CH_DCH_SET(uint8_t device, uint16_t current)       // значение тока передавать в десятых долях ампера. Т.е. если ток нужен 36А, передать нужно значение 360
{
  switch (device)
  {
  case CHARGER:
    GPIO_WriteBit(dch_en_GPIO_Port, dch_en_Pin, Bit_RESET);      // отключаем выходы разрядника от аккумулятора
    GPIO_WriteBit(chdch_select_ch1_GPIO_Port, chdch_select_ch0_Pin | chdch_select_ch1_Pin, Bit_RESET);
    GPIO_WriteBit(chdch_A_B_GPIO_Port, chdch_A_B_Pin, Bit_SET);    // RESET для аккумулятора A, SET для аккумулятора B
    GPIO_WriteBit(ch_en_GPIO_Port, ch_en_Pin, Bit_SET);     // подключаем выходы зарядника к аккумулятору
    break;
  case DISCHARGER:
    GPIO_WriteBit(ch_en_GPIO_Port, ch_en_Pin, Bit_RESET);       // отключаем выходы зарядника от аккумулятора
    GPIO_WriteBit(chdch_select_ch0_GPIO_Port, chdch_select_ch0_Pin, Bit_RESET);
    GPIO_WriteBit(chdch_select_ch1_GPIO_Port, chdch_select_ch1_Pin, Bit_SET);
//    GPIO_WriteBit(chdch_A_B_GPIO_Port, chdch_A_B_Pin, Bit_SET);   // RESET для аккумулятора A, SET для аккумулятора B
    if (current)                                                // если устанавливаемый ток разряда не равен 0
    {
      GPIO_WriteBit(dch_en_GPIO_Port, dch_en_Pin, Bit_SET);    // подключаем выходы разрядника к аккумулятору
    }
    else
    {
      GPIO_WriteBit(dch_en_GPIO_Port, dch_en_Pin, Bit_RESET);  // отключаем выходы разрядника от аккумулятора
    }
    break;
  }
  vTaskDelay(1000);
  uint8_t nums_9A =current/90;  // вычисляем количество 9А каналов
  if (nums_9A > 3) nums_9A = 3;
  chdch_D_GPIO_Port->MODER |= GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0 | GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0;
  switch (nums_9A)      // определяем каналы дискретных зарядников (0, 2-7 - каналы по 9 А, 1 канал на 4,5 А)
  {
  case 0:
    GPIO_WriteBit(fan_en_GPIO_Port, fan_en_Pin, Bit_RESET);
    GPIO_WriteBit(chdch_D_GPIO_Port, chdch_D0_Pin | chdch_D1_Pin | chdch_D2_Pin, Bit_SET);
    break;
  case 1:
    GPIO_WriteBit(fan_en_GPIO_Port, fan_en_Pin, Bit_SET);
    GPIO_WriteBit(chdch_D_GPIO_Port, chdch_D1_Pin | chdch_D2_Pin, Bit_SET);
    GPIO_WriteBit(chdch_D_GPIO_Port, chdch_D0_Pin, Bit_RESET);
    break;
  case 2:
    GPIO_WriteBit(fan_en_GPIO_Port, fan_en_Pin, Bit_SET);
    GPIO_WriteBit(chdch_D_GPIO_Port, chdch_D2_Pin, Bit_SET);
    GPIO_WriteBit(chdch_D_GPIO_Port, chdch_D0_Pin | chdch_D1_Pin, Bit_RESET);
    break;
  case 3:
    GPIO_WriteBit(fan_en_GPIO_Port, fan_en_Pin, Bit_SET);
    GPIO_WriteBit(chdch_D_GPIO_Port, chdch_D0_Pin | chdch_D1_Pin | chdch_D2_Pin, Bit_RESET);
    break;

/*
  case 0:
    GPIO_WriteBit(chdch_D_GPIO_Port, chdch_D0_Pin | chdch_D2_Pin | chdch_D3_Pin | chdch_D4_Pin | chdch_D5_Pin | chdch_D6_Pin | chdch_D7_Pin, Bit_SET);
    break;
  case 1:
    GPIO_WriteBit(chdch_D_GPIO_Port, chdch_D2_Pin | chdch_D3_Pin | chdch_D4_Pin | chdch_D5_Pin | chdch_D6_Pin | chdch_D7_Pin, Bit_SET);
    GPIO_WriteBit(chdch_D_GPIO_Port, chdch_D0_Pin, Bit_RESET);
    break;
  case 2:
    GPIO_WriteBit(chdch_D_GPIO_Port, chdch_D3_Pin | chdch_D4_Pin | chdch_D5_Pin | chdch_D6_Pin | chdch_D7_Pin, Bit_SET);
    GPIO_WriteBit(chdch_D_GPIO_Port, chdch_D0_Pin | chdch_D2_Pin, Bit_RESET);
    break;
  case 3:
    GPIO_WriteBit(chdch_D_GPIO_Port, chdch_D4_Pin | chdch_D5_Pin | chdch_D6_Pin | chdch_D7_Pin, Bit_SET);
    GPIO_WriteBit(chdch_D_GPIO_Port, chdch_D0_Pin | chdch_D2_Pin | chdch_D3_Pin, Bit_RESET);
    break;
  case 4:
    GPIO_WriteBit(chdch_D_GPIO_Port, chdch_D5_Pin | chdch_D6_Pin | chdch_D7_Pin, Bit_SET);
    GPIO_WriteBit(chdch_D_GPIO_Port, chdch_D0_Pin | chdch_D2_Pin | chdch_D3_Pin | chdch_D4_Pin, Bit_RESET);
    break;
  case 5:
    GPIO_WriteBit(chdch_D_GPIO_Port, chdch_D6_Pin | chdch_D7_Pin, Bit_SET);
    GPIO_WriteBit(chdch_D_GPIO_Port, chdch_D0_Pin | chdch_D2_Pin | chdch_D3_Pin | chdch_D4_Pin | chdch_D5_Pin, Bit_RESET);
    break;
  case 6:
    GPIO_WriteBit(chdch_D_GPIO_Port, chdch_D7_Pin, Bit_SET);
    GPIO_WriteBit(chdch_D_GPIO_Port, chdch_D0_Pin | chdch_D2_Pin | chdch_D3_Pin | chdch_D4_Pin | chdch_D5_Pin | chdch_D6_Pin, Bit_RESET);
    break;
  case 7:
    GPIO_WriteBit(chdch_D_GPIO_Port, chdch_D0_Pin | chdch_D2_Pin | chdch_D3_Pin | chdch_D4_Pin | chdch_D5_Pin | chdch_D6_Pin | chdch_D7_Pin, Bit_RESET);
    break;
*/

  }

/*
  uint16_t residual = current-nums_9A*90;       // разностный ток между заданным значением тока и током через дискретные каналы
  if (residual>45)      // определяем, включать или нет канал на 4,5А
  {
    GPIO_WriteBit(chdch_D_GPIO_Port, chdch_D1_Pin, Bit_RESET);
  }
  else
  {
    GPIO_WriteBit(chdch_D_GPIO_Port, chdch_D1_Pin, Bit_SET);
  }
*/
  GPIO_WriteBit(chdch_select_io_GPIO_Port, chdch_select_io_Pin, Bit_SET);
  vTaskDelay(1);
  GPIO_WriteBit(chdch_en_GPIO_Port, chdch_en_Pin, Bit_RESET);
  vTaskDelay(1);
  GPIO_WriteBit(chdch_en_GPIO_Port, chdch_en_Pin, Bit_SET);
  GPIO_WriteBit(chdch_select_io_GPIO_Port, chdch_select_io_Pin, Bit_RESET);
  chdch_D_GPIO_Port->MODER &= ~(GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0 | GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0);
}

uint8_t ReadStateChDch(uint8_t device)
{
  uint8_t state;
  chdch_D_GPIO_Port->MODER &= ~(GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0 | GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0);
  GPIO_WriteBit(chdch_en_GPIO_Port, chdch_select_io_Pin, Bit_RESET);
  switch (device)
  {
  case CHARGER:
    GPIO_WriteBit(chdch_en_GPIO_Port, chdch_select_ch1_Pin, Bit_SET);
    break;
  case DISCHARGER:
    GPIO_WriteBit(chdch_en_GPIO_Port, chdch_select_ch1_Pin, Bit_RESET);
    break;
  }
  GPIO_WriteBit(chdch_en_GPIO_Port, chdch_en_Pin, Bit_RESET);
  state = uint8_t (chdch_D_GPIO_Port->IDR);
  GPIO_WriteBit(chdch_en_GPIO_Port, chdch_en_Pin, Bit_SET);
  return state;
}
