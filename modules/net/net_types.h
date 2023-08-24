#ifndef __NET_TYPES_H
#define __NET_TYPES_H

#ifdef __cplusplus
 extern "C" {
#endif
   
#include "lwip/sockets.h"
   
typedef struct HttpRequestOptions {

    uint32_t conTimeout;
    uint32_t recvTimeout;
    uint16_t recvBufMaxLen;
    
} HttpRequestOptions;
   
typedef struct RemouteServerOptions {
  
    struct sockaddr_in addr;
    char host[30];
    
} RemouteServerOptions;

typedef struct ServerOptions {

    HttpRequestOptions*     requestOpt;
    RemouteServerOptions*   serverOpt;
  
} ServerOptions; 

#ifdef __cplusplus
}
#endif

#endif  // __NET_TYPES_H

