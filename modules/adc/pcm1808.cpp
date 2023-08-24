#include "stm32f4xx.h"
#include "pcm1808.h"
#include "stm32_ub_dac_dma.h"
#include "proto_tcp.h"

#include <string.h>
#include <stdio.h>

#define SWAP24(w) ((w) << 8) | ((w) >> 24)
#define SWAP32(w) (((w) & 0xFFFF) << 16) | (((w) & 0xFFFF0000) >> 16)

#define PCM_DMA_BUF_SIZE    32

int32_t rawDataBuf[2][PCM_DMA_BUF_SIZE]; // Буфер для сырых данных с ADC
int32_t testBuf[32];
//uint32_t leftChannel[PCM_DMA_BUF_SIZE/2];
//uint32_t rightChannel[PCM_DMA_BUF_SIZE/2];

// Для обработки и тестирования
uint32_t unsignedBuf[PCM_DMA_BUF_SIZE];
uint16_t leftChanForDac[PCM_DMA_BUF_SIZE/2];
uint16_t rightChanForDac[PCM_DMA_BUF_SIZE/2];

uint16_t leftChanForDacTmp[PCM_DMA_BUF_SIZE/2];
uint16_t rightChanForDacTmp[PCM_DMA_BUF_SIZE/2];

// -----------------------------------------------------------------------------
// Для тестирования фильтрации
uint32_t uBuf[CLEAN_DATA_SIZE] = {0};

uint16_t leftChanForDacF[CLEAN_DATA_SIZE/2];
uint16_t rightChanForDacF[CLEAN_DATA_SIZE/2];

uint16_t leftChanForDacTmpF[CLEAN_DATA_SIZE/2];
uint16_t rightChanForDacTmpF[CLEAN_DATA_SIZE/2];

// -----------------------------------------------------------------------------

uint32_t cpBuf[2][32];
uint32_t buf[32];
float buf2[32];
uint8_t BufferIndex = 0;

pcm1808 adcPCM;

//
void pcm1808::configure()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    I2S_InitTypeDef  I2S_InitType;
    NVIC_InitTypeDef NVIC_InitStructure;
  
    m_semph = xSemaphoreCreateBinary();
    m_mutex = xSemaphoreCreateMutex();
    m_dataReadyFlag = false;
    
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOI, ENABLE);
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOF, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    RCC_PLLI2SCmd(ENABLE);  // Master clock
             
    // Пины для конфигурации PCM1808
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
    
    GPIO_SetBits(GPIOF, GPIO_Pin_11);   // Формат протокола Left-justified 24 bit
    GPIO_ResetBits(GPIOF, GPIO_Pin_12); // Slave mode
    GPIO_ResetBits(GPIOF, GPIO_Pin_13); // Slave mode
       
    // Пины I2S
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3;
    GPIO_Init(GPIOI, &GPIO_InitStructure);
        
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_SPI2); // Master clock
    GPIO_PinAFConfig(GPIOI, GPIO_PinSource0, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOI, GPIO_PinSource1, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOI, GPIO_PinSource3, GPIO_AF_SPI2);
    
    dmaInit();
    
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    I2S_InitType.I2S_AudioFreq  = I2S_AudioFreq_8k;
    I2S_InitType.I2S_MCLKOutput = I2S_MCLKOutput_Enable;
    I2S_InitType.I2S_Mode       = I2S_Mode_MasterRx;
    I2S_InitType.I2S_DataFormat = I2S_DataFormat_24b;
    I2S_InitType.I2S_Standard   = I2S_Standard_MSB;
    I2S_InitType.I2S_CPOL       = I2S_CPOL_High;
    I2S_Init(SPI2, &I2S_InitType);
        
    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);
    I2S_Cmd(SPI2, ENABLE);
}

//
void pcm1808::dmaInit()
{
    DMA_InitTypeDef DMA_InitStructure;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    
    DMA_DoubleBufferModeConfig(DMA1_Stream3, (uint32_t)m_raw.data[1], DMA_Memory_1);
    DMA_DoubleBufferModeCmd(DMA1_Stream3, ENABLE);
    
    DMA_InitStructure.DMA_Channel = DMA_Channel_0;
    DMA_InitStructure.DMA_PeripheralBaseAddr = ((uint32_t)&SPI2->DR);
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)m_raw.data[0];
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = 2*RAW_DATA_SIZE; // регистр SPI 16 бит, а нужно 32
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream3,&DMA_InitStructure);
    DMA_Cmd(DMA1_Stream3, ENABLE);
    
    DMA_ITConfig(DMA1_Stream3, DMA_IT_TC, ENABLE);
    
    RTC_GetSecAndSubsec(&adcPCM.m_time.sec, &adcPCM.m_time.subsec);
}

