#ifndef __IP_STACK_H
#define __IP_STACK_H

#include "lwip/netif.h"

class IP_Stack {
public:
  
    bool configure();
    
    void initEhtNetif();
    void setMacAddress(char* mac);
    
private:
  
    
};


extern IP_Stack lwip;

#endif  // __IP_STACK_H

