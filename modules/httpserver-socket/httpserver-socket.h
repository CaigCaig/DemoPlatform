/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HTTPSERVER_SOCKET_H__
#define __HTTPSERVER_SOCKET_H__

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void http_server_socket_init(void);

#ifdef WEBSERVER
void http_server_web_init(void);
void DynWebPage(int conn);
#endif

#endif /* __HTTPSERVER_SOCKET_H__ */