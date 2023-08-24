#include "proto_tcp.h"
#include "concentrator.h"
#include "net_bridge.h"
#include "manager_tcp.h"
#include "pcm1808.h"
#include "commands.h"
#include "tm_stm32f4_id.h"

#include <stdio.h>
#include <string.h>

MProtoTCP pTCP;


#define TEST_BUF_SIZE   250
uint32_t test[TEST_BUF_SIZE];


//
void MProtoTCP::configure()
{
    uint32_t id = 0;
    
    memset(m_id, 0, 97);
    
    id = TM_ID_GetUnique32(0);
    sprintf(m_id, "%u", id);
    id = TM_ID_GetUnique32(1);
    sprintf(m_id + strlen(m_id), "%u", id);
    id = TM_ID_GetUnique32(2);
    sprintf(m_id + strlen(m_id), "%u", id);
    
    memcpy(&m_options, &concentrator.m_options, sizeof(ConcentratorOptions));
  
    for (uint32_t i = 0; i < TEST_BUF_SIZE; i++) {
        test[i] = RNG_GetRandomNumber();
    }
}

// -----------------------------------------------------------------------------
// State machine этапа иниициализации
// Ожидание получения сетевых параметров
bool MProtoTCP::initState()
{
    switch (m_initState)
    {
    // Получение сетевых параметров
    case istate_GetNetParams :
        //printf("Waiting net params\r\n");
        if (gettingNetParams())
            m_initState = istate_Connection;
    break;
    // Установка TCP соединения
    case istate_Connection :
        if (connection()) {
            printf("Connection OK\r\n");
            m_initState = istate_GetTime;
            //managerTCP.closeCon();
            //m_initState = istate_Nop;
        }
        else {
            printf("Connection failed\r\n");
            vTaskDelay(5000);
        }
    break;
    // Запрос времени
    case istate_GetTime :
        //vTaskDelay(5000);
        if (getTime()) {
            m_initState = istate_Status;
        }
        else
            vTaskDelay(5000);
    break;
    // Отправка STATUS_OK
    case istate_Status :
      vTaskDelay(2000); // Необходимая задержка по протоколу
        if (sendStatus("qwerty")) {
            m_initState = istate_Connection;
            return true;
        }
    break;
    
    case istate_Nop :
        vTaskDelay(1000);
    break;
    
    
    default :
    break;
    }
        
    return false;
}

// Ожидание получения сетевых параметров
bool MProtoTCP::gettingNetParams()
{
    return netBridge.getEthState();
}

// Установка TCP соединения
bool MProtoTCP::connection()
{
    if (managerTCP.connection(&m_options.addr) == conres_ConSuccess)
        return true;
        
    return false;
}

// Запрос и установка системного времени
bool MProtoTCP::getTime()
{
    char str[20] = {};    
    char* ptr;
    uint8_t size;
    uint32_t time;
    uint32_t dTime;
    
    memset(sendBuf, 0, SEND_BUF_SIZE);
    memset(recvBuf, 0, RECV_BUF_SIZE);
    
    // Собираем запрос MSG_GET_TIME
    strcpy(sendBuf, "{\"msgtype\":\"MSG_GET_TIME\",\"sendid\":\"");
    strcat(sendBuf, concentrator.m_options.sendid);
    strcat(sendBuf, "\",\"destid\":\"");
    strcat(sendBuf, concentrator.m_options.destid); 
    strcat(sendBuf, "\",\"ttl\":0,\"time\":0}");
        
    if (!managerTCP.timeRequest(sendBuf, recvBuf, (uint16_t)RECV_BUF_SIZE, &dTime))
        return false;
    
    if (!jParse(recvBuf))
        return false;
    
    ptr = jGetParam(recvBuf, "time", &size);
        
    if (ptr == NULL)
        return false;

    // TEST
    //dTime = 3541;
    
    // Вычисляем сдвиг
    m_dTimeSec = dTime/1000;
    m_dTimeUSec = dTime - (m_dTimeSec*1000);
    
    // Время получили, устанавливаем значение RTC
    memcpy(str, ptr, size);
    time = strtol(str, NULL, 10);
    TM_RTC_SetDataTimeUnix(time);
   
    return true;
    
/*    
    if (!managerTCP.request(sendBuf, recvBuf, (uint16_t)RECV_BUF_SIZE)) 
        return false;
      
    if (!jParse(recvBuf))
        return false;
    
    ptr = jGetParam(recvBuf, "time", &size);
        
    if (ptr == NULL)
        return false;

    // Время получили, устанавливаем значение RTC
    memcpy(str, ptr, size);
    time = strtol(str, NULL, 10);
    TM_RTC_SetDataTimeUnix(time);
    
    return true;
*/    
}

