#ifndef __MANAGER_TCP_H
#define __MANAGER_TCP_H

#include "lwip/sockets.h"


// Connection errors
typedef enum
{
    conres_SocketErr = 0,
    conres_ConTimeout,
    conres_ConSuccess,
}
conRes;



class ManagerTCP {
  
public :
    ManagerTCP() {}

    void configure(struct sockaddr_in* addr);
      
public :
    conRes connection(struct sockaddr_in* addr);
    bool closeCon();
    bool request(char* send, char* recv, uint16_t maxRecvSize);
    bool timeRequest(char* send, char* recv, uint16_t maxRecvSize, uint32_t* time);
    
private :
    bool setSendTimout(uint16_t sec, uint16_t usec);
    bool setRecvTimout(uint16_t sec, uint16_t usec);
    
private :
    int m_socket;
    struct sockaddr_in* m_sockAddr; 
    
        
};


extern ManagerTCP managerTCP;


#endif // __MANAGER_TCP_H
