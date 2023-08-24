#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "lcd_hd44780.h"

#include <string.h>

LCD lcd;


/**
  * @brief  
  */
LCD::LCD()
{
  memset(buf, 0, LCD_BUF_SIZE);
  bufIndex = 0;
}

/**
  * @brief  
  */
void LCD::configure()
{
  portInit();
 
  putCmd(0x3A);
  delayUs(1000);
  
  putCmd(0x0C);
  delayUs(1000);
  
  putCmd(0x01);
  delayUs(1000);
  
  putCmd(0x06);
  delayUs(1000);
}

/**
  * @brief  
  */
void LCD::setPositin(uint8_t x, uint8_t y)
{
  if (y >= LCD_HEIGHT) 
    y %= LCD_HEIGHT;

  if (x >= LCD_WIDTH)
    x %= LCD_WIDTH;
  
  // 0x40 - смещение каждой строки в памяти дисплея
  putCmd( LCD_CMD_SET_ADDR | (y * 0x40 + x) );
  delayUs(1000);
}

/**
  * @brief  
  */
void LCD::clear()
{
  char* ptr = buf;

  for (uint16_t i = LCD_BUF_SIZE; i; --i, ++ptr)
    *ptr = ' ';
  
  bufIndex = 0;
}

/**
  * @brief  
  */
void LCD::print(const char *str)
{
  uint8_t len = strlen(str);
  
  clear();
  
  for (uint8_t i = 0; i < len; i++)
    putChar(*str++);
  
  update();
}

/**
  * @brief  
  */
void LCD::printNoClear(const char *str)
{
  uint8_t len = strlen(str);
  
  for (uint8_t i = 0; i < len; i++)
    putChar(*str++);
  
  update();
}

/**
  * @brief  Выключить/выключить подсветку экрана
  */
void LCD::ledOnOff(bool state)
{
  if (state)
    GPIO_SetBits(LCD_PORT_LED, LCD_PIN_LED); // Установка бита
  else
    GPIO_ResetBits(LCD_PORT_LED, LCD_PIN_LED); // Сброс бита
}


/**
  * @brief  
  */
void LCD::putByte(uint8_t byte)
{
  GPIO_ResetBits(LCD_PORT_DB0, LCD_PIN_DB0);
  GPIO_ResetBits(LCD_PORT_DB1, LCD_PIN_DB1);
  GPIO_ResetBits(LCD_PORT_DB2, LCD_PIN_DB2);
  GPIO_ResetBits(LCD_PORT_DB3, LCD_PIN_DB3);
  GPIO_ResetBits(LCD_PORT_DB4, LCD_PIN_DB4);
  GPIO_ResetBits(LCD_PORT_DB5, LCD_PIN_DB5);
  GPIO_ResetBits(LCD_PORT_DB6, LCD_PIN_DB6);
  GPIO_ResetBits(LCD_PORT_DB7, LCD_PIN_DB7);
  
  if (byte & 0x01)
    GPIO_SetBits(LCD_PORT_DB0, LCD_PIN_DB0);
      
  if (byte & 0x02)
    GPIO_SetBits(LCD_PORT_DB1, LCD_PIN_DB1);
  
  if (byte & 0x04)
    GPIO_SetBits(LCD_PORT_DB2, LCD_PIN_DB2);
  
  if (byte & 0x08)
    GPIO_SetBits(LCD_PORT_DB3, LCD_PIN_DB3);
  
  if (byte & 0x10)
    GPIO_SetBits(LCD_PORT_DB4, LCD_PIN_DB4);
  
  if (byte & 0x20)
    GPIO_SetBits(LCD_PORT_DB5, LCD_PIN_DB5);
  
  if (byte & 0x40)
    GPIO_SetBits(LCD_PORT_DB6, LCD_PIN_DB6);
  
  if (byte & 0x80)
    GPIO_SetBits(LCD_PORT_DB7, LCD_PIN_DB7);
}

/**
  * @brief  Записать данные
  */
