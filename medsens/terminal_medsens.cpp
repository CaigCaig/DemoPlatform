#include "terminal_medsens.h"
#include "commands.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

extern COMMANDS_t commands[];


MedsensTerminal medsensTerminal;

//Глобальный указатель на терминал
Terminal* pTerminal;



MedsensTerminal::MedsensTerminal() :
    Terminal()
{
}



void MedsensTerminal::configure()
{
    m_dbgMsgQueue = xQueueCreate(5, sizeof(DebugMessage));;
  
    Terminal::configure();
    pTerminal = &medsensTerminal;   
    //printeol();
    //printl("Coffee terminal started...");        
}



int MedsensTerminal::execute(int argc, const char * const *argv)
{
    if (argc <= 0) {
        return -1;
    }
    if (strcmp(argv[0], "help") == 0) {
        return help(argc, argv);
    }
    else if (strcmp(argv[0], "version") == 0) {
        return version(argc, argv);
    }
    else if (strcmp(argv[0], "clear") == 0) {
        return clear(argc, argv);
    }    
    else if (strcmp(argv[0], "printall") == 0) {
        return print_all_options(argc, argv);
    }
    
    // Вывести сетевые параметры
    else if (strcmp(argv[0], "ipconfig") == 0) {
      commands[IP_CONFIG].flag = true;
    }
    // Установить дефолтные настройки
    else if (strcmp(argv[0], "setdef") == 0) {
      commands[SET_DEF].flag = true;
    }
    // Вывод статистики LwIP
    else if (strcmp(argv[0], "lwstat") == 0) {
      commands[LWIP_STAT].flag = true;
    }
    // Вывод текущего времени
    else if (strcmp(argv[0], "time") == 0) {
      commands[TIME].flag = true;
    }
    
    else {
        printeol();
        printll("Uncknown command [oO]");
        return -1;
    }
    return 0;
}


void MedsensTerminal::sigint() {
}


//Колбэк, который может быть вызван при подключении
void MedsensTerminal::connectCallback() 
{
    clearScreen();
    printll("Coffee control terminal.");
  
    //Тут выводим полезную при подключении информацию
    printeol();
    printll("For help type 'help'.");
    insert('\r');    
}

    

int MedsensTerminal::help(int argc, const char * const *argv)
{
    printeol();
    printeol();
    printl ("You can use the following commands:");
    printl ("  version        Print Coffee software version");
    printl ("  clear          Clear terminal screen");
    printl ("  printall       Print all options");
    printeol();

    printll("See '<command> help' for read about specific command");
    
    help_connection();
    return 0;
}


//Справка по поводу соединения
int MedsensTerminal::help_connection() 
{
    return 0;
}



int MedsensTerminal::version(int argc, const char * const *argv) 
{
    return 0;
}




int MedsensTerminal::clear(int argc, const char * const *argv)
{
    if ((argc > 1) &&(strcmp(argv[1], "help") == 0))  
    {
        printeol();
        printl("Clear terminal screen");      
        printeol();
        printeol();
        return 0;
    }
    clearScreen();
    return 0;
}


//-----------------------------------------------------------------------------
// Вывод всех настроек
//-----------------------------------------------------------------------------

int MedsensTerminal::print_all_options(int argc, const char * const *argv) 
{
    return 0;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------

void MedsensTerminal::debug(DebugMessage* msg)
{
    
    xQueueSend(m_dbgMsgQueue, msg, 0);
}

void MedsensTerminal::dbgMsgProcessing()
{
    DebugMessage msg;
  
    if (xQueueReceive(m_dbgMsgQueue, &msg, portMAX_DELAY) == pdTRUE)
    {
        print(msg.str);
    }
}

void MedsensTerminal::clearDebugStr(DebugMessage* msg)
{
    memset(msg->str, 0, DBG_STR_SIZE);
}

void vDbgMsgTerminal(void* params)
{
    for (;;)
    {
        medsensTerminal.dbgMsgProcessing();
    }
}

