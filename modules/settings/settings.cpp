#include "settings.h"
//#include "coffee_settings.h"
#include "FreeRTOS.h"
#include "task.h"

#include <string.h>


//extern CoffeeOptions options;

//Settings settings;

Settings::Settings()
{}

//
void Settings::loadSettings()
{
/*  
  if (loadFromSd() == false)
  {
    // Собщение об ошибке. 
    // Перевод контроллера в состояние "Не прошел загрузку"
  }  
*/  
}

//
bool Settings::loadFromSd()
{
/*  
  FATFS     sdFileSystem;
  char      sdPath[4];
  FIL       file;
  //FRESULT   res;  
  
  if(FATFS_LinkDriver(&SD_Driver, sdPath))
    return false;
  
  if(f_mount(&sdFileSystem, (TCHAR const*)sdPath, 1) != FR_OK)
    return false;
  
  if (f_chdir("/") != FR_OK)
    return false;
  
  if (f_open(&file, "STM32.TXT", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
    return false;
  
  f_close(&file);
*/   
  return true;
  
}

//
void Settings::TestSetAll()
{
/*  
  //options.restore();
  
  // Пока так для теста
  //BackliteOptions backliteOptions; // Подсветка
  NetCommunicatorOptions netCommunicatorOptions; // Сетевые коммуникации
    
  
  //memset(&addr, 0, sizeof(addr));
  netCommunicatorOptions.addr.sin_len = sizeof(netCommunicatorOptions.addr);
  netCommunicatorOptions.addr.sin_addr.s_addr = inet_addr("5.45.112.31");
  netCommunicatorOptions.addr.sin_family = AF_INET;
  netCommunicatorOptions.addr.sin_port = PP_HTONS(8089);
  
  netCommunicatorOptions.conTimeoutMs = 500;
  netCommunicatorOptions.recTimeoutMs = 200;
  
  netCommunicator.setOptions(netCommunicatorOptions);
  //uploader.setOptions(netCommunicatorOptions);
*/  
}