void LCD::putData(char data)
{
  GPIO_SetBits(LCD_PORT_A0, LCD_PIN_A0);
  GPIO_SetBits(LCD_PORT_E1, LCD_PIN_E1);
  GPIO_ResetBits(LCD_PORT_RW, LCD_PIN_RW);
  
  putByte((uint8_t)data);
  
  GPIO_ResetBits(LCD_PORT_E1, LCD_PIN_E1);
  delayUs(LCD_TIMING_DEL);
}

/**
  * @brief  Записать команду
  */
void LCD::putCmd(char cmd)
{
  GPIO_ResetBits(LCD_PORT_A0, LCD_PIN_A0);
  GPIO_SetBits(LCD_PORT_E1, LCD_PIN_E1);
  GPIO_ResetBits(LCD_PORT_RW, LCD_PIN_RW);
    
  putByte((uint8_t)cmd);
  
  GPIO_ResetBits(LCD_PORT_E1, LCD_PIN_E1);
}

/**
  * @brief  Вывод символа на экран через буфер
  */
void LCD::putChar(char ch)
{
  if (bufIndex >= LCD_BUF_SIZE) {
    // Скролл
    memcpy(buf, &buf[LCD_WIDTH], LCD_WIDTH);
    memset(&buf[LCD_WIDTH], LCD_CLEAR_SYMBOL, LCD_WIDTH);
    bufIndex -= LCD_WIDTH;
  }
  
  switch(ch)
  {
    case '\r':
      bufIndex = bufIndex - bufIndex % LCD_WIDTH;   //переход на начало текущей строки
    break;
    case '\n':
        bufIndex += LCD_WIDTH;  //переход на следующую строку
    break;
    case '\f': // clear form
      bufIndex = 0;
      memset(buf, LCD_CLEAR_SYMBOL, sizeof(buf));
    break;
    default:
      buf[bufIndex++ ] = ch;
    break;
  }
}

/**
  * @brief  Обновление экрана
  */
void LCD::update()
{
  putCmd(LCD_CMD_HOME);
  
  for (uint16_t i = 0; i < LCD_BUF_SIZE; ++i) {
    // переносим курсор в конце каждой строки (т.к. буфер экрана в lcd больше его ширины)
    if (!(i % LCD_WIDTH) ) 
      setPositin(0, i / LCD_WIDTH);
    
    putData(buf[i]);
  }  
  setPositin(bufIndex % LCD_WIDTH, bufIndex / LCD_WIDTH);
}

/**
  * @brief  Задержка в микросекундах (цикл)
  */
void LCD::delayUs(uint16_t us)
{
  uint32_t  tick = SystemCoreClock / 1000000;  
  tick *= us;

  while ( tick-- )
    __asm volatile ( "nop \n" );
}

/**
  * @brief  Настройка портов
  *         LCD LED - Pin29 PA5
  *         A0      - Pin58 PD11
  *         E       - Pin85, Pin88 PD4, PD7
  *         RW      - Pin86 PD5
  *         DB0     - Pin61 PD14
  *         DB1     - Pin62 PD15
  *         DB2     - Pin81 PD0
  *         DB3     - Pin82 PD1
  *         DB4     - Pin37 PE7
  *         DB5     - Pin38 PE8
  *         DB6     - Pin39 PE9
  *         DB7     - Pin40 PE10
  * @retval 
  */
