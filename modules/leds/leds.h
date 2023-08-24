#ifndef __LEDS_H
#define __LEDS_H

//#ifdef USE_LEDS

typedef struct
{

  uint8_t Hour;         // часы
  uint8_t Min;          // минуты
  uint8_t Sec;          // секунды
//  uint16_t mSec;        // миллисекунды
  uint8_t State;        // "0"- часы не тикают, "1"- тикают
  uint8_t Mode;         // режим часов "0" или таймера "1"
}WatchTypeDef;

typedef enum {WATCH = 0, TIMER = !WATCH} WatchMode;
typedef enum {STOP = 0, START = !STOP} WatchState;

//#define led_nOE1_Pin GPIO_Pin_2
//#define led_nOE1_GPIO_Port GPIOH
//#define led_nOE2_Pin GPIO_Pin_3
//#define led_nOE2_GPIO_Port GPIOH
//#define led_CS1_Pin GPIO_Pin_5
//#define led_CS1_GPIO_Port GPIOH
#define led_upd_Pin GPIO_Pin_14
#define led_upd_GPIO_Port GPIOB
#define B2_Pin GPIO_Pin_10
#define B2_GPIO_Port GPIOD
#define G2_Pin GPIO_Pin_11
#define G2_GPIO_Port GPIOD
#define R2_Pin GPIO_Pin_12
#define R2_GPIO_Port GPIOD
#define polarity2_Pin GPIO_Pin_8
#define polarity2_GPIO_Port GPIOD
//#define led_CS2_Pin GPIO_Pin_6
//#define led_CS2_GPIO_Port GPIOH
#define led_en_Pin GPIO_Pin_8
#define led_en_GPIO_Port GPIOE
#define B1_Pin GPIO_Pin_13
#define B1_GPIO_Port GPIOD
#define G1_Pin GPIO_Pin_14
#define G1_GPIO_Port GPIOD
#define R1_Pin GPIO_Pin_15
#define R1_GPIO_Port GPIOD
#define polarity1_Pin GPIO_Pin_9
#define polarity1_GPIO_Port GPIOD
#define leds_sck_Pin GPIO_Pin_10
#define leds_sck_GPIO_Port GPIOB
#define leds_mosi_Pin GPIO_Pin_15
#define leds_mosi_GPIO_Port GPIOB
#define LED_DMA_Channel DMA_Channel_0

/* Биты регистра таймеров */
#define tim_ms                  ((uint8_t)0x01) // таймер мс
#define tim_us                  ((uint8_t)0x02) // таймер мкс

#define LEDS                    ((uint8_t)10)   // количество семисегментных индикаторов
#define DISPLAYS                ((uint8_t)2)    // количество дисплеев
#define CHARGERS                ((uint8_t)2)    // количество зарядников
#define CHARGER1                ((uint8_t)0)    // номер зарядника 1
#define CHARGER2                ((uint8_t)1)    // номер зарядника 2
#define decode_tablesize        ((uint8_t)16)
#define CHARGE_STATE1           ((uint8_t)0x01) // бит состояния заряда аккумулятора 1
#define CHARGE_STATE2           ((uint8_t)0x02) // бит состояния заряда аккумулятора 2

void SPI2_Init(void);
void ConvDataToLEDarr(uint8_t charger);
void vLEDs(void *params);
void vWatch1(void *params);
void vWatch2(void *params);
void NetStateToLED(void);
void ByteHexToDec(uint8_t hex, uint8_t& dech, uint8_t& dect, uint8_t& deco);
uint8_t CountOnes(uint32_t n);
uint8_t GetNumErrPeriph(uint32_t n);

//#endif
#endif // __LEDS_H