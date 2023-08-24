#include "stm32f4xx.h"

#include "netconf.h"

#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "lwip/opt.h"
#include "ethernetif.h"

//EthernetOptions ethernetOptions;

void DHCP_thread(void const * argument);

struct netif gnetif; /* network interface structure */
__IO uint8_t DHCP_state = DHCP_START;
uint8_t iptxt[20];

bool ethernet_port = false;

/**
  * @brief  
  */
void NET_Init(void)
{
  tcpip_init(NULL, NULL);
  NET_InitNetif();
  //User_notification(&gnetif);
  
}

/**
  * @brief  
  */
void NET_InitNetif(void)
{
#if 0  
    ip4_addr_t ip;
    ip4_addr_t gate;	
    ip4_addr_t mask;
  
    // DHCP включен
    if (ethernetOptions.isDHCP) 
    {
        ip.addr   = 0;
        gate.addr = 0;
        mask.addr = 0;
        
        // В этой функции задержка
        netif_add(&gnetif, &ip, &mask, &gate, NULL, &ethernetif_init, &tcpip_input);
        netif_set_default(&gnetif);
        
        if (netif_is_link_up(&gnetif))
            netif_set_up(&gnetif);
        else {
            netif_set_down(&gnetif);
        }
        
        osThreadDef(DHCP, DHCP_thread, osPriorityBelowNormal, 0, 2*configMINIMAL_STACK_SIZE);
        osThreadCreate (osThread(DHCP), &gnetif);
    }
    else
    {
        ip.addr   = ipaddr_addr(ethernetOptions.ip);
        gate.addr = ipaddr_addr(ethernetOptions.gate);
        mask.addr = ipaddr_addr(ethernetOptions.mask);
        
        // В этой функции задержка
        netif_add(&gnetif, &ip, &mask, &gate, NULL, &ethernetif_init, &tcpip_input);
        netif_set_default(&gnetif);
  
        printf("Static IP parameters:\r\n");  
		printf(" IP      : %s\r\n", ip4addr_ntoa(&ip));
		printf(" Netmask : %s\r\n", ip4addr_ntoa(&mask));
		printf(" Gateway : %s\r\n", ip4addr_ntoa(&gate));
        
        if (netif_is_link_up(&gnetif))
            netif_set_up(&gnetif);
        else
            netif_set_down(&gnetif);
        
        ethernet_port = true;
    }
#endif    
}

/**
* @brief  DHCP Process
* @param  argument: network interface
* @retval None
*/
void DHCP_thread(void const * argument)
{
#if 0  
  struct netif *netif = (struct netif *) argument;
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;
  uint32_t IPaddress;
  static uint8_t dhcpTryCounter = 0; // dhcp try counter
  
  
  for (;;)
  {
    switch (DHCP_state)
    {
    case DHCP_START:
      {
        netif->ip_addr.addr = 0;
        netif->netmask.addr = 0;
        netif->gw.addr = 0;
        IPaddress = 0;
        dhcp_start(netif);
        DHCP_state = DHCP_WAIT_ADDRESS;
        printf ("\r\nLooking for DHCP server ...\r\n");
      }
      break;
      
    case DHCP_WAIT_ADDRESS:
      {
        /* Read the new IP address */
        IPaddress = netif->ip_addr.addr;
        if (IPaddress!=0) 
        {
          DHCP_state = DHCP_ADDRESS_ASSIGNED;	

		  ipaddr = netif->ip_addr;
		  netmask = netif->netmask;
		  gw = netif->gw;
		  
          /* Stop DHCP */
          dhcp_stop(netif);
	  
		  printf("Parameters assigned by a DHCP server:\r\n");  
		  printf(" IP      : %s\r\n", ip4addr_ntoa(&ipaddr));
		  printf(" Netmask : %s\r\n", ip4addr_ntoa(&netmask));
		  printf(" Gateway : %s\r\n", ip4addr_ntoa(&gw));
          
          ethernet_port = true;
        }
        else
        {
          /* DHCP timeout */ 
          //if (netif->dhcp->tries > MAX_DHCP_TRIES)
          if (dhcpTryCounter++ > MAX_DHCP_TRIES)
          {
            DHCP_state = DHCP_TIMEOUT;
            
            /* Stop DHCP */
            dhcp_stop(netif);
            
            /* Static address used */
            IP4_ADDR(&ipaddr, IP_ADDR0 ,IP_ADDR1 , IP_ADDR2 , IP_ADDR3 );
            IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
            IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
            netif_set_addr(netif, &ipaddr , &netmask, &gw);
            
            //uint8_t iptxt[20];
            
            sprintf((char*)iptxt, "%d.%d.%d.%d", IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
            //LCD_UsrLog ("DHCP timeout !!\n");
            //LCD_UsrLog ("Static IP address  : %s\n", iptxt);
          }
        }
      }
      break;
      
    default: break;
    }
    
    /* wait 250 ms */
    osDelay(250);
  }
#endif  
}

#if 0
extern ETH_HandleTypeDef EthHandle;

void NET_TestPHY(void *params)
{
  uint32_t phyreg = 0;
  //char msg[30];
  
  for (;;)
  {
    vTaskDelay(3000);
    
    for (uint16_t i = 0; i < 10; i++)
    {
      HAL_ETH_ReadPHYRegister(&EthHandle, i, &phyreg);
      
      //memset(msg, 0, 30);
      //sprintf(msg, "Reg %d: %d\r\n", i, phyreg);
      //CDC_Transmit_FS(msg, 30);
      vTaskDelay(100);
    }
    
    vTaskDelete(NULL);
  }
}
#endif