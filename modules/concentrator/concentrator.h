#ifndef __CONCENTRATOR_H
#define __CONCENTRATOR_H

#include "lwip/sockets.h"

#define ID_SIZE      40

typedef struct ConcentratorOptions {
  
    struct sockaddr_in addr;
    
    char destid[ID_SIZE];           
    char sendid[ID_SIZE];
    
} ConcentratorOptions;



class Concentrator {
  
public :
    Concentrator() {}
        
public :
    ConcentratorOptions m_options;
};


extern Concentrator concentrator;

#endif // __CONCENTRATOR_H
