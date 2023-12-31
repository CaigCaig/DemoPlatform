//#include "opt.h"
//#include "arch.h"
#include "api.h"
//#include "inet.h"
#include "sockets.h"
#include "string.h"
#include "com_handler.h"

//#define WEBSERVER

#include "httpserver-socket.h"

#ifdef WEBSERVER
#include "fs.h"
#include "fsdata.h"
#endif

/* Private define ------------------------------------------------------------*/
#define WEBSERVER_THREAD_PRIO    ( tskIDLE_PRIORITY + 3 )

/*
typedef struct _xx
{
  uint8_t buf1[8];
  fd_set readset;
  uint8_t buf2[8];
  fd_set writeset;
  uint8_t buf3[8];
  fd_set errset;
  uint8_t buf4[8];
} fdsets;

#define INIT_FDSETS(sets) do { \
  memset((sets)->buf1, 0xab, 8); \
  memset((sets)->buf2, 0xab, 8); \
  memset((sets)->buf3, 0xab, 8); \
  memset((sets)->buf4, 0xab, 8); \
}while(0)

#define CHECK_FDSETS(sets) do { \
  LWIP_ASSERT("buf1 fail", !memcmp((sets)->buf1, cmpbuf, 8)); \
  LWIP_ASSERT("buf2 fail", !memcmp((sets)->buf2, cmpbuf, 8)); \
  LWIP_ASSERT("buf3 fail", !memcmp((sets)->buf3, cmpbuf, 8)); \
  LWIP_ASSERT("buf4 fail", !memcmp((sets)->buf4, cmpbuf, 8)); \
}while(0)
*/

/* Private variables ---------------------------------------------------------*/
uint32_t nPageHits = 0;
portCHAR PAGE_BODY[512];

