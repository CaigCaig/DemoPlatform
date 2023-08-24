#ifndef __SETTINGS_H
#define __SETTINGS_H

#include "stm32f4xx.h"
//#include "fatfs.h"

/**
  * @brief  Базовый интерфейс для обращения к хранилищам настроек
  */
class OptionsStorage {
public:

    OptionsStorage() {}

    virtual void writeBlock(char* data, int size) = 0;
    virtual void readBlock(char* data, int &size) = 0;
    virtual void resetWrite() = 0;
    virtual void resetRead() = 0;
    virtual bool verify() {return true;}

};



//  Базовый класс для хранения настроек во flash памяти
class OptionsFlash : public OptionsStorage {
public:
    OptionsFlash(uint32_t add) : OptionsStorage() {startAddress = add;}
  
    virtual void writeBlock(char* data, int size) 
    {
        for (uint32_t i = 0; i < size; i++)
            //FLASH_Status FLASH_ProgramByte(uint32_t Address, uint8_t Data)
            //HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, offset++, *data++);
            FLASH_ProgramByte(offset++, *data++);
    }
  
    virtual void readBlock(char* data, int &size) 
    {
        for (uint32_t i = 0; i < size; i++)
            *data++ = (*(uint32_t*)offset++);
    }
  
    virtual void resetWrite()
    {
        offset = startAddress;
    }
  
    virtual void resetRead()
    {
        offset = startAddress;
    }
  
    virtual bool verify() {
        return true;
    }

public :
  
    void setStartAddress(uint32_t add) {
        startAddress = add;
    }
    
private :
    
    uint32_t    startAddress;
    uint32_t    offset;
    
};


/**
  * @brief  
  */
class Options
{
public:
    Options(OptionsStorage* storage) : m_storage(storage) {}

    void setStorage(OptionsStorage* storage) {
        m_storage = storage;
    }
  
protected:
    void writeBlock(char* data, int size) {
        m_storage->writeBlock(data, size);
    }

    void readBlock(char* data, int &size) {
        m_storage->readBlock(data, size);
    }

    void resetRead() {
        m_storage->resetRead();
    }

    void resetWrite() {
        m_storage->resetWrite();
    }

    virtual void save() = 0;
    virtual bool restore() = 0;

private:
    OptionsStorage* m_storage;
};








class Settings {
public:
    
  Settings();
  
  void loadSettings(void);  // 
  void TestSetAll();
  
private:
  
  bool loadFromSd(void);  // Загрузка настроек с SD карты
};

extern Settings settings;


#endif  // __SETTINGS_H

