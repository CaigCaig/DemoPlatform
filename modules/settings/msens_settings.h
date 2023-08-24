#ifndef __MSENS_SETTINGS_H
#define __MSENS_SETTINGS_H

#include "settings.h"





//Класс работы с настройками конкретной программы
//  - загрузка/сохранение
//  - доступ
//  - изменение настроек

class CoffeeOptions : public Options {
public:
    CoffeeOptions(OptionsFlash* flash_storage) : 
        Options(flash_storage), 
        m_flashStorage(flash_storage)
    {}  

    //Загрузка / сохранение
    virtual void save();

    virtual bool restore();

public :    
    
    void writeCRC();
    bool loadSettings(); // Реализация логики загрузки настроек
    void setDefault();    
    
    
    // Доступ к настройкам
    //const EthernetOptions& ethernet();

    // Управление настройками
    //void setEthernet(const EthernetOptions& ethernet);

    // Установка значений по умолчанию
    void setDefNetBridge();
    void setDefConcentrator();
   
private :
  
    OptionsFlash*      m_flashStorage;
    
private :
  
    uint32_t    settingsCRC;    // crc файла настроек, хранится во flash
    
};


extern CoffeeOptions options;

#endif  // __MSENS_SETTINGS_H