#ifdef WEBSERVER
/* Format of dynamic web page: the page header */
static const unsigned char PAGE_START[] = {
0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,0x20,0x68,0x74,0x6d,0x6c,0x20,0x50,
0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,0x57,0x33,0x43,0x2f,0x2f,0x44,
0x54,0x44,0x20,0x48,0x54,0x4d,0x4c,0x20,0x34,0x2e,0x30,0x31,0x2f,0x2f,0x45,0x4e,
0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,
0x2e,0x6f,0x72,0x67,0x2f,0x54,0x52,0x2f,0x68,0x74,0x6d,0x6c,0x34,0x2f,0x73,0x74,
0x72,0x69,0x63,0x74,0x2e,0x64,0x74,0x64,0x22,0x3e,0x0d,0x0a,0x3c,0x68,0x74,0x6d,
0x6c,0x3e,0x0d,0x0a,0x3c,0x68,0x65,0x61,0x64,0x3e,0x0d,0x0a,0x20,0x20,0x3c,0x74,
0x69,0x74,0x6c,0x65,0x3e,0x53,0x54,0x4d,0x33,0x32,0x46,0x34,0x78,0x37,0x54,0x41,
0x53,0x4b,0x53,0x3c,0x2f,0x74,0x69,0x74,0x6c,0x65,0x3e,0x0d,0x0a,0x20,0x20,0x3c,
0x6d,0x65,0x74,0x61,0x20,0x68,0x74,0x74,0x70,0x2d,0x65,0x71,0x75,0x69,0x76,0x3d,
0x22,0x43,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x2d,0x54,0x79,0x70,0x65,0x22,0x0d,0x0a,
0x20,0x63,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x3d,0x22,0x74,0x65,0x78,0x74,0x2f,0x68,
0x74,0x6d,0x6c,0x3b,0x20,0x63,0x68,0x61,0x72,0x73,0x65,0x74,0x3d,0x77,0x69,0x6e,
0x64,0x6f,0x77,0x73,0x2d,0x31,0x32,0x35,0x32,0x22,0x3e,0x0d,0x0a,0x20,0x20,0x3c,
0x6d,0x65,0x74,0x61,0x20,0x68,0x74,0x74,0x70,0x2d,0x65,0x71,0x75,0x69,0x76,0x3d,
0x22,0x72,0x65,0x66,0x72,0x65,0x73,0x68,0x22,0x20,0x63,0x6f,0x6e,0x74,0x65,0x6e,
0x74,0x3d,0x22,0x31,0x22,0x3e,0x0d,0x0a,0x20,0x20,0x3c,0x6d,0x65,0x74,0x61,0x20,
0x63,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x3d,0x22,0x4d,0x53,0x48,0x54,0x4d,0x4c,0x20,
0x36,0x2e,0x30,0x30,0x2e,0x32,0x38,0x30,0x30,0x2e,0x31,0x35,0x36,0x31,0x22,0x20,
0x6e,0x61,0x6d,0x65,0x3d,0x22,0x47,0x45,0x4e,0x45,0x52,0x41,0x54,0x4f,0x52,0x22,
0x3e,0x0d,0x0a,0x20,0x20,0x3c,0x73,0x74,0x79,0x6c,0x65,0x20,0x3d,0x22,0x66,0x6f,
0x6e,0x74,0x2d,0x77,0x65,0x69,0x67,0x68,0x74,0x3a,0x20,0x6e,0x6f,0x72,0x6d,0x61,
0x6c,0x3b,0x20,0x66,0x6f,0x6e,0x74,0x2d,0x66,0x61,0x6d,0x69,0x6c,0x79,0x3a,0x20,
0x56,0x65,0x72,0x64,0x61,0x6e,0x61,0x3b,0x22,0x3e,0x3c,0x2f,0x73,0x74,0x79,0x6c,
0x65,0x3e,0x0d,0x0a,0x3c,0x2f,0x68,0x65,0x61,0x64,0x3e,0x0d,0x0a,0x3c,0x62,0x6f,
0x64,0x79,0x3e,0x0d,0x0a,0x3c,0x68,0x34,0x3e,0x3c,0x73,0x6d,0x61,0x6c,0x6c,0x20,
0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x66,0x6f,0x6e,0x74,0x2d,0x66,0x61,0x6d,0x69,
0x6c,0x79,0x3a,0x20,0x56,0x65,0x72,0x64,0x61,0x6e,0x61,0x3b,0x22,0x3e,0x3c,0x73,
0x6d,0x61,0x6c,0x6c,0x3e,0x3c,0x62,0x69,0x67,0x3e,0x3c,0x62,0x69,0x67,0x3e,0x3c,
0x62,0x69,0x67,0x0d,0x0a,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x66,0x6f,0x6e,
0x74,0x2d,0x77,0x65,0x69,0x67,0x68,0x74,0x3a,0x20,0x62,0x6f,0x6c,0x64,0x3b,0x22,
0x3e,0x3c,0x62,0x69,0x67,0x3e,0x3c,0x73,0x74,0x72,0x6f,0x6e,0x67,0x3e,0x3c,0x65,
0x6d,0x3e,0x3c,0x73,0x70,0x61,0x6e,0x0d,0x0a,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,
0x22,0x66,0x6f,0x6e,0x74,0x2d,0x73,0x74,0x79,0x6c,0x65,0x3a,0x20,0x69,0x74,0x61,
0x6c,0x69,0x63,0x3b,0x22,0x3e,0x53,0x54,0x4d,0x33,0x32,0x46,0x34,0x78,0x37,0x20,
0x4c,0x69,0x73,0x74,0x20,0x6f,0x66,0x20,0x74,0x61,0x73,0x6b,0x73,0x20,0x61,0x6e,
0x64,0x0d,0x0a,0x74,0x68,0x65,0x69,0x72,0x20,0x73,0x74,0x61,0x74,0x75,0x73,0x3c,
0x2f,0x73,0x70,0x61,0x6e,0x3e,0x3c,0x2f,0x65,0x6d,0x3e,0x3c,0x2f,0x73,0x74,0x72,
0x6f,0x6e,0x67,0x3e,0x3c,0x2f,0x62,0x69,0x67,0x3e,0x3c,0x2f,0x62,0x69,0x67,0x3e,
0x3c,0x2f,0x62,0x69,0x67,0x3e,0x3c,0x2f,0x62,0x69,0x67,0x3e,0x3c,0x2f,0x73,0x6d,
0x61,0x6c,0x6c,0x3e,0x3c,0x2f,0x73,0x6d,0x61,0x6c,0x6c,0x3e,0x3c,0x2f,0x68,0x34,
0x3e,0x0d,0x0a,0x3c,0x68,0x72,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x77,0x69,
0x64,0x74,0x68,0x3a,0x20,0x31,0x30,0x30,0x25,0x3b,0x20,0x68,0x65,0x69,0x67,0x68,
0x74,0x3a,0x20,0x32,0x70,0x78,0x3b,0x22,0x3e,0x3c,0x73,0x70,0x61,0x6e,0x0d,0x0a,
0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x66,0x6f,0x6e,0x74,0x2d,0x77,0x65,0x69,
0x67,0x68,0x74,0x3a,0x20,0x62,0x6f,0x6c,0x64,0x3b,0x22,0x3e,0x0d,0x0a,0x3c,0x2f,
0x73,0x70,0x61,0x6e,0x3e,0x3c,0x73,0x70,0x61,0x6e,0x20,0x73,0x74,0x79,0x6c,0x65,
0x3d,0x22,0x66,0x6f,0x6e,0x74,0x2d,0x77,0x65,0x69,0x67,0x68,0x74,0x3a,0x20,0x62,
0x6f,0x6c,0x64,0x3b,0x22,0x3e,0x0d,0x0a,0x3c,0x74,0x61,0x62,0x6c,0x65,0x20,0x73,
0x74,0x79,0x6c,0x65,0x3d,0x22,0x77,0x69,0x64,0x74,0x68,0x3a,0x20,0x39,0x36,0x31,
0x70,0x78,0x3b,0x20,0x68,0x65,0x69,0x67,0x68,0x74,0x3a,0x20,0x33,0x30,0x70,0x78,
0x3b,0x22,0x20,0x62,0x6f,0x72,0x64,0x65,0x72,0x3d,0x22,0x31,0x22,0x0d,0x0a,0x20,
0x63,0x65,0x6c,0x6c,0x70,0x61,0x64,0x64,0x69,0x6e,0x67,0x3d,0x22,0x32,0x22,0x20,
0x63,0x65,0x6c,0x6c,0x73,0x70,0x61,0x63,0x69,0x6e,0x67,0x3d,0x22,0x32,0x22,0x3e,
0x0d,0x0a,0x20,0x20,0x3c,0x74,0x62,0x6f,0x64,0x79,0x3e,0x0d,0x0a,0x20,0x20,0x20,
0x20,0x3c,0x74,0x72,0x3e,0x0d,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x3c,0x74,0x64,
0x0d,0x0a,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x66,0x6f,0x6e,0x74,0x2d,0x66,
0x61,0x6d,0x69,0x6c,0x79,0x3a,0x20,0x56,0x65,0x72,0x64,0x61,0x6e,0x61,0x3b,0x20,
0x66,0x6f,0x6e,0x74,0x2d,0x77,0x65,0x69,0x67,0x68,0x74,0x3a,0x20,0x62,0x6f,0x6c,
0x64,0x3b,0x20,0x66,0x6f,0x6e,0x74,0x2d,0x73,0x74,0x79,0x6c,0x65,0x3a,0x20,0x69,
0x74,0x61,0x6c,0x69,0x63,0x3b,0x20,0x62,0x61,0x63,0x6b,0x67,0x72,0x6f,0x75,0x6e,
0x64,0x2d,0x63,0x6f,0x6c,0x6f,0x72,0x3a,0x20,0x72,0x67,0x62,0x28,0x35,0x31,0x2c,
0x20,0x35,0x31,0x2c,0x20,0x32,0x35,0x35,0x29,0x3b,0x20,0x74,0x65,0x78,0x74,0x2d,
0x61,0x6c,0x69,0x67,0x6e,0x3a,0x20,0x63,0x65,0x6e,0x74,0x65,0x72,0x3b,0x22,0x3e,
0x3c,0x73,0x6d,0x61,0x6c,0x6c,0x3e,0x3c,0x61,0x0d,0x0a,0x20,0x68,0x72,0x65,0x66,
0x3d,0x22,0x2f,0x53,0x54,0x4d,0x33,0x32,0x46,0x34,0x78,0x37,0x2e,0x68,0x74,0x6d,
0x6c,0x22,0x3e,0x3c,0x73,0x70,0x61,0x6e,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,
0x63,0x6f,0x6c,0x6f,0x72,0x3a,0x20,0x77,0x68,0x69,0x74,0x65,0x3b,0x22,0x3e,0x48,
0x6f,0x6d,0x65,0x0d,0x0a,0x70,0x61,0x67,0x65,0x3c,0x2f,0x73,0x70,0x61,0x6e,0x3e,
0x3c,0x2f,0x61,0x3e,0x3c,0x2f,0x73,0x6d,0x61,0x6c,0x6c,0x3e,0x3c,0x2f,0x74,0x64,
0x3e,0x0d,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x3c,0x74,0x64,0x0d,0x0a,0x20,0x73,
0x74,0x79,0x6c,0x65,0x3d,0x22,0x66,0x6f,0x6e,0x74,0x2d,0x66,0x61,0x6d,0x69,0x6c,
0x79,0x3a,0x20,0x56,0x65,0x72,0x64,0x61,0x6e,0x61,0x3b,0x20,0x66,0x6f,0x6e,0x74,
0x2d,0x77,0x65,0x69,0x67,0x68,0x74,0x3a,0x20,0x62,0x6f,0x6c,0x64,0x3b,0x20,0x66,
0x6f,0x6e,0x74,0x2d,0x73,0x74,0x79,0x6c,0x65,0x3a,0x20,0x69,0x74,0x61,0x6c,0x69,
0x63,0x3b,0x20,0x62,0x61,0x63,0x6b,0x67,0x72,0x6f,0x75,0x6e,0x64,0x2d,0x63,0x6f,
0x6c,0x6f,0x72,0x3a,0x20,0x72,0x67,0x62,0x28,0x35,0x31,0x2c,0x20,0x35,0x31,0x2c,
0x20,0x32,0x35,0x35,0x29,0x3b,0x20,0x74,0x65,0x78,0x74,0x2d,0x61,0x6c,0x69,0x67,
0x6e,0x3a,0x20,0x63,0x65,0x6e,0x74,0x65,0x72,0x3b,0x22,0x3e,0x3c,0x61,0x0d,0x0a,
0x20,0x68,0x72,0x65,0x66,0x3d,0x22,0x53,0x54,0x4d,0x33,0x32,0x46,0x34,0x78,0x37,
0x41,0x44,0x43,0x2e,0x68,0x74,0x6d,0x6c,0x22,0x3e,0x3c,0x73,0x70,0x61,0x6e,0x20,
0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x66,0x6f,0x6e,0x74,0x2d,0x77,0x65,0x69,0x67,
0x68,0x74,0x3a,0x20,0x62,0x6f,0x6c,0x64,0x3b,0x22,0x3e,0x3c,0x2f,0x73,0x70,0x61,
0x6e,0x3e,0x3c,0x2f,0x61,0x3e,0x3c,0x73,0x6d,0x61,0x6c,0x6c,0x3e,0x3c,0x61,0x0d,
0x0a,0x20,0x68,0x72,0x65,0x66,0x3d,0x22,0x2f,0x53,0x54,0x4d,0x33,0x32,0x46,0x34,
0x78,0x37,0x54,0x41,0x53,0x4b,0x53,0x2e,0x68,0x74,0x6d,0x6c,0x22,0x3e,0x3c,0x73,
0x70,0x61,0x6e,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x63,0x6f,0x6c,0x6f,0x72,
0x3a,0x20,0x77,0x68,0x69,0x74,0x65,0x3b,0x22,0x3e,0x4c,0x69,0x73,0x74,0x0d,0x0a,
0x6f,0x66,0x20,0x74,0x61,0x73,0x6b,0x73,0x3c,0x2f,0x73,0x70,0x61,0x6e,0x3e,0x3c,
0x2f,0x61,0x3e,0x3c,0x2f,0x73,0x6d,0x61,0x6c,0x6c,0x3e,0x3c,0x2f,0x74,0x64,0x3e,
0x0d,0x0a,0x20,0x20,0x20,0x20,0x3c,0x2f,0x74,0x72,0x3e,0x0d,0x0a,0x20,0x20,0x3c,
0x2f,0x74,0x62,0x6f,0x64,0x79,0x3e,0x0d,0x0a,0x3c,0x2f,0x74,0x61,0x62,0x6c,0x65,
0x3e,0x0d,0x0a,0x3c,0x62,0x72,0x3e,0x0d,0x0a,0x3c,0x2f,0x73,0x70,0x61,0x6e,0x3e,
0x3c,0x73,0x70,0x61,0x6e,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x66,0x6f,0x6e,
0x74,0x2d,0x77,0x65,0x69,0x67,0x68,0x74,0x3a,0x20,0x62,0x6f,0x6c,0x64,0x3b,0x22,
0x3e,0x3c,0x2f,0x73,0x70,0x61,0x6e,0x3e,0x3c,0x73,0x6d,0x61,0x6c,0x6c,0x3e,0x3c,
0x73,0x70,0x61,0x6e,0x0d,0x0a,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x66,0x6f,
0x6e,0x74,0x2d,0x66,0x61,0x6d,0x69,0x6c,0x79,0x3a,0x20,0x56,0x65,0x72,0x64,0x61,
0x6e,0x61,0x3b,0x22,0x3e,0x4e,0x75,0x6d,0x62,0x65,0x72,0x20,0x6f,0x66,0x20,0x70,
0x61,0x67,0x65,0x20,0x68,0x69,0x74,0x73,0x3a,0x0d,0x0a,0x00};
#endif

