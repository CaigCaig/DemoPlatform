#ifndef __PROTO_TCP_H
#define __PROTO_TCP_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "concentrator.h"
#include "jsmn.h"
#include "pcm1808.h"
#include "rtc.h"

#define SEND_BUF_SIZE   12300
#define RECV_BUF_SIZE   400

#define JSON_TOKEN_CNT  20



// Protocol states
typedef enum
{
    pstate_Init = 0, // Инициализация
    pstate_Data,
}
pState;

// Init stage states
typedef enum
{
    istate_GetNetParams = 0, // Ожидание получения сетевых параметров
    istate_Connection,
    istate_GetTime,
    istate_Status,
    istate_Nop,
}
iState;

// Protocol errors
typedef enum
{
    mperr_NetworkUnreachable = 101,
    
}
mpErr;


class MProtoTCP {
  
public :
    MProtoTCP() {m_protoState = pstate_Init; m_initState = istate_GetNetParams;}

    void configure();
    void processing();
  
private :
    bool initState();
        
private :
    bool gettingNetParams();
    bool connection();
    bool getTime();
    bool sendStatus(char* str);
    bool sendData();
    
private :
    ConcentratorOptions m_options;
    int m_socket;
    
    char sendBuf[SEND_BUF_SIZE];
    char recvBuf[RECV_BUF_SIZE];

private :
    uint16_t m_dTimeSec;
    uint16_t m_dTimeUSec;
    
// Данные с ADC    
public :
    CleanData   m_cleanData;
    char        m_id[97];

// Для JSON парсера    
private :    
    jsmn_parser m_jParser;
	jsmntok_t   m_jToken[JSON_TOKEN_CNT]; // We expect no more than 128 tokens 
    int         m_jObjCounter;
    
    bool jParse(char* str);
    char* jGetParam(char* jstr, char* param, uint8_t* size);
      
// Вспомогательные функции    
private :
    void addCurrentTime(char* str);
    void addTime(char* str, TM_RTC_UnixTime_t* time);
    
private :
    pState m_protoState; // основные стейты протокола
    iState m_initState;
    
};

void vMProtoTCP(void* params);



extern MProtoTCP pTCP;

#endif // __PROTO_TCP_H
