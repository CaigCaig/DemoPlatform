#include "stm32f4xx.h"
#include "dac.h"

static uint16_t sinus[72]={ 2225,2402,2577,2747,2912,3070,3221,3363,3494,3615,
                            3724,3820,3902,3971,4024,4063,4085,4095,4085,4063,
							4024,3971,3902,3820,3724,3615,3495,3363,3221,3071,
							2912,2747,2577,2403,2226,2047,1869,1692,1517,1347,
							1182,1024,873, 731, 600, 479, 370, 274, 192, 124,
							70,	 31,  10,  0,	10,	 31,  70,  123, 192, 274,
							370, 479, 599, 731, 873, 1023,1182,1347,1517,1691,
							1868,2047};


inDAC dac;

//
void inDAC::initChan4()
{
    GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef  DAC_InitStructure;	
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
			
	DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_4095; //DAC_TriangleAmplitude_1023;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);

	DAC_Cmd(DAC_Channel_1, ENABLE);
}

//
void inDAC::initChan5()
{
    
}

//
void inDAC::initTim4()
{

}

void inDAC::initDMA4()
{
    DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
	DMA_InitStructure.DMA_Channel = DMA_Channel_7;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(DAC->DHR12R1);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)sinus;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = 72;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	
	DMA_Init(DMA1_Stream5, &DMA_InitStructure);
	
	DAC_DMACmd(DAC_Channel_1, ENABLE);
	
	DMA_Cmd(DMA1_Stream5, ENABLE);
}