/**
  * @brief serve tcp connection  
  * @param conn: connection socket 
  * @retval None
  */
void http_server_serve(int conn) 
{
  uint16_t buflen = 1500;
//  int s;
//  int backlog;
  int data_len;
//  fdsets sets;
//  struct fs_file * file;
  char recv_buffer[1500], transmit_buffer[1500] = {0};
  char com1[]="\ncommand 1 accepted\n";
  char com2[]="\ncommand 2 accepted\n";
  char close_con[]="\n�������� ����������\n";
                                  
  for (;;)
  {
    data_len = read(conn, recv_buffer, buflen); 

/*    s = lwip_socket(AF_INET, SOCK_STREAM, 0);
    FD_ZERO(&sets.readset);
    FD_SET(s, &sets.readset);
    FD_ZERO(&sets.writeset);
    FD_SET(s, &sets.writeset);
    FD_ZERO(&sets.errset);
    FD_SET(s, &sets.errset);
    */
    /* select with waiting should succeed */
//    ret = lwip_select(s + 1, &sets.readset, &sets.writeset, &sets.errset, NULL);
    if(data_len < 0)    // ���� -1, �� ���� ����� �������������������� ethernet ����������
    {
      close(conn);
      return;
    }
    if (data_len>=2)
    {
      Com_Handler(conn, (char *)recv_buffer, data_len);
    }
    if (data_len == 0)
    {
      close(conn);
      return;
    }
    if (strncmp((char *)recv_buffer,"close connection",16)==0)
    {
      strncpy(transmit_buffer, close_con, sizeof(close_con));
      write(conn, (char *)transmit_buffer, sizeof(close_con));
      close(conn);
      return;
    }
    if (strncmp((char *)recv_buffer,"command_1",9)==0)
    {
      strncpy(transmit_buffer, com1, sizeof(com1));
      write(conn, (char *)transmit_buffer, sizeof(com1));
//      return;
    }
    if (strncmp((char *)recv_buffer,"command_2",9)==0)
    {
      strncpy(transmit_buffer, com2, sizeof(com2));
      write(conn, (char *)transmit_buffer, sizeof(com2));
//      return;
    }
  }
  /* Close connection socket */
//  close(conn);
}

