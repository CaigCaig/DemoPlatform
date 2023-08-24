#ifndef __LCD_HD44780_H
#define __LCD_HD44780_H

#define LCD_TIMING_DEL          40
#define LCD_CLEAR_SYMBOL        32
#define LCD_WIDTH               16
#define LCD_HEIGHT              2
#define LCD_BUF_SIZE           (LCD_WIDTH * LCD_HEIGHT)

// Команды
#define LCD_CMD_8BIT_P0         0x38
#define LCD_CMD_8BIT_P1         0x3a
#define LCD_CMD_HOME            0x02
#define LCD_CMD_CLEAR           0x01
#define LCD_CMD_DISP_OFF        0x08
#define LCD_CMD_DISP_ON         0x0c
#define LCD_CMD_CURSOR_OFF      0x0c
#define LCD_CMD_CURSOR_FLASH    ( 0x0c | 0x01 )
#define LCD_CMD_CURSOR_UNDER    ( 0x0c | 0x02 )
#define LCD_CMD_CURSOR_BLINK    ( 0x0c | 0x03 )
#define LCD_CMD_CURSOR_LEFT     0x10
#define LCD_CMD_CURSOR_RIGHT    0x14

#define LCD_CMD_SET_ADDR        0x80

// устанавливаетнаправление сдвига курсора и необходимость сдвига экрана
// CURS_DIR - направление сдвига курсора
//              0 - влево
//              1 - вправо
// DISP_SHIFT - сдвиг экрана
//              0 - сдвиг выключен
//              1 - сдвиг включен
#define LCD_CMD_SETMODE( CURS_DIR, DISP_SHIFT )     ( (1<<2) | (CURS_DIR<<1) | DISP_SHIFT )

// Порты
#define LCD_PORT_LED   GPIOA
#define LCD_PORT_A0    GPIOB
#define LCD_PORT_E1    GPIOB
#define LCD_PORT_E2    GPIOB
#define LCD_PORT_RW    GPIOB

#define LCD_PIN_LED    GPIO_Pin_5
#define LCD_PIN_A0     GPIO_Pin_7
#define LCD_PIN_E1     GPIO_Pin_5
#define LCD_PIN_E2     GPIO_Pin_5
#define LCD_PIN_RW     GPIO_Pin_6

#define LCD_PORT_DB0   GPIOC
#define LCD_PORT_DB1   GPIOE
#define LCD_PORT_DB2   GPIOE
#define LCD_PORT_DB3   GPIOE
#define LCD_PORT_DB4   GPIOE
#define LCD_PORT_DB5   GPIOE
#define LCD_PORT_DB6   GPIOE
#define LCD_PORT_DB7   GPIOE

#define LCD_PIN_DB0    GPIO_Pin_13
#define LCD_PIN_DB1    GPIO_Pin_6
#define LCD_PIN_DB2    GPIO_Pin_5
#define LCD_PIN_DB3    GPIO_Pin_4
#define LCD_PIN_DB4    GPIO_Pin_3
#define LCD_PIN_DB5    GPIO_Pin_2
#define LCD_PIN_DB6    GPIO_Pin_1
#define LCD_PIN_DB7    GPIO_Pin_0


typedef struct LcdOptions {
  
  bool exist;
  
  char staticMsg[41];
  char deliveryMsg[41];
  char groupProductsMsg[41];
  char oneProductsMsg[41];
  char codeTimeupMsg[41];
  char codeClosedMsg[41];
  char codeErrorMsg[41];
  char payReqMsg[41];
  char payOkMsg[41];
  char payFailMsg[41];
  char payCanselMsg[41];
  
} LcdOptions;


class LCD {
public:
  LCD();
  void configure();
  
  void setPositin(uint8_t x, uint8_t y);
  void clear();
  void print(const char *str);
  void printNoClear(const char *str);
  void ledOnOff(bool state);  
  
  LcdOptions m_options;
  
  void go(char address, char line);
  void clearScr();

private:

  char buf[LCD_BUF_SIZE];       // Буфер экрана
  uint16_t bufIndex;
  
  void portInit();              // Настройка портов
  void putByte(uint8_t byte);   // Дергает ножками LCD DB0..DB7
  void putData(char data);      // Записать данные
  void putCmd(char cmd);        // Записать команду
  void putChar(char ch);        // Вывод символа на экран через буфер
  void update();                // Обновление экрана
  
  void writeByte(char byte);
  void writeData(char byte);
  void writeInst(char byte);
  
  
  void delayUs(uint16_t us);   // Задержка в микросекундах (цикл)
    
};

extern LCD lcd;



#endif  // __LCD_HD44780_H