// 
void pcm1808::filter(int32_t* src, int32_t* dest, uint32_t size)
{
    int32_t leftChan = 0;
    int32_t rightChan = 0;

    for (uint32_t i = 0; i < (size); i++) 
    {
        leftChan += *src*0.1;
        src++;
        rightChan += *src*0.1;
        src++;
    }
    
    *dest = leftChan;    
    dest++;
    *dest = rightChan;
}

//
bool pcm1808::isDataReady()
{
    return m_dataReadyFlag;
}

//
void pcm1808::clearDataReady()
{
    m_dataReadyFlag = false;
}

//
void pcm1808::takeDataMutex()
{
    xSemaphoreTake(m_mutex, portMAX_DELAY);
}

void pcm1808::giveDataMutex()
{
    xSemaphoreGive(m_mutex);
}


extern "C" {
  
void DMA1_Stream3_IRQHandler()
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    
    if (DMA_GetITStatus(DMA1_Stream3, DMA_IT_TCIF3))
    {
        DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_TCIF3);

        if (DMA_GetCurrentMemoryTarget(DMA1_Stream3)) 
            adcPCM.m_raw.bufIndex = 0;
        else 
            adcPCM.m_raw.bufIndex = 1; 
    }
    
    // Перестановка байтов
    for (uint32_t i = 0; i < RAW_DATA_SIZE; i++) 
    {    
        adcPCM.m_raw.data[adcPCM.m_raw.bufIndex][i] = SWAP32(adcPCM.m_raw.data[adcPCM.m_raw.bufIndex][i]);
    }
    
    // Фильтрация 
    adcPCM.filter(&adcPCM.m_raw.data[adcPCM.m_raw.bufIndex][0], 
                  &adcPCM.m_clean.data[adcPCM.m_clean.index], 
                  RAW_DATA_SIZE/2);
    
    adcPCM.m_clean.index += 2;
        
    // Массив "чистых" данных заполнен, нужно отправлять
    if (adcPCM.m_clean.index == CLEAN_DATA_SIZE)
    {
        twosComplimentToUnsigned(uBuf, &adcPCM.m_clean.data[0], CLEAN_DATA_SIZE);
        splittingForDAC(leftChanForDacF, rightChanForDacF, uBuf, CLEAN_DATA_SIZE);
        DAC_ResetDMA();
        
        adcPCM.m_clean.index = 0;
        
        // Время
        adcPCM.m_clean.timeStart.sec = adcPCM.m_time.sec;
        adcPCM.m_clean.timeStart.subsec = adcPCM.m_time.subsec;
        RTC_GetSecAndSubsec(&adcPCM.m_clean.timeEnd.sec, &adcPCM.m_clean.timeEnd.subsec);
        RTC_GetSecAndSubsec(&adcPCM.m_time.sec, &adcPCM.m_time.subsec);
        
        xSemaphoreGiveFromISR(adcPCM.m_semph, &xHigherPriorityTaskWoken);
    }
}  

}

//
void splittingForDAC(uint16_t* leftChan, uint16_t* rightChan, uint32_t* src, uint32_t size)
{
    for (uint32_t i = 0; i < (size/2); i++) {
        *leftChan = *src >> 20;
        src++;
        *rightChan = *src >> 20;
        src++;
        leftChan++;
        rightChan++;
    }
}

//
void twosComplimentToUnsigned(uint32_t* dest, int32_t* src, uint32_t size)
{
    int32_t  iTemp;
    
    for (uint32_t i = 0; i < size; i++) {
        iTemp = *src;
        *dest = iTemp + 0x7FFFFFFF;
        dest++; 
        src++; 
    }
}


//
void vCopyAdcData(void* params)
{
    for (;;)
    {
        xSemaphoreTake(adcPCM.m_semph, portMAX_DELAY);
        
        // ---------------------------------------------------------------------
        adcPCM.takeDataMutex();
        
        // Копируем свежие отфильтрованные данные для формирования пакета на отправку
        memcpy(pTCP.m_cleanData.data, adcPCM.m_clean.data, CLEAN_DATA_SIZE);
        // Время
        pTCP.m_cleanData.timeStart.sec = adcPCM.m_clean.timeStart.sec;
        pTCP.m_cleanData.timeStart.subsec = adcPCM.m_clean.timeStart.subsec;
        pTCP.m_cleanData.timeEnd.sec = adcPCM.m_clean.timeEnd.sec;
        pTCP.m_cleanData.timeEnd.subsec = adcPCM.m_clean.timeEnd.subsec;
          
        adcPCM.m_dataReadyFlag = true;
        
        adcPCM.giveDataMutex();
        // ---------------------------------------------------------------------
    }
}
