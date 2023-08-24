#include "stm32f4xx.h"
#include "net_bridge.h"

#include <string.h>
#include <stdio.h>

NetBridge netBridge;
TaskHandle_t    xNetBridge;

NetBridge::NetBridge()
{
    m_ethernet = &ethernetBridge;
    
    m_ethState = false;
    m_gsmState = false;
    m_wifiState = false;
    
//    medsensTerminal.clearDebugStr(&m_dbgMsg);
}

//
bool NetBridge::configure()
{
    // LwIP
    m_ipStack = &lwip;
        
    //Ehternet  
    if (m_options.eth.exist)
    {
        m_ipStack->configure();
        m_ethernet->setOptions(&m_options.eth);
        m_ethernet->configure();
        m_ethTaskEnabled = true;
        xTaskCreate(vEthernetBridge, "Ethernet_bridge", 2*configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &m_ethTaskHandle);  
    }

    // Запускаем таск мониторинга подключенных сетевых портов
    if (m_options.eth.exist || m_options.gsmExist || m_options.wifiExist)
        xTaskCreate(vNetBridge, "Net_bridge", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xNetBridge);  
    
    return true;
}

// Контроль состояния подключенных сетевых портов
void NetBridge::run()
{
    if (m_options.eth.exist) {
        ethTest();
    }
    
    vTaskDelay(500);
}

// Контроль Ethernet порта
void NetBridge::ethTest()
{
    EthState ethState;
    
    // Таск ethernet порта активен. Идет процесс получения сетевых параметров.
    // Мониторим состояние, удаляем таск.
    if (m_ethTaskEnabled)
    {
        ethState = m_ethernet->getState();
        
        switch (ethState) {
        case ethstate_NotInit :
            m_ethState = false;
        break;
        
        case ethstate_Configure :
            m_ethState = false;
        break;
        
        case ethstate_WaitAddr :
            m_ethState = false;
        break;  
        
        case ethstate_LinkDown :
            m_ethState = false;
        break;
        
        // Получена IP адрес или установлен статический
        case ethstate_Running :
            
//            printNetParams();

            m_ethState = true;
            m_ethTaskEnabled = false;
            vTaskDelete(m_ethTaskHandle);
            
        break;  
        
        case ethstate_Error :
            m_ethState = false;
            m_ethTaskEnabled = false;
            vTaskDelete(m_ethTaskHandle);
        break;
        
        default :
            m_ethState = false;
        break;
        }
        
        return;
    }
    
    // Сетевые параметры установлены. Таска уже не активна.
    // Нужно периодически мониторить состояние ethernet порта.
    
    // if (m_ethernet->...)
}

// Вывод сетевых параметров
void NetBridge::printNetParams()
{
    EthState ethState;
    
    if (m_options.eth.exist) {
      
        ethState = m_ethernet->getState();
      
        switch (ethState) 
        {
        case ethstate_NotInit :
//            sprintf(m_dbgMsg.str, " \r\nEthernet no init\r\n");
//            medsensTerminal.debug(&m_dbgMsg);
        break;
        
        case ethstate_WaitAddr :
//            sprintf(m_dbgMsg.str, " \r\nWaiting DHCP parameters...\r\n");
//            medsensTerminal.debug(&m_dbgMsg);
        break; 
        
        case ethstate_Running :
            if (m_ethernet->m_options->isDHCP)
            {  
//                strcpy(m_dbgMsg.str, "\r\nParameters assigned by a DHCP server:\r\n");
//                medsensTerminal.debug(&m_dbgMsg);
            }  
            else
            {
//                strcpy(m_dbgMsg.str, "Static address is set:\r\n");
//                medsensTerminal.debug(&m_dbgMsg);
            }
            
//            sprintf(m_dbgMsg.str, " IP      : %s\r\n", ip4addr_ntoa(&m_ethernet->m_ip));
//            medsensTerminal.debug(&m_dbgMsg);  
//            sprintf(m_dbgMsg.str, " Netmask : %s\r\n", ip4addr_ntoa(&m_ethernet->m_mask));
//            medsensTerminal.debug(&m_dbgMsg);  
//            sprintf(m_dbgMsg.str, " Gateway : %s\r\n", ip4addr_ntoa(&m_ethernet->m_gate));
//            medsensTerminal.debug(&m_dbgMsg);
        break;
        
        case ethstate_LinkDown :
//            sprintf(m_dbgMsg.str, " \r\nEthernet link down!\r\n");
//            medsensTerminal.debug(&m_dbgMsg);
        break;
        
        case ethstate_Error :
//            sprintf(m_dbgMsg.str, " \r\nEthernet error!\r\n");
//            medsensTerminal.debug(&m_dbgMsg);
        break;
        }
    }
}

bool NetBridge::getEthState()
{
    EthState ethState;
    
    if (m_options.eth.exist) {
        ethState = m_ethernet->getState();
    
        if (ethState == ethstate_Running)
            return true;
    }
    
    return false;
}

//
void vNetBridge(void *params)
{
    for (;;)
    {  
        netBridge.run();
    }
}
