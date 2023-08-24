#ifndef __24AA025E48_H
#define __24AA025E48_H

typedef struct {
  
    uint32_t eeprom_key;
    uint8_t eeprom_dhcp;  // 0 - DHCP отключен, 1 - включен
    uint32_t eeprom_ip;
    uint32_t eeprom_mask;
    uint32_t eeprom_gate;
    uint8_t eeprom_cs;
  
} eeprom_t;
#define EEPROM_T_SIZE                   ((uint8_t)18)
#define EEPROM_KEY                      ((uint32_t)0xA1B2C3D4)
#define EEPROM_DHCP                     ((uint8_t)0)
#define EEPROM_IP                       ((uint32_t)0xaC0A801F9) // 192.168.1.249
#define EEPROM_MASK                     ((uint32_t)0xaFFFFFF00) // 255.255.255.0
#define EEPROM_GATE                     ((uint32_t)0xaC0A80101) // 192.168.1.1

#define adc_scl_Pin GPIO_Pin_8
#define adc_scl_GPIO_Port GPIOB
#define adc_sda_Pin GPIO_Pin_9
#define adc_sda_GPIO_Port GPIOB

#define EEPROM_I2C     I2C1
#define EEPROM_DEVICE_WRITE_ADDRESS     ((uint8_t)0xA0)
#define EEPROM_DEVICE_READ_ADDRESS      ((uint8_t)0xA1)
#define EEPROM_START_ADDR_MAC           ((uint8_t)0xFA)
#define EEPROM_START_ADDR_DATA          ((uint8_t)0x00)
#define EEPROM_CHARGING_STATE_ADDR      ((uint8_t)0x12)
#define MAC_LEN                         ((uint8_t)0x06)
#define EEPROM_PAGE_SIZE                ((uint8_t)16)
#define EEPROM_WRITE_TIME               ((uint8_t)7)   // время записи страницы в EEPROM, мс
#define EEPROM_DMA_Channel      DMA_Channel_1
#define EEPROM_DMA_Stream       DMA1_Stream0

ErrorStatus EEPROM_Read(uint8_t eeprom_addr, uint8_t buf_len, uint8_t *read_addr);
ErrorStatus EEPROM_Write(uint8_t eeprom_addr, uint8_t buf_len, uint8_t *write_addr);

#endif // __24AA025E48_H