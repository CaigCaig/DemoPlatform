   
#include "led.h"

/**
  * @brief  
  */
void LED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    GPIO_StructInit(&GPIO_InitStructure);				/* Init Structure */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void LED_Toggle(void)
{
    GPIOC->ODR ^= GPIO_Pin_0;
}