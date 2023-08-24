#include "stm32_ub_dac_dma.h"

#include <stdio.h>

extern uint16_t leftChanForDac[];
extern uint16_t rightChanForDac[];

extern uint16_t leftChanForDacTmp[];
extern uint16_t rightChanForDacTmp[];

extern uint16_t leftChanForDacF[];
extern uint16_t rightChanForDacF[];

extern uint16_t leftChanForDacTmpF[];
extern uint16_t rightChanForDacTmpF[];

DAC_InitTypeDef  DAC_InitStructure;
//DMA_InitTypeDef  DMA_InitStructure;
DAC_DMA_MODE_t akt_dac_dma_mode;


void P_DAC_DMA_InitIO1(void);
void P_DAC_DMA_InitTIM1(void);
void P_DAC_DMA_InitDAC1(void);
void P_DAC_DMA_InitIO2(void);
void P_DAC_DMA_InitTIM2(void);
void P_DAC_DMA_InitDAC2(void);



//--------------------------------------------------------------
// Стандартный сигнал DAC=OFF
//--------------------------------------------------------------
const uint16_t WaveOff[] = {0}; // постоянный низкий уровень

//--------------------------------------------------------------
// Сигнал создается в виде массива
// Размер 12 бит (от 0 до 4095)
//--------------------------------------------------------------
const uint16_t Wave1[] = { // Синусоидальный
  2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056,
  4095, 4056, 3939, 3750, 3495, 3185, 2831, 2447,
  2047, 1647, 1263,  909,  599,  344,  155,   38,
     0,   38,  155,  344,  599,  909, 1263, 1647
};


//--------------------------------------------------------------
// Сигнал создается в виде массива
// Размер 12 бит (от 0 до 4095)
//--------------------------------------------------------------
const uint16_t Wave2[] = { // Пилообразный
     0,  256,  512,  768, 1024, 1280, 1536, 1792,
  2048, 2304, 2560, 2816, 3072, 3328, 3584, 3840,
  4095
};


//--------------------------------------------------------------
// Сигнал создается в виде массива
// Размер 12 бит (от 0 до 4095)
//--------------------------------------------------------------
const uint16_t Wave3[] = { // Треугольный
     0,  256,  512,  768, 1024, 1280, 1536, 1792,
  2048, 2304, 2560, 2816, 3072, 3328, 3584, 3840,
  4095, 3840, 3584, 3328, 3072, 2816, 2560, 2304,
  2048, 1792, 1536, 1280, 1024,  768,  512,  256
};


//--------------------------------------------------------------
// Сигнал создается в виде массива
// Размер 12 бит (от 0 до 4095)
//--------------------------------------------------------------
const uint16_t Wave4[] = { // Прямоугольный
     0,  0,  4095,  4095
};

const uint16_t Wave5[] = { 
                            1861, 1950, 2038, 2126, 2213, 2300, 2385, 2470, 2552, 2633,
                            2712, 2790, 2864, 2936, 3006, 3073, 3137, 3198, 3255, 3309,
                            3360, 3407, 3450, 3489, 3524, 3555, 3582, 3605, 3623, 3638,
                            3648, 3653, 3654, 3650, 3643, 3631, 3615, 3594, 3569, 3540,
                            3507, 3470, 3429, 3383, 3335, 3282, 3227, 3168, 3105, 3040,
                            2971, 2900, 2827, 2751, 2673, 2593, 2511, 2428, 2343, 2257,
                            2170, 2082, 1994, 1905, 1817, 1728, 1640, 1552, 1465, 1379,
                            1294, 1210, 1129, 1049,  970,  895,  821,  750,  682,  617,
                            554,   495,  440,  387,  339,  293,  252,  215,  182,  153,
                            128,   107,   90,   79,   71,   68,   69,   74,   84,   99,
                            117,   140,  167,  198,  233,  272,  315,  362,  413,  467,
                            524,   585,  649,  716,  786,  858,  932, 1010, 1089, 1170,
                            1252, 1337, 1422, 1509, 1596, 1684, 1772, 1860,
/*  
                            2048, 2145, 2242, 2339, 2435, 2530, 2624, 2717, 2808, 2897, 
                           2984, 3069, 3151, 3230, 3307, 3381, 3451, 3518, 3581, 3640, 
                           3696, 3748, 3795, 3838, 3877, 3911, 3941, 3966, 3986, 4002, 
                           4013, 4019, 4020, 4016, 4008, 3995, 3977, 3954, 3926, 3894, 
                           3858, 3817, 3772, 3722, 3669, 3611, 3550, 3485, 3416, 3344, 
                           3269, 3191, 3110, 3027, 2941, 2853, 2763, 2671, 2578, 2483, 
                           2387, 2291, 2194, 2096, 1999, 1901, 1804, 1708, 1612, 1517, 
                           1424, 1332, 1242, 1154, 1068,  985,  904,  826,  751,  679, 
                            610,  545,  484,  426,  373,  323,  278,  237,  201,  169, 
                            141,  118,  100,   87,   79,   75,   76,   82,   93,  109, 
                            129,  154,  184,  218,  257,  300,  347,  399,  455,  514, 
                            577,  644,  714,  788,  865,  944, 1026, 1111, 1198,  1287, 
                           1378, 1471, 1565, 1660, 1756, 1853, 1950, 2047 
*/  
};   

