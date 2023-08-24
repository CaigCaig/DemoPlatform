#include "proto_mqtt.h"
#include "net_bridge.h"
#include "manager_tcp.h"

#include <string.h>
#include <stdio.h>

MProtoMQTT pMQTT;


static void MQTT_ConnectionCb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);
static void MQTT_PublishCb(void *arg, err_t result);


//
void MProtoMQTT::configure()
{
    memcpy(&m_options, &concentrator.m_options, sizeof(ConcentratorOptions));
/*  
    for (uint32_t i = 0; i < TEST_BUF_SIZE; i++) {
        test[i] = RNG_GetRandomNumber();
    }
*/  
}

// -----------------------------------------------------------------------------
// State machine этапа иниициализации
// Ожидание получения сетевых параметров
bool MProtoMQTT::initState()
{
    switch (m_initState)
    {
    // Получение сетевых параметров
    case imqttstate_GetNetParams :
        //printf("Waiting net params\r\n");
        if (gettingNetParams())
            m_initState = imqttstate_Connection;
    break;
    // Установка TCP соединения
    case imqttstate_Connection :
        if (m_conResult == constate_idle) {
            m_conResult = constate_wait;
            connection();
        }
        if (m_conResult == constate_wait) {
            printf("Waiting connection\r\n");
            vTaskDelay(100);
        }  
        if (m_conResult == constate_ok) {
            return true;
        }
        if (m_conResult == constate_error) {
            m_initState = imqttstate_Connection;
        } 
    break;
    
    case imqttstate_Nop :
        vTaskDelay(1000);
    break;
    
    
    default :
    break;
    }
        
    return false;
}

bool MProtoMQTT::publishState()
{
/*  
    switch (m_pubState)
    {
    case 
    }
*/    
    return false;
}

// Ожидание получения сетевых параметров
bool MProtoMQTT::gettingNetParams()
{
    return netBridge.getEthState();
}

// Установка TCP соединения с брокером
void MProtoMQTT::connection()
{
    struct mqtt_connect_client_info_t ci;
    err_t err;
    ip_addr_t adr; 
    adr.addr = m_options.addr.sin_addr.s_addr;
    
    memset(&ci, 0, sizeof(ci));
    ci.client_id = "lwip_test";
  
    // Initiate client and connect to server, if this fails immediately an error code is returned
    // otherwise mqtt_connection_cb will be called with connection result after attempting 
    // to establish a connection with the server. 
    // For now MQTT version 3.1.1 is always used 
    err = mqtt_client_connect(&m_client, &adr, MQTT_PORT, &MQTT_ConnectionCb, 0, &ci);
  
    // For now just print the result code if something goes wrong 
    if(err != ERR_OK) {
        printf("mqtt_connect return %d\n", err);
    }
}

// Отправка данных
pubres MProtoMQTT::publishData()
{
    const char *pub_payload= "PubSubHubLubJub";
    err_t err;
    u8_t qos = 2;    // 0 1 or 2, see MQTT specification 
    u8_t retain = 0; // No don't retain such crappy payload... 
    void *arg = 0;
    
    err = mqtt_publish(&m_client, "pub_topic", pub_payload, strlen(pub_payload), qos, retain, MQTT_PublishCb, arg);
    
    if (err == 0xF5)
        return pubres_Disconnect;
    
    if(err != ERR_OK) {
        return pubres_FooErr;
        //printf("Publish err: %d\n", err);
    }
    
    return pubres_SendOk;
}


//
void MProtoMQTT::processing()
{
    pubres res;
    
    switch (m_protoState)
    {
    case pmqttstate_Init :
        if (initState()) {
            m_protoState = pmqttstate_Data;
        }
    break;
    
    case pmqttstate_Data :
        publishState();
        //res = publishData();
    break;
    
    default :
    break;
    }
    
    vTaskDelay(10);
}



//
void MQTT_ConnectionCb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    // Соединение не установлено. Добавить логику обработки
    if (status != MQTT_CONNECT_ACCEPTED) {
        printf("MQTT connection failed\r\n");
        pMQTT.m_conResult = constate_idle;
    }
    
    // Соединение установлено. 
    printf("MQTT connection OK\r\n");
    pMQTT.m_conResult = constate_ok;
}

//
void MQTT_PublishCb(void *arg, err_t result)
{
    if(result != ERR_OK) {
        printf("Publish result: %d\n", result);
    }
}


//
void vMProtoMQTT(void* params)
{
    for (;;) {
        pMQTT.processing();
    }
}