#ifdef WEBSERVER
void http_server_web(int conn) 
{
  int buflen = 1500;
  int ret;
  struct fs_file * file;
  unsigned char recv_buffer[1500];
				
  /* Read in the request */
  ret = read(conn, recv_buffer, buflen); 
  if(ret < 0) return;

  /* Check if request to get ST.gif */
  if (strncmp((char *)recv_buffer,"GET /STM32F4x7_files/ST.gif",27)==0)
  {
//    file = fs_open("/STM32F4x7_files/ST.gif"); 
//    write(conn, (const unsigned char*)(file->data), (size_t)file->len);
//    if(file) fs_close(file);
  }
  /* Check if request to get stm32.jpeg */
  else if (strncmp((char *)recv_buffer,"GET /STM32F4x7_files/stm32.jpg",30)==0)
  {
//    file = fs_open("/STM32F4x7_files/stm32.jpg"); 
//    write(conn, (const unsigned char*)(file->data), (size_t)file->len);
//    if(file) fs_close(file);
  }
  /* Check if request to get ST logo.jpeg */
  else if (strncmp((char *)recv_buffer,"GET /STM32F4x7_files/logo.jpg", 29) == 0)
  {
//    file = fs_open("/STM32F4x7_files/logo.jpg"); 
//    write(conn, (const unsigned char*)(file->data), (size_t)file->len);
//    if(file) fs_close(file);
  }
  else if(strncmp((char *)recv_buffer, "GET /STM32F4x7TASKS.html", 24) == 0)
  {
    /* Load dynamic page */
    DynWebPage(conn);
  }
  else if((strncmp((char *)recv_buffer, "GET /STM32F4x7.html", 19) == 0)||(strncmp((char *)recv_buffer, "GET / ", 6) == 0))
  {
    /* Load STM32F4x7 page */
//    file = fs_open("/STM32F4x7.html"); 
//    write(conn, (const unsigned char*)(file->data), (size_t)file->len);
//    if(file) fs_close(file);
  }
  else
  {
    /* Load 404 page */
//    file = fs_open("/404.html"); 
//    write(conn, (const unsigned char*)(file->data), (size_t)file->len);
//    if(file) fs_close(file);
  }
  /* Close connection socket */
  close(conn);
}
#endif

