#include "manager_tcp.h"

#include "FreeRTOS.h"
#include "task.h"

#include "string.h"

ManagerTCP managerTCP;

void ManagerTCP::configure(struct sockaddr_in* addr)
{
    m_sockAddr = addr;
}


// Установка соединения
// Сокет создается в блокирующем режиме
conRes ManagerTCP::connection(struct sockaddr_in* addr)
{   
    int ret;
    int opt;
    
    m_socket = lwip_socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket < 0)
        return conres_SocketErr;  
       
    ret = lwip_connect(m_socket, (struct sockaddr*)addr, addr->sin_len);
    
    if (ret) {
        lwip_close(m_socket);
        return conres_ConTimeout;
    }     
    
    return conres_ConSuccess;
}

//
bool ManagerTCP::request(char* send, char* recv, uint16_t maxRecvSize)
{
    int ret;
    int cnt;
    
    setSendTimout(3, 0);
    setRecvTimout(7, 0);
    
    cnt = strlen(send);
    ret = lwip_write(m_socket, send, cnt);
    if (ret != cnt) {
        printf("Server send error!\r\n");
        return false;
    }
    
    ret = lwip_read(m_socket, recv, maxRecvSize);
  
    // Не удалось принять данные 
    if (ret == -1) {	
        printf("Server receive error!\r\n");
        return false;
    }
/*    
    printf("\r\n");
    printf(recv);
    printf("\r\n");
*/    
    return true;
}

//
bool ManagerTCP::timeRequest(char* send, char* recv, uint16_t maxRecvSize, uint32_t* time)
{
    int ret;
    int cnt;
    uint32_t oldTicks;
    
    setSendTimout(3, 0);
    setRecvTimout(7, 0);
    
    cnt = strlen(send);
    ret = lwip_write(m_socket, send, cnt);
    if (ret != cnt) {
        printf("Server send error!\r\n");
        return false;
    }
    
    oldTicks = xTaskGetTickCount();
    
    ret = lwip_read(m_socket, recv, maxRecvSize);
  
    // Не удалось принять данные 
    if (ret == -1) {	
        printf("Server receive error!\r\n");
        return false;
    }
    
    *time = xTaskGetTickCount() - oldTicks;
/*    
    printf("\r\n");
    printf(recv);
    printf("\r\n");
*/    
    return true;
}

//
bool ManagerTCP::closeCon()
{
    lwip_close(m_socket);
    return true;
}

//
bool ManagerTCP::setSendTimout(uint16_t sec, uint16_t usec)
{
    struct timeval timeout;

    timeout.tv_sec = sec;
    timeout.tv_usec = usec;
    
    if(setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, 
                  sizeof(timeout)) != 0)
    {
        return false;
    }
    return true;
}

//
bool ManagerTCP::setRecvTimout(uint16_t sec, uint16_t usec)
{
    struct timeval timeout;

    timeout.tv_sec = sec;
    timeout.tv_usec = usec;
    
    if(setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, 
                  sizeof(timeout)) != 0)
    {
        return false;
    }
    return true;
}


