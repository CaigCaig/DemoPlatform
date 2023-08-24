#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "flash_manager.h"
#include "mod_config.h"

#include <stdio.h>
#include <string.h>

//
void FLASH_MAN_EraseSettingSector(void)
{
    FLASH_Unlock();
    FLASH_EraseSector(FLASH_Sector_4, VoltageRange_3);
    FLASH_Lock();
}

//
void FLASH_MAN_WriteCRC(void)
{
    FLASH_Unlock();
    //FLASH_Erase_Sector(FLASH_SECTOR_3, FLASH_VOLTAGE_RANGE_3);
    //HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, CRC_FLASH_ADDRESS, 4136205474);
    FLASH_Lock();
}

//
bool FLASH_MAN_CheckKey(void)
{
    uint32_t key = (*(uint32_t*)SETTINGS_KEY_ADDRESS);
    
    if (key == SETTINGS_KEY)
        return true;
    
    return false;
}

//
void FLASH_MAN_WriteKey(void)
{
    FLASH_ProgramWord(SETTINGS_KEY_ADDRESS, SETTINGS_KEY);
}

//
void FLASH_MAN_Testing(void)
{
#if 0  
    /* Allow Access to Flash control registers and user Flash */   
    HAL_FLASH_Unlock();

    /* Allow Access to option bytes sector */ 
    HAL_FLASH_OB_Unlock();
      
    /* Get the Dual boot configuration status */
    HAL_FLASHEx_OBGetConfig(&OBInit);
        
    printf("OBInit structure:\r\n");
    printf("    OptionType: %X\r\n", OBInit.OptionType);
    
    // Write protection (0 - off, 1 - on)
    printf("    WRPState:   %X\r\n", OBInit.WRPState);
    
    // Write protection sector (Зависит от серии)
    printf("    WRPSector:  %X\r\n", OBInit.WRPSector);
    
    // Read protection level (AA - 0, 55 - 1, CC - 2)
    printf("    RDPLevel:   %X\r\n", OBInit.RDPLevel);
    
    // FLASH BOR Reset Level (Напряжение при котором происходит reset (возможно))
    printf("    BORLevel:   %X\r\n", OBInit.BORLevel);
    
    // Program the FLASH User Option Byte: WWDG_SW / IWDG_SW / RST_STOP / RST_STDBY / 
    // IWDG_FREEZE_STOP / IWDG_FREEZE_SANDBY / nDBANK / nDBOOT.
    // nDBANK / nDBOOT are only available for STM32F76xxx/STM32F77xxx devices 
    printf("    USERConfig: %X\r\n", OBInit.USERConfig);
    
    // Boot base address when Boot pin = 0.
    // This parameter can be a value of @ref FLASHEx_Boot_Address 
    printf("    BootAddr0:  %X\r\n", OBInit.BootAddr0);
    
    // Boot base address when Boot pin = 1.
    // This parameter can be a value of @ref FLASHEx_Boot_Address */
    printf("    BootAddr1:  %X\r\n", OBInit.BootAddr1);
    
    /* Prevent Access to option bytes sector */
    HAL_FLASH_OB_Lock();
    
    /* Disable the Flash option control register access (recommended to protect 
      the option Bytes against possible unwanted operations) */
    HAL_FLASH_Lock();
#endif    
}
