#ifndef __PROTO_MQTT_H
#define __PROTO_MQTT_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "mqtt.h"

#include "concentrator.h"
#include "jsmn.h"
#include "pcm1808.h"
#include "rtc.h"

// Connect status
typedef enum
{
    constate_idle = 0,
    constate_wait, // 
    constate_ok,
    constate_error,
}
conState;


// Protocol states
typedef enum
{
    pmqttstate_Init = 0, // Инициализация
    pmqttstate_Data,
}
pmqttState;

// Init stage states
typedef enum
{
    imqttstate_GetNetParams = 0, // Ожидание получения сетевых параметров
    imqttstate_Connection,
    imqttstate_Nop,
}
imqttState;

// Publish stage states
typedef enum
{
    pubstate_Publish = 0,
    pubstate_Wait,
}
pubState;

// publish result
typedef enum
{
    pubres_FooErr = 0,  // пока все остальные ошибки
    pubres_Disconnect,  // нет соединения
    pubres_SendOk,      // отправка состоялось. Ожидание CallBack
}
pubres;

class MProtoMQTT {
  
public :
    MProtoMQTT() {m_protoState = pmqttstate_Init; m_initState = imqttstate_GetNetParams;
                  m_pubState = pubstate_Publish;
                  m_conResult = constate_idle;}

    void configure();
    void processing();
  
private :
    bool initState();
    bool publishState();
    
private :
    bool gettingNetParams();
    void connection();
    pubres publishData();    
    
public :    
    conState m_conResult;
    
private :
    ConcentratorOptions m_options;    
    
private :
    pmqttState m_protoState; // основные стейты протокола
    imqttState m_initState;
    pubState   m_pubState;
    
    mqtt_client_t m_client;
};

void vMProtoMQTT(void* params);



extern MProtoMQTT pMQTT;

#endif // __PROTO_MQTT_H