/**
  * @brief  http server thread 
  * @param arg: pointer on argument(not used here) 
  * @retval None
  */
static void http_server_socket_thread(void *arg)
{
  int sock, newconn, size;
  struct sockaddr_in address, remotehost;

 /* create a TCP socket */
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
  {
//    printf("can not create socket");
    return;
  }
  
  /* bind to port 80 at any interface */
  address.sin_family = AF_INET;
  address.sin_port = htons(49003);
  address.sin_addr.s_addr = INADDR_ANY;

  if (bind(sock, (struct sockaddr *)&address, sizeof (address)) < 0)
  {
//    printf("can not bind socket");
    return;
  }
  
  /* listen for incoming connections (TCP listen backlog = 5) */
  listen(sock, 5);
  
  size = sizeof(remotehost);
  
  while (1) 
  {
    newconn = accept(sock, (struct sockaddr *)&remotehost, (socklen_t *)&size);
    http_server_serve(newconn);
  }
}

#ifdef WEBSERVER
static void http_server_web_thread(void *arg)
{
  int sock, newconn, size;
  struct sockaddr_in address, remotehost;

 /* create a TCP socket */
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
  {
//    printf("can not create socket");
    return;
  }
  
  /* bind to port 80 at any interface */
  address.sin_family = AF_INET;
  address.sin_port = htons(80);
  address.sin_addr.s_addr = INADDR_ANY;

  if (bind(sock, (struct sockaddr *)&address, sizeof (address)) < 0)
  {
//    printf("can not bind socket");
    return;
  }
  
  /* listen for incoming connections (TCP listen backlog = 5) */
  listen(sock, 5);
  
  size = sizeof(remotehost);
  
  while (1) 
  {
    newconn = accept(sock, (struct sockaddr *)&remotehost, (socklen_t *)&size);
    http_server_web(newconn);
  }
}
#endif