//--------------------------------------------------------------
// Определение всех сигналов
// Перечисляются в DAC_DMA_WAVE_NAME_t
//--------------------------------------------------------------
const DAC_WAVE_t DAC_WAVE[] = {
  // Name             , Kurve  , Lдnge
  {DAC_WAVE_OFF       , WaveOff, 1},
  {DAC_WAVE1_SINUS    , Wave1  , 32},
  {DAC_WAVE2_SAEGEZAHN, Wave2  , 17},
  {DAC_WAVE3_DREIECK  , Wave3  , 32},
  {DAC_WAVE4_RECHTECK , Wave4  , 4},
  {DAC_WAVE5_SINUS_ADD, Wave5  , 128},
  
};




//--------------------------------------------------------------
// Инициализация ЦАП в режиме DMA
// режим: [SINGLE_DAC1_DMA, SINGLE_DAC2_DMA, DUAL_DAC_DMA]
//--------------------------------------------------------------
void UB_DAC_DMA_Init(DAC_DMA_MODE_t mode)
{
  // режим работы ЦАП
  akt_dac_dma_mode=mode;

  if((mode==SINGLE_DAC1_DMA) || (mode==DUAL_DAC_DMA)) {
    P_DAC_DMA_InitIO1();
    P_DAC_DMA_InitTIM1();
    P_DAC_DMA_InitDAC1();
    UB_DAC_DMA_SetWaveform1(DAC_WAVE_OFF);
  }
  if((mode==SINGLE_DAC2_DMA) || (mode==DUAL_DAC_DMA)) {
    P_DAC_DMA_InitIO2();
    P_DAC_DMA_InitTIM2();
    P_DAC_DMA_InitDAC2();
    UB_DAC_DMA_SetWaveform2(DAC_WAVE_OFF);
  }
}


//--------------------------------------------------------------
// Выбор сигнала для DAC1
//--------------------------------------------------------------
void UB_DAC_DMA_SetWaveform1(DAC_DMA_WAVE_NAME_t wave)
{
  DMA_InitTypeDef DMA_InitStructure;

  if(akt_dac_dma_mode==SINGLE_DAC2_DMA) return; //если не разрешено

  // включение тактирования (DMA)
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); 

  // инициализация DMA (DMA1, Channel7, Stream5)
  DMA_Cmd(DMA1_Stream5, DISABLE);
  DMA_DeInit(DMA1_Stream5);
  DMA_InitStructure.DMA_Channel = DMA_Channel_7;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)DAC1_DHR_12R1_ADDRESS;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)DAC_WAVE[wave].ptr;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_BufferSize = DAC_WAVE[wave].wave_len;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA1_Stream5, &DMA_InitStructure);

  // включение DMA
  DMA_Cmd(DMA1_Stream5, ENABLE);
  // включение DMA-DAC 
  DAC_DMACmd(DAC_Channel_1, ENABLE);
}


//--------------------------------------------------------------
// Выбор сигнала для DAC2
//--------------------------------------------------------------
void UB_DAC_DMA_SetWaveform2(DAC_DMA_WAVE_NAME_t wave)
{
  DMA_InitTypeDef DMA_InitStructure;

  if(akt_dac_dma_mode==SINGLE_DAC1_DMA) return; //если не разрешено

  // включение тактирования (DMA)
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

  // инициализация DMA (DMA1, Channel7, Stream6)
  DMA_Cmd(DMA1_Stream6, DISABLE);
  DMA_DeInit(DMA1_Stream6);
  DMA_InitStructure.DMA_Channel = DMA_Channel_7;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)DAC2_DHR_12R2_ADDRESS;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)DAC_WAVE[wave].ptr;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_BufferSize = DAC_WAVE[wave].wave_len;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA1_Stream6, &DMA_InitStructure);

  // включение DMA 
  DMA_Cmd(DMA1_Stream6, ENABLE);
  // включение DMA-DAC 
  DAC_DMACmd(DAC_Channel_2, ENABLE);
}


