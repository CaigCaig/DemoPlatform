#ifndef __NETCONF_H
#define __NETCONF_H

#include <stdbool.h>
  
#define USE_DHCP       
#define MAX_DHCP_TRIES  4

#define DHCP_START                 (uint8_t) 1
#define DHCP_WAIT_ADDRESS          (uint8_t) 2
#define DHCP_ADDRESS_ASSIGNED      (uint8_t) 3
#define DHCP_TIMEOUT               (uint8_t) 4
#define DHCP_LINK_DOWN             (uint8_t) 5

/*Static IP ADDRESS*/
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   0
#define IP_ADDR3   9
   
/*NETMASK*/
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

/*Gateway Address*/
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   0
#define GW_ADDR3   1 
   
/**
  * @brief  
  */
void NET_Init(void);

/**
  * @brief  
  */
void NET_InitNetif(void);

/**
  * @brief  
  */
void NET_TestPHY(void *params);


//extern EthernetOptions ethernetOptions;
extern bool ethernet_port;




#endif  // __NETCONF_H

