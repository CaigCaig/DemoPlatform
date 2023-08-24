#ifndef __PCM1808_H
#define __PCM1808_H

#include "rtc.h"
#include "Thread.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#define RAW_DATA_SIZE   16    // Размер массива сырых данных на 2 канала
                              // 16/2 = 8 точек - это соответствует частоте 
                              // цикла набора 1000 Гц  при частоте семплирования ADC 8кГц

#define CLEAN_DATA_SIZE 1000  // Размер массива чистых данных на 2 канала 
                              // Получаем по 500 отсчетов с каждого канала
                              // Заполняется 2 раза в секунду при фильтрации 1000 Гц
//
/*
typedef struct
{
    TM_RTC_t start;
    TM_RTC_t end;

} TimeStartEnd;
*/

//
typedef struct
{
    int32_t data[CLEAN_DATA_SIZE];
    //TimeStartEnd time;
    TM_RTC_UnixTime_t timeStart;
    TM_RTC_UnixTime_t timeEnd;
    uint32_t index;
        
} CleanData;


//
typedef struct
{
    int32_t data[2][RAW_DATA_SIZE];
    uint8_t bufIndex;       // определяет в какой массив пишем
    
} RawData;


class pcm1808 {
  
public :
    pcm1808() {}
    
    void configure();
    
private :
    void dmaInit();
    
public :
    void filter(int32_t* src, int32_t* dest, uint32_t size);
    
// Параметры данных и сами данные
public :
    CleanData m_clean;
    RawData   m_raw;
    
    bool      m_dataReadyFlag;
    
    TM_RTC_UnixTime_t  m_time;
    
    bool isDataReady();
    void clearDataReady();
    
public :
    SemaphoreHandle_t m_semph;
    SemaphoreHandle_t m_mutex;
    void takeDataMutex();
    void giveDataMutex();
};

//
void splittingForDAC(uint16_t* leftChan, uint16_t* rightChan, uint32_t* src, uint32_t size);

//
void twosComplimentToUnsigned(uint32_t* dest, int32_t* src, uint32_t size);


//
void vCopyAdcData(void* params);

extern pcm1808 adcPCM;

#endif  // __PCM1808_H

