#include "stm32f4xx.h"
#include "pwm_test.h"

uint32_t period;  

// Генерирует ШИМ TIM1 Channel1 PA8
void PWM_InitTestSignal(void)
{
    TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
    TIM_OCInitTypeDef         TIM_OCInitStructure;
    GPIO_InitTypeDef          GPIO_InitStructure;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);  
    
    TIM_DeInit(TIM1);
    TIM_TimeBaseStructure.TIM_Prescaler = 83; 
    TIM_TimeBaseStructure.TIM_Period = 19999;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 2000;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

// Захват ШИМ сигнала
void PWM_InitCaptureTimer(void)
{
    TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
    TIM_ICInitTypeDef         TIM_ICInitStructure;
    NVIC_InitTypeDef          NVIC_InitStructure;
    GPIO_InitTypeDef          GPIO_InitStructure;
  
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
  
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3);  
    
    TIM_DeInit(TIM3);
    TIM_TimeBaseStructure.TIM_Prescaler = 83;   
    TIM_TimeBaseStructure.TIM_Period = 65535;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode	= TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;     
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; 
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;           
    TIM_ICInitStructure.TIM_ICFilter = 0;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    
    TIM_Cmd(TIM3, ENABLE);

    TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  // Захват ШИМ
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

extern "C" {

// Захват входного PWM
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)  
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
        
        period = TIM_GetCapture1(TIM3);
        TIM_SetCounter(TIM3, 0);
    }
}

}