#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"

#include "ethernet_bridge.h"
#include "ethernetif.h"
#include "mcp3422.h"
#include "24AA025E48.h"

#include "stm32f4x7_eth.h"
#include "stm32f4x7_eth_conf.h"
#include "stm32f4x7_eth_bsp.h"

#include <string.h>

// Global HAL Ethernet handle
//extern ETH_HandleTypeDef EthHandle;

EthernetBridge ethernetBridge;

extern "C" {
uint8_t* getMac()
{
    return ethernetBridge.m_mac;
}
}

// Настройка железа и стека
bool EthernetBridge::configure()
{
    extern uint8_t MAC_24AA025E48[6];
    memset(m_mac, 0, sizeof(m_mac));
    for (uint8_t i = 0; i < 6; i++)
    {
        m_mac[i] = MAC_24AA025E48[i];
    }
    
//    macToChar();
    return true;
}

void EthernetBridge::run()
{
    extern eeprom_t eeprom_data;
    extern uint8_t Net_Update;
    switch (m_state) {
    
    // Инициализация структуры netif
    case ethstate_NotInit :
        initNetif();
        m_state = ethstate_Configure;
    break;
    
    //
    case ethstate_Configure :
      
        if (!isLinkUp()) {
            m_state = ethstate_LinkDown;
            return;
        }
//        if (m_options->isDHCP) {
        if (!eeprom_data.eeprom_dhcp) {
            m_state = ethstate_WaitAddr;
            m_dhpcState = statedhcp_Start;
        }
        else {
            setStaticIP();
            m_state = ethstate_Running;
        }
    break;
    
    // Установка сетевых параметров
    case ethstate_WaitAddr :
        setDinamicIP();
        if (m_dhpcState == statedhcp_AddrAssigned)
            m_state = ethstate_Running;
        else if (m_dhpcState == statedhcp_Timeout) {
            vTaskDelay(1000);
//            vTaskDelay(3000);
            m_dhpcState = statedhcp_Start;
        }   
    break;
    
    //
    case ethstate_Running :
        vTaskDelay(100);
        if ((!isLinkUp() || (Net_Update)))
            m_state = ethstate_LinkDown;
        //printf("ehternet runing\r\n");
    break;
    
    //
    case ethstate_LinkDown :
      
        if (isLinkUp())
            m_state = ethstate_Configure;
        else
            vTaskDelay(100);
    break;  
    
    // Авария
    case ethstate_Error :
        vTaskDelay(100);
        //printf("ehternet error\r\n");
    break;
    
    }
      
}

bool EthernetBridge::initNetif()
{
    ip4_addr_t ip;
    ip4_addr_t gate;	
    ip4_addr_t mask;
    
    ip.addr   = 0;
    gate.addr = 0;
    mask.addr = 0;
        
    // В этой функции задержка
    netif_add(&m_ethNetif, &ip, &mask, &gate, NULL, &ethernetif_init, &tcpip_input);
    netif_set_default(&m_ethNetif);
    netif_set_link_up(&m_ethNetif);
    netif_set_up(&m_ethNetif);
  
    return true;
}

bool EthernetBridge::isLinkUp()
{
    uint16_t phyreg = 0;
    
    //HAL_ETH_ReadPHYRegister(&EthHandle, PHY_BSR, &phyreg);
    phyreg = ETH_ReadPHYRegister(DP83848_PHY_ADDRESS, PHY_SR);
    
    if (phyreg & 1) 
        return true;
    
    return false;
}

//
void EthernetBridge::setOptions(EthernetOptions* opt)
{
    m_options = opt;
}

//
void EthernetBridge::macToChar()
{
    char dummy[2];
  
    for (uint8_t i = 0; i < 6; i++)
    {
        strncpy(dummy, m_options->mac+i*3, 2);
        m_mac[i] = (uint8_t)strtol(dummy, NULL, 16);
    }
}

//
void EthernetBridge::setStaticIP()
{
/*    m_ip.addr   = ipaddr_addr(m_options->ip);
    m_gate.addr = ipaddr_addr(m_options->gate);
    m_mask.addr = ipaddr_addr(m_options->mask);
*/        
    extern eeprom_t eeprom_data;

    m_ip.addr   = eeprom_data.eeprom_ip;
    m_gate.addr = eeprom_data.eeprom_gate;
    m_mask.addr = eeprom_data.eeprom_mask;
    // В этой функции задержка
    netif_add(&m_ethNetif, &m_ip, &m_mask, &m_gate, NULL, &ethernetif_init, &tcpip_input);
    netif_set_default(&m_ethNetif);
}

//
void EthernetBridge::setDinamicIP()
{
    switch (ethernetBridge.m_dhpcState) {
        
    case statedhcp_Start:
        
        m_ethNetif.ip_addr.addr = 0;
        m_ethNetif.netmask.addr = 0;
        m_ethNetif.gw.addr = 0;
        dhcp_start(&m_ethNetif);
        
        m_dhpcState = statedhcp_WaitAddr;
        
        //printf ("\r\nLooking for DHCP server ...\r\n");
        
    break;
      
    case statedhcp_WaitAddr:
      
        if (m_ethNetif.ip_addr.addr != 0) 
        {
            m_dhpcState = statedhcp_AddrAssigned;	
            m_ip   = m_ethNetif.ip_addr;
            m_mask = m_ethNetif.netmask;
            m_gate = m_ethNetif.gw;
		  
            dhcp_stop(&m_ethNetif);
        }
        else
        {
            if (m_dhcpTryCounter++ > MAX_DHCP_TRIES)
            {
                m_dhpcState = statedhcp_Timeout;
                dhcp_stop(&m_ethNetif);
                m_dhcpTryCounter = 0;
            }
        }
    break;
    }
    vTaskDelay(250);
}

//
EthState EthernetBridge::getState()
{
    return m_state;
}

//
void vEthernetBridge(void* params)
{
    for (;;)
    {
        ethernetBridge.run();
    }
}