void LCD::portInit()
{
    GPIO_InitTypeDef GPIO_InitStruct;
  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
  
    GPIO_InitStruct.GPIO_Pin = LCD_PIN_A0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(LCD_PORT_A0, &GPIO_InitStruct);
      
    GPIO_InitStruct.GPIO_Pin = LCD_PIN_E1;
    GPIO_Init(LCD_PORT_E1, &GPIO_InitStruct);
  
    GPIO_InitStruct.GPIO_Pin = LCD_PIN_RW;
    GPIO_Init(LCD_PORT_RW, &GPIO_InitStruct);
   
    GPIO_ResetBits(LCD_PORT_A0, LCD_PIN_A0); 
    GPIO_ResetBits(LCD_PORT_E1, LCD_PIN_E1); 
    GPIO_ResetBits(LCD_PORT_RW, LCD_PIN_RW); 
  
    GPIO_InitStruct.GPIO_Pin = LCD_PIN_DB0;  
    GPIO_Init(LCD_PORT_DB0, &GPIO_InitStruct);
  
    GPIO_InitStruct.GPIO_Pin = LCD_PIN_DB1;
    GPIO_Init(LCD_PORT_DB1, &GPIO_InitStruct);
  
    GPIO_InitStruct.GPIO_Pin = LCD_PIN_DB2;
    GPIO_Init(LCD_PORT_DB2, &GPIO_InitStruct);
  
    GPIO_InitStruct.GPIO_Pin = LCD_PIN_DB3;
    GPIO_Init(LCD_PORT_DB3, &GPIO_InitStruct);
  
    GPIO_InitStruct.GPIO_Pin = LCD_PIN_DB4;
    GPIO_Init(LCD_PORT_DB4, &GPIO_InitStruct);
  
    GPIO_InitStruct.GPIO_Pin = LCD_PIN_DB5;
    GPIO_Init(LCD_PORT_DB5, &GPIO_InitStruct);
  
    GPIO_InitStruct.GPIO_Pin = LCD_PIN_DB6;
    GPIO_Init(LCD_PORT_DB6, &GPIO_InitStruct);
  
    GPIO_InitStruct.GPIO_Pin = LCD_PIN_DB7;
    GPIO_Init(LCD_PORT_DB7, &GPIO_InitStruct);
  
    GPIO_ResetBits(LCD_PORT_DB0, LCD_PIN_DB0);
    GPIO_ResetBits(LCD_PORT_DB1, LCD_PIN_DB1);
    GPIO_ResetBits(LCD_PORT_DB2, LCD_PIN_DB2);
    GPIO_ResetBits(LCD_PORT_DB3, LCD_PIN_DB3);
    GPIO_ResetBits(LCD_PORT_DB4, LCD_PIN_DB4);
    GPIO_ResetBits(LCD_PORT_DB5, LCD_PIN_DB5);
    GPIO_ResetBits(LCD_PORT_DB6, LCD_PIN_DB6);
    GPIO_ResetBits(LCD_PORT_DB7, LCD_PIN_DB7);

}

void LCD::writeByte(char byte)
{
    GPIO_ResetBits(LCD_PORT_DB0, LCD_PIN_DB0);
    GPIO_ResetBits(LCD_PORT_DB1, LCD_PIN_DB1);
    GPIO_ResetBits(LCD_PORT_DB2, LCD_PIN_DB2);
    GPIO_ResetBits(LCD_PORT_DB3, LCD_PIN_DB3);
    GPIO_ResetBits(LCD_PORT_DB4, LCD_PIN_DB4);
    GPIO_ResetBits(LCD_PORT_DB5, LCD_PIN_DB5);
    GPIO_ResetBits(LCD_PORT_DB6, LCD_PIN_DB6);
    GPIO_ResetBits(LCD_PORT_DB7, LCD_PIN_DB7);
    
    GPIO_SetBits(LCD_PORT_E1, LCD_PIN_E1); 
    putByte(byte);
    GPIO_ResetBits(LCD_PORT_E1, LCD_PIN_E1); 
}

void LCD::writeData(char byte)
{
    GPIO_SetBits(LCD_PORT_A0, LCD_PIN_A0);
    writeByte(byte);
}

void LCD::writeInst(char byte)
{
    GPIO_ResetBits(LCD_PORT_A0, LCD_PIN_A0);
    writeByte(byte);
}

void LCD::go(char address, char line)
{
    switch(line)
  {
    case 1:
      writeInst(0x80|address); 
    break;
   
    case 2: 
      writeInst(0xC0|address); 
    break;
   
    default :
    break;
  }
  delayUs(1000);
}

void LCD::clearScr()
{
    writeInst(1);
    delayUs(1000);
}
