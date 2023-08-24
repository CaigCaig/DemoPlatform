#ifndef __FLASH_MANAGER_H
#define __FLASH_MANAGER_H

void FLASH_MAN_EraseSettingSector(void);

void FLASH_MAN_WriteCRC(void);

void FLASH_MAN_Testing(void);

bool FLASH_MAN_CheckKey(void);

void FLASH_MAN_WriteKey(void);

#endif  // __FLASH_MANAGER_H

