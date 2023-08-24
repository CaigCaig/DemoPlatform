#ifndef __TERMINAL_COFFEE_H
#define __TERMINAL_COFFEE_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "terminal.h"

#define DBG_STR_SIZE    100

typedef struct DebugMessage
{
    char      str[DBG_STR_SIZE];
    TextColor color;
} DebugMessage;

//namespace Coffee {


class MedsensTerminal : public Terminal
{
public:
    MedsensTerminal();

    virtual void configure();
    virtual int execute(int argc, const char * const * argv);
    virtual void sigint (void);
    virtual void connectCallback();


public:
    int help(int argc, const char * const * argv);
    int help_connection();
    int version(int argc, const char * const * argv);
    int clear(int argc, const char * const * argv);

    int save(int argc, const char * const * argv);
    int restore(int argc, const char * const * argv);
    int erase(int argc, const char * const * argv);
    int reset(int argc, const char * const * argv);

    int print_all_options(int argc, const char * const *argv);
    
public :
  
    void debug(DebugMessage* msg);
    void dbgMsgProcessing();
    void clearDebugStr(DebugMessage* msg);
    
private :
  
    QueueHandle_t m_dbgMsgQueue;
    
};

extern MedsensTerminal medsensTerminal;

void vDbgMsgTerminal(void* params);



#endif // __TERMINAL_COFFEE_H
