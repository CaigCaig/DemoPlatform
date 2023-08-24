#include "stm32f4xx.h"
#include "mod_config.h"
#include "msens_settings.h"
#include "settings.h"
#include "flash_manager.h"
#include "net_bridge.h"
#include "concentrator.h"

#include <string.h>


//-----------------------------------------------------------------------------
// Классы сохранения настроек в конкретной задаче
//-----------------------------------------------------------------------------

//Список ключей
static char* keys[] = {
    "net_bridge",
    "concentrator",
};

//Количество ключей
static const int c_countKeys = sizeof(keys) / sizeof(keys[0]);

OptionsFlash        flashStorage(SETTINGS_BASE_ADDRESS);

CoffeeOptions       options(&flashStorage);



//Класс работы с настройками конкретной программы
//  - загрузка/сохранение
//  - доступ
//  - изменение настроек

//CoffeeOptions(CoffeeFileStorage* storage) : Options(storage) {}


  //Загрузка / сохранение
void CoffeeOptions :: save() 
{
    resetWrite();

    writeBlock((char*)&netBridge.m_options, sizeof(NetOptions)); 
    writeBlock((char*)&concentrator.m_options, sizeof(ConcentratorOptions));
}


bool CoffeeOptions :: restore() 
{
    resetRead();

    int size = sizeof(NetOptions);
    readBlock((char*)&netBridge.m_options , size);
        
    size = sizeof(ConcentratorOptions);
    readBlock((char*)&concentrator.m_options , size);
    
    return false;
}


// Записать CRC для дебага
void CoffeeOptions :: writeCRC()
{
    
}

// Реализация логики загрузки настроек
bool CoffeeOptions :: loadSettings()
{
    // Проверка наличия/целостности настроек
    if (!FLASH_MAN_CheckKey()) {
        setDefault();
    }
    
    // Загружаем настройки из flash
    setStorage(m_flashStorage);
    restore();        
    
    return true;
}

//
void CoffeeOptions :: setDefault()
{
    
    setDefNetBridge();
    setDefConcentrator();
    // ...
        
    setStorage(m_flashStorage);
    FLASH_MAN_EraseSettingSector();
    
    FLASH_Unlock();
    save();
    FLASH_MAN_WriteKey();
    FLASH_Lock();
    
}

//
void CoffeeOptions :: setDefNetBridge()
{
    netBridge.m_options.gsmExist = false;
    netBridge.m_options.wifiExist = false;
    
    netBridge.m_options.eth.exist = true;
    netBridge.m_options.eth.isDHCP = true;
    strcpy(netBridge.m_options.eth.ip, "0.0.0.0");
    strcpy(netBridge.m_options.eth.gate, "0.0.0.0");
    strcpy(netBridge.m_options.eth.mask, "0.0.0.0");
    strcpy(netBridge.m_options.eth.mac, "f0:7d:68:0f:aa:55");
}

//
void CoffeeOptions :: setDefConcentrator()
{
    concentrator.m_options.addr.sin_len = sizeof(concentrator.m_options.addr);
    concentrator.m_options.addr.sin_family = AF_INET;
    //concentrator.m_options.addr.sin_addr.s_addr = inet_addr("192.168.1.2"); 
    concentrator.m_options.addr.sin_addr.s_addr = inet_addr("83.69.204.14"); // remoute, сервер
    //concentrator.m_options.addr.sin_addr.s_addr = inet_addr("10.6.8.50"); // local, сервер    
    
    concentrator.m_options.addr.sin_port = PP_HTONS(55000);
        
    //concentrator.m_options.addr.sin_addr.s_addr = inet_addr("192.168.1.2"); // MQTT Localhost
    //concentrator.m_options.addr.sin_port = PP_HTONS(1883);
    
    memset(concentrator.m_options.destid, 0, ID_SIZE);
    memset(concentrator.m_options.sendid, 0, ID_SIZE);
        
    strcpy(concentrator.m_options.destid, "DHB-MD737408184368469P0");
    strcpy(concentrator.m_options.sendid, "SNR-MD123456789012345P0");
}

  //Доступ к настройкам
/*
const EthernetOptions& CoffeeOptions :: ethernet() {
  return ethernetOptions;
}
*/

  //Управление настройками
/*
void CoffeeOptions :: setEthernet(const EthernetOptions& ethernet) {
  ethernetOptions = ethernet;
}
*/