//--------------------------------------------------------------
// Установка частоты DAC1
// Основная частота = 2*APB1 (APB1=42MHz) => TIM_CLK=84MHz
// делитель: 0 bis 0xFFFF
// период: 0 bis 0xFFFF
//
// выходная частота = TIM_CLK/делитель+1)/период+1)/длинна волны
//--------------------------------------------------------------
void UB_DAC_DMA_SetFrq1(uint16_t vorteiler, uint16_t periode)
{
  TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;

  if(akt_dac_dma_mode==SINGLE_DAC2_DMA) return; //если не разрешено

  // Конфигурация таймера
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Period = periode;
  TIM_TimeBaseStructure.TIM_Prescaler = vorteiler;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(DAC_TIM1_NAME, &TIM_TimeBaseStructure);
}


//--------------------------------------------------------------
// Установка частоты DAC2
// Основная частота = 2*APB1 (APB1=42MHz) => TIM_CLK=84MHz
// делитель: 0 bis 0xFFFF
// период: 0 bis 0xFFFF
//
// выходная частота = TIM_CLK/делитель+1)/период+1)/длинна волны
//--------------------------------------------------------------
void UB_DAC_DMA_SetFrq2(uint16_t vorteiler, uint16_t periode)
{
  TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
 
  if(akt_dac_dma_mode==SINGLE_DAC1_DMA) return; //если не разрешено

  // Конигурация таймера
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Period = periode;
  TIM_TimeBaseStructure.TIM_Prescaler = vorteiler;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(DAC_TIM2_NAME, &TIM_TimeBaseStructure);
}


//--------------------------------------------------------------
// внутренняя функция
// Инициализация IO-выводов как DAC1
//--------------------------------------------------------------
void P_DAC_DMA_InitIO1(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  // Включение тактирования (GPIO)
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  // Конфигурирование выводов ЦАП как аналоговый выход
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}


//--------------------------------------------------------------
// внутренняя функция
// Инициализация IO-выводов как DAC2
//--------------------------------------------------------------
void P_DAC_DMA_InitIO2(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  // Включение тактирования (GPIO)
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  // Конфигурирование выводов ЦАП как аналоговый выход
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}


//--------------------------------------------------------------
// внутренняя функция
// Инициализация таймера для DAC1
//--------------------------------------------------------------
void P_DAC_DMA_InitTIM1(void)
{
  TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;

  // Включение тактирования (Timer)
  RCC_APB1PeriphClockCmd(DAC_TIM1_CLK, ENABLE);

  // Конигурация таймера
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Period = DAC_DMA_DEF_PERIODE;
  TIM_TimeBaseStructure.TIM_Prescaler = DAC_DMA_DEF_PRESCALE;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(DAC_TIM1_NAME, &TIM_TimeBaseStructure);

  // Триггер
  TIM_SelectOutputTrigger(DAC_TIM1_NAME, TIM_TRGOSource_Update);

  // Включение таймера
  TIM_Cmd(DAC_TIM1_NAME, ENABLE);
}


//--------------------------------------------------------------
// внутренняя функция
// Инициализация таймера для DAC2
//--------------------------------------------------------------
void P_DAC_DMA_InitTIM2(void)
{
  TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;

  // Включение тактирования (Timer)
  RCC_APB1PeriphClockCmd(DAC_TIM2_CLK, ENABLE);

  // Конигурация таймера
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Period = DAC_DMA_DEF_PERIODE;
  TIM_TimeBaseStructure.TIM_Prescaler = DAC_DMA_DEF_PRESCALE;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(DAC_TIM2_NAME, &TIM_TimeBaseStructure);

  // Триггер
  TIM_SelectOutputTrigger(DAC_TIM2_NAME, TIM_TRGOSource_Update);

  // Включение таймера
  TIM_Cmd(DAC_TIM2_NAME, ENABLE);
}


