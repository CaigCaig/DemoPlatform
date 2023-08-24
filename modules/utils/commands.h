#ifndef __COMMANDS_H
#define __COMMANDS_H

#include <stdbool.h>


/**
  * @brief  
  */
typedef enum
{
  IP_CONFIG = 0,
  SET_DEF,
  LWIP_STAT,
  TIME,
    
} COMMANDS_LIST_t;

/**
  * @brief  
  */
typedef struct
{
  void (*handler)();
  bool flag;
    
} COMMANDS_t;



// -----------------------------------------------------------------------------

void NetParams(void);
void SetDefault(void);

// -----------------------------------------------------------------------------

//
void vCommands(void *params);

//
void COM_Launcher(void);

//
void COM_Configure(void);

// -----------------------------------------------------------------------------

//
void PrintLwipStat(void);

//
void PrintTime(void);

#endif  // __COMMANDS_H