/**
  * @brief  Initialize the HTTP server (start its thread) 
  * @param  none
  * @retval None
  */
void http_server_socket_init()
{
  sys_thread_new("HTTP", http_server_socket_thread, NULL, DEFAULT_THREAD_STACKSIZE * 2, WEBSERVER_THREAD_PRIO);
}

#ifdef WEBSERVER
void http_server_web_init()
{
  sys_thread_new("WEB", http_server_web_thread, NULL, DEFAULT_THREAD_STACKSIZE * 2, WEBSERVER_THREAD_PRIO);
}
#endif

#ifdef WEBSERVER
void DynWebPage(int conn)
{
  portCHAR pagehits[10];

  memset(PAGE_BODY, 0,512);

  /* Update the hit count */
  nPageHits++;
//  sprintf( pagehits, "%d", nPageHits );
  strcat(PAGE_BODY, pagehits);
  strcat((char *) PAGE_BODY, "<pre><br>Name          State  Priority  Stack   Num" );
  strcat((char *) PAGE_BODY, "<br>---------------------------------------------<br>"); 
    
  /* The list of tasks and their status */
  vTaskList((char *)(PAGE_BODY + strlen(PAGE_BODY)));
  strcat((char *) PAGE_BODY, "<br><br>---------------------------------------------"); 
  strcat((char *) PAGE_BODY, "<br>B : Blocked, R : Ready, D : Deleted, S : Suspended<br>");
  
  /* Send the dynamically generated page */
  write(conn, PAGE_START, strlen((char*)PAGE_START));
  write(conn, PAGE_BODY, strlen(PAGE_BODY));
}
#endif