//
bool MProtoTCP::sendStatus(char* str)
{
    memset(sendBuf, 0, SEND_BUF_SIZE);
    memset(recvBuf, 0, RECV_BUF_SIZE);
        
    // Собираем запрос MSG_STATUS
    strcpy(sendBuf, "{\"msgtype\":\"MSG_STATUS\",\"sendid\":\"");
    strcat(sendBuf, concentrator.m_options.sendid);
    strcat(sendBuf, "\",\"sensid\":\"");
    strcat(sendBuf, concentrator.m_options.sendid);
    strcat(sendBuf, "\",\"destid\":\"");
    strcat(sendBuf, concentrator.m_options.destid);
    
    //strcat(sendBuf, concentrator.m_options.sendid);
    
    strcat(sendBuf, "\",\"time\":");
    addCurrentTime(sendBuf);
    
    strcat(sendBuf, ",\"info\":\"Sensor ");
    strcat(sendBuf, concentrator.m_options.sendid);
    strcat(sendBuf, " online\"");
    
    //strcat(sendBuf, ",\"status\":\"SENSOR_ONLINE\"}");
    strcat(sendBuf, ",\"status\":\"STATUS_OK\"}");
        
    
    printf(sendBuf);
    printf("\r\n");
    
    managerTCP.request(sendBuf, recvBuf, (uint16_t)RECV_BUF_SIZE);
    
    return true;
    
    //return (managerTCP.request(sendBuf, recvBuf, (uint16_t)RECV_BUF_SIZE));
    /*
    if (!managerTCP.request(sendBuf, recvBuf, (uint16_t)RECV_BUF_SIZE))
        return false;
    ptr = jGetParam(recvBuf, "time", &size);
    */
}

//
bool MProtoTCP::sendData()
{
    char str[20];
    bool ret;
    
    uint32_t oldTicks;
    uint32_t periodTicks;
    
    while (!adcPCM.isDataReady()) {}
    adcPCM.clearDataReady();
    
    //printf("SendData\r\n");

    oldTicks = xTaskGetTickCount();
    
    memset(sendBuf, 0, SEND_BUF_SIZE);
    memset(recvBuf, 0, RECV_BUF_SIZE);

    //memcpy(sendBuf, rawDataStr, strlen(rawDataStr));
    
    // Собираем запрос MSG_DATA
    strcpy(sendBuf, "{\"msgtype\":\"MSG_DATA\",\"sendid\":\"");
    strcat(sendBuf, concentrator.m_options.sendid);
    strcat(sendBuf, "\",\"destid\":\"");
    strcat(sendBuf, concentrator.m_options.destid);
    strcat(sendBuf, "\",\"format\":\"RAWDUMP");
    strcat(sendBuf, "\",\"time\":");
    addCurrentTime(sendBuf);
    strcat(sendBuf, ",\"ttl\":1000000,\"opcode\":\"RAW_DATA\",\"start\":");
    addTime(sendBuf, &m_cleanData.timeStart);
    strcat(sendBuf, ",\"stop\":");
    addTime(sendBuf, &m_cleanData.timeEnd);
    strcat(sendBuf, ",\"data\":[");       
    
    for (uint32_t i = 0; i < (TEST_BUF_SIZE - 1); i++) {
        //memset(str, 0, 20);
        //sprintf(str, "%i,", test[i]);
        //strcat(sendBuf, str);
        //sprintf(sendBuf + strlen(sendBuf), "%i,", test[i]);
        sprintf(sendBuf + strlen(sendBuf), "%u,", test[i]);
    }
    // Последний элемент
    memset(str, 0, 20);
    sprintf(str, "%u", test[TEST_BUF_SIZE - 1]);
    strcat(sendBuf, str);
    
    strcat(sendBuf, "]}");
    
    //printf(sendBuf);
    //printf("\r\n");
    
    periodTicks = xTaskGetTickCount() - oldTicks;
    printf("Time to create package: %d\r\n", periodTicks);
    
    
    oldTicks = xTaskGetTickCount();
    ret = managerTCP.request(sendBuf, recvBuf, (uint16_t)RECV_BUF_SIZE);
    periodTicks = xTaskGetTickCount() - oldTicks;
    printf("Time to send package: %d\r\n", periodTicks);
    
    return ret;    





#if 0    
    memset(sendBuf, 0, SEND_BUF_SIZE);
    memset(recvBuf, 0, RECV_BUF_SIZE);

    //memcpy(sendBuf, rawDataStr, strlen(rawDataStr));
    
    // Собираем запрос MSG_DATA
    strcpy(sendBuf, "{\"msgtype\":\"MSG_DATA\",\"sendid\":\"");
    strcat(sendBuf, concentrator.m_options.sendid);
    strcat(sendBuf, "\",\"destid\":\"");
    strcat(sendBuf, concentrator.m_options.destid);
    strcat(sendBuf, "\",\"time\":");
    addCurrentTime(sendBuf);
    strcat(sendBuf, ",\"ttl\":1000000,\"opcode\":\"RAW_DATA\",\"start\":122.456,");
    strcat(sendBuf, "\"stop\":123.456,\"rawdump\":[");       

    for (uint32_t i = 0; i < (TEST_BUF_SIZE - 1); i++) {
        //memset(str, 0, 20);
        //sprintf(str, "%i,", test[i]);
        //strcat(sendBuf, str);
        sprintf(sendBuf + strlen(sendBuf), "%i,", test[i]);
    }
    // Последний элемент
    memset(str, 0, 20);
    sprintf(str, "%i", test[TEST_BUF_SIZE - 1]);
    strcat(sendBuf, str);
    
    strcat(sendBuf, "]}");

    //printf(sendBuf);
    //printf("\r\n");
    
    periodTicks = xTaskGetTickCount() - oldTicks;
    printf("Time to create package: %d\r\n", periodTicks);
    
    
    oldTicks = xTaskGetTickCount();
    ret = managerTCP.request(sendBuf, recvBuf, (uint16_t)RECV_BUF_SIZE);
    periodTicks = xTaskGetTickCount() - oldTicks;
    printf("Time to send package: %d\r\n", periodTicks);
    
    return ret;    
#endif    
}


