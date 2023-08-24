#ifndef __ETHERNET_BRIDGE_H
#define __ETHERNET_BRIDGE_H

//#include "terminal_medsens.h"
#include "thread.h"
#include "net_types.h"
#include "netconf.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"


typedef enum
{
    statedhcp_Start = 1,
    statedhcp_WaitAddr = 2,
    statedhcp_AddrAssigned = 3,
    statedhcp_Timeout = 4,
    statedhcp_LinkDown = 5,
  
} stateDHCP;

typedef enum
{
    ethstate_NotInit = 0,
    ethstate_Configure,
    ethstate_WaitAddr,
    ethstate_Running,
    ethstate_LinkDown,
    ethstate_Error,
  
} EthState;

typedef struct {
  
    bool exist;
    bool isDHCP;  // 0 - DHCP отключен, 1 - включен
    char ip[15];     
    char gate[15];
    char mask[15];
    char mac[18];
  
} EthernetOptions;



class EthernetBridge : public Thread {
public:
  
    EthernetBridge() { 
      m_state = ethstate_NotInit; 
      m_dhcpTryCounter = 0; };
    
    virtual void run();
    bool configure();
    bool initNetif();
    bool isLinkUp();
    
    EthState getState();
    
    void setStaticIP();
    void setDinamicIP();

    void setOptions(EthernetOptions* opt);
    
private :    
  
    void macToChar();
    
//private :
public:
    
    netif            m_ethNetif;
    EthState         m_state;
    
public :

    EthernetOptions* m_options;
    
    uint8_t   m_mac[6];
    ip_addr_t m_ip;
    ip_addr_t m_mask;
    ip_addr_t m_gate;
    
private :
        
    uint8_t   m_dhcpTryCounter; 
    stateDHCP m_dhpcState;
};


extern EthernetBridge ethernetBridge;

void vEthernetBridge(void* params);    
    
void vDHCP(void* params);

#endif  // __ETHERNET_BRIDGE_H

