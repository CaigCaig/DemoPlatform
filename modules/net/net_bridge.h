#ifndef __NET_BRIDGE_H
#define __NET_BRIDGE_H

#include "FreeRTOS.h"
#include "task.h"
#include "ip_stack.h"
#include "ethernet_bridge.h"
#include "net_types.h"
#include "thread.h"
//#include "terminal_medsens.h"

//#include "lwip/sockets.h"

typedef enum
{
    netuser_TelemetryServer = 0,
    netuser_Controller,
    netuser_System,
} 
netUser;

typedef struct NetOptions
{
    EthernetOptions eth;
    bool gsmExist;
    bool wifiExist;
    
} NetOptions;




class NetBridge : public Thread {
public:
    NetBridge();
    
    virtual void run();
    bool configure();
    
    void ethTest();

    bool getEthState(); //{return m_ethState;}
    bool getGsmState() {return m_gsmState;}
    bool getWifiState() {return m_wifiState;}
    
    void printNetParams();
    
public :

    NetOptions  m_options;
    TaskHandle_t    m_ethTaskHandle;
    
private :
  
    IP_Stack*       m_ipStack;
    
    EthernetBridge* m_ethernet;
    bool            m_ethTaskEnabled;
    
private :
  
    bool m_ethState;
    bool m_gsmState;
    bool m_wifiState;
    
//private :
    
//    DebugMessage     m_dbgMsg;
    
};



extern NetBridge netBridge;

void vNetBridge(void *params);

#endif  // __SERVER_BRIDGE_H