// -----------------------------------------------------------------------------
//                          JSON парсер

bool MProtoTCP::jParse(char* str)
{
    jsmn_init(&m_jParser);
    m_jObjCounter = jsmn_parse(&m_jParser, str, strlen(str), m_jToken, JSON_TOKEN_CNT);
    if (m_jObjCounter < 1 || m_jToken[0].type != JSMN_OBJECT)
        return false;
    
    return true;
}

char* MProtoTCP::jGetParam(char* jstr, char* param, uint8_t* size)
{
    for (int i = 0; i < m_jObjCounter; i++)
    {  
        if (jsoneq(jstr, &m_jToken[i], param) == 0) 
        {
            *size = m_jToken[i+1].end - m_jToken[i+1].start;
            return (jstr + m_jToken[i+1].start);
	}
    }
    return NULL;
}

// -----------------------------------------------------------------------------
//                      Вспомогательные функции

// Добавляет текущее время к строке
void MProtoTCP::addCurrentTime(char* str)
{
    TM_RTC_t currentTime;
    char strTime[20] = {0};
    uint32_t unitxTime;
    uint16_t subsecond;
    uint16_t sec = 0; 
    uint16_t uSec = 0; 
    
    
    TM_RTC_GetDateTime(&currentTime, TM_RTC_Format_BIN);
    unitxTime = TM_RTC_GetUnixTimeStamp(&currentTime);
    subsecond = (uint16_t)(1000 - (float)currentTime.subseconds * 1000.0/1023.0);    
    
    if (m_dTimeSec)
        unitxTime += m_dTimeSec;
    
    uSec = subsecond + m_dTimeUSec;
    sec = uSec/1000;
    if (sec != 0) {
        unitxTime += uSec;
        uSec = uSec - sec*1000;
    }
          
    subsecond += uSec;
    
    sprintf(strTime, "%d.", unitxTime);
    sprintf(strTime + strlen(strTime), "%d", subsecond);
        
    strcat((str + strlen(str)), strTime);
}

//
void MProtoTCP::addTime(char* str, TM_RTC_UnixTime_t* time)
{
    char strTime[20] = {0};
    uint16_t sec = 0; 
    uint16_t uSec = 0; 
        
    uSec = time->subsec + m_dTimeUSec;
    sec = uSec/1000;
    if (sec != 0) {
        uSec = uSec - sec*1000;
    }    
    
    sprintf(strTime, "%d.", time->sec + sec);
    sprintf(strTime + strlen(strTime), "%d", time->subsec + uSec);
        
    strcat((str + strlen(str)), strTime);
}


// -----------------------------------------------------------------------------
//                      Стейты главного цикла протокола

//
void MProtoTCP::processing()
{
    switch (m_protoState)
    {
    case pstate_Init :
        if (initState()) {
            m_protoState = pstate_Data;
        }
    break;
    
    case pstate_Data :
        //vTaskDelay(1000);
        sendData();
    break;
    
    default :
    break;
    }
    //printf("Server protocol over TCP\r\n");
    vTaskDelay(10);
}

//
void vMProtoTCP(void* params)
{
    for (;;) {
        pTCP.processing();
    }
}