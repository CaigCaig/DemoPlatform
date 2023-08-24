#ifndef __CH_DCH_H
#define __CH_DCH_H

#define chdch_select_ch0_Pin GPIO_Pin_5
#define chdch_select_ch0_GPIO_Port GPIOB
#define chdch_select_ch1_Pin GPIO_Pin_4
#define chdch_select_ch1_GPIO_Port GPIOB
#define chdch_select_io_Pin GPIO_Pin_3
#define chdch_select_io_GPIO_Port GPIOB
#define chdch_en_Pin GPIO_Pin_7
#define chdch_en_GPIO_Port GPIOD

#define chdch_A_B_Pin GPIO_Pin_0
#define chdch_A_B_GPIO_Port GPIOD
#define dch_en_Pin GPIO_Pin_2
#define dch_en_GPIO_Port GPIOD
#define ch_en_Pin GPIO_Pin_1
#define ch_en_GPIO_Port GPIOD
#define chdch_D7_Pin GPIO_Pin_7
#define chdch_D6_Pin GPIO_Pin_6
#define chdch_D5_Pin GPIO_Pin_5
#define chdch_D4_Pin GPIO_Pin_4
#define chdch_D3_Pin GPIO_Pin_3
#define chdch_D2_Pin GPIO_Pin_2
#define chdch_D1_Pin GPIO_Pin_1
#define chdch_D0_Pin GPIO_Pin_0
#define chdch_D_GPIO_Port GPIOE
#define fan_state_Pin GPIO_Pin_4
#define fan_state_GPIO_Port GPIOA
#define fan_en_Pin GPIO_Pin_5
#define fan_en_GPIO_Port GPIOA

#define CHARGER         ((uint8_t)0)
#define DISCHARGER      ((uint8_t)1)

void CH_DCH_Init(void);
void CH_DCH_SET(uint8_t device, uint16_t current);      // значение тока передавать в десятых долях ампера. Т.е. если ток нужен 36А, передать нужно значение 360
uint8_t ReadStateChDch(uint8_t device);

#endif // ___CH_DCH_H