//--------------------------------------------------------------
// внутренняя функция
// Инициализация DAC1
//--------------------------------------------------------------
void P_DAC_DMA_InitDAC1(void)
{
  // Включение тактирования (DAC)
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

  // Инициализация DAC
  DAC_InitStructure.DAC_Trigger = DAC_TIM1_TRG;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);

  // Включение DAC
  DAC_Cmd(DAC_Channel_1, ENABLE);
}


//--------------------------------------------------------------
// внутренняя функция
// Инициализация DAC2
//--------------------------------------------------------------
void P_DAC_DMA_InitDAC2(void)
{
  // Включение тактирования (DAC)
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

  // Инициализация DAC
  DAC_InitStructure.DAC_Trigger = DAC_TIM2_TRG;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_Init(DAC_Channel_2, &DAC_InitStructure);

  // Включение DAC
  DAC_Cmd(DAC_Channel_2, ENABLE);
}

//--------------------------------------------------------------
// Дополнительные функции для сигналов с внешнего ADC
//--------------------------------------------------------------

//
void UB_DAC_DMA_SetAdcChannel1(ADC_CHANNEL_t channel)
{
  DMA_InitTypeDef DMA_InitStructure;

  if(akt_dac_dma_mode==SINGLE_DAC2_DMA) return; //если не разрешено

  // включение тактирования (DMA)
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); 

  // инициализация DMA (DMA1, Channel7, Stream5)
  DMA_Cmd(DMA1_Stream5, DISABLE);
  DMA_DeInit(DMA1_Stream5);
  DMA_InitStructure.DMA_Channel = DMA_Channel_7;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)DAC1_DHR_12R1_ADDRESS;
  
  if (channel == LEFT) {
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)leftChanForDacTmp;
    DMA_InitStructure.DMA_BufferSize = 16;
  }
  else if (channel == RIGHT) {
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)rightChanForDacTmp;
    DMA_InitStructure.DMA_BufferSize = 16;
  }
    
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA1_Stream5, &DMA_InitStructure);

  // включение DMA
  DMA_Cmd(DMA1_Stream5, ENABLE);
  // включение DMA-DAC 
  DAC_DMACmd(DAC_Channel_1, ENABLE);
}

const uint16_t testWaweform[3] = {0, 1000, 2000};

// PA4
void DAC_InitChannel1(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //DAC_InitTypeDef  DAC_InitStructure;
    DMA_InitTypeDef  DMA_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

        
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Частота таймера 8000 Гц (TIM_Prescaler = 11; TIM_Period = 875)
    // Частота таймера 1000 Гц (TIM_Prescaler = 95; TIM_Period = 875)
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 95;
    TIM_TimeBaseStructure.TIM_Period = 874;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
  
    TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
    TIM_Cmd(TIM6, ENABLE);
    
    //DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);
    DAC_Cmd(DAC_Channel_1, ENABLE);
    
  
    DMA_Cmd(DMA1_Stream5, DISABLE);
    DMA_DeInit(DMA1_Stream5);
    DMA_InitStructure.DMA_Channel = DMA_Channel_7;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)DAC1_DHR_12R1_ADDRESS;
    //DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&rightChanForDac[0];
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&rightChanForDacF[0];
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    //DMA_InitStructure.DMA_BufferSize = 16;
    DMA_InitStructure.DMA_BufferSize = 500;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream5, &DMA_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    //DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE);
    
    DMA_Cmd(DMA1_Stream5, ENABLE);
    DAC_DMACmd(DAC_Channel_1, ENABLE);
}

// 
void DAC_TimStop(void)
{
    TIM_Cmd(TIM6, DISABLE);
}

// 
void DAC_TimStart(void)
{
    TIM_Cmd(TIM6, ENABLE);
}

//
void DAC_ResetDMA(void)
{
    DMA_Cmd(DMA1_Stream5, DISABLE);
        
    DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5);
    //DMA_SetCurrDataCounter(DMA1_Stream5, 16);
    DMA_SetCurrDataCounter(DMA1_Stream5, 500);
        
    DMA_Cmd(DMA1_Stream5, ENABLE);
    
}

extern "C" {
  
void DMA1_Stream5_IRQHandler()
{
    if(DMA_GetITStatus(DMA1_Stream5, DMA_IT_TCIF5))
    {
        DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5);
        DMA_Cmd(DMA1_Stream5, DISABLE);
    }
}  

}
