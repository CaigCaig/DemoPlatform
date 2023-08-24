#include "stm32f4xx.h"
//#include "FreeRTOS.h"
//#include "task.h"
#include "ip_stack.h"

#include "netconf.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "lwip/opt.h"
#include "ethernetif.h"

IP_Stack lwip;

//
bool IP_Stack::configure()
{
    tcpip_init(NULL, NULL);
    
    return true;
}






