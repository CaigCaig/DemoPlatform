#ifndef __ADC_H
#define __ADC_H

#define adc_scl_Pin GPIO_Pin_8
#define adc_scl_GPIO_Port GPIOB
#define adc_sda_Pin GPIO_Pin_9
#define adc_sda_GPIO_Port GPIOB

#define MCP3422_I2C     I2C1
#define MCP3422_DEVICE_WRITE_ADDRESS    ((uint8_t)0xD0)
#define MCP3422_DEVICE_READ_ADDRESS     ((uint8_t)0xD1)
#define MCP3422_DMA_Channel     DMA_Channel_1
#define MCP3422_DMA_Stream      DMA1_Stream0

/*
RDY: Ready Bit
This bit is the data ready flag. In read mode, this bit indicates if the output register has been updated
with a latest conversion result. In One-Shot Conversion mode, writing this bit to У1Ф initiates a new
conversion.
Reading RDY bit with the read command:
1 = Output register has not been updated
0 = Output register has been updated with the latest conversion result
Writing RDY bit with the write command:
Continuous Conversion mode: No effect
One-Shot Conversion mode:
1 = Initiate a new conversion
0 = No effect
*/
#define MCP3422_START_CONV    ((uint8_t)0x80)

/*
C1-C0: Channel Selection Bits
00 = Select Channel 1 (Default)
01 = Select Channel 2
10 = Select Channel 3 (MCP3424 only, treated as У00Ф by the MCP3422/MCP3423)
11 = Select Channel 4 (MCP3424 only, treated as У01Ф by the MCP3422/MCP3423)
*/
#define MCP3422_CH1     ((uint8_t)0x00)
#define MCP3422_CH2     ((uint8_t)0x20)
#define MCP3424_CH3     ((uint8_t)0x40)
#define MCP3424_CH4     ((uint8_t)0x60)
#define MCP3422_ADDR    ((uint8_t)0x00)
#define MCP3422_CONF    ((uint8_t)0x01)

/*
O/C: Conversion Mode Bit
1 = Continuous Conversion Mode (Default). The device performs data conversions continuously
0 = One-Shot Conversion Mode. The device performs a single conversion and enters a low power
standby mode until it receives another write or read command
*/
#define MCP3422_CONT_CONV       ((uint8_t)0x10)
#define MCP3422_ONE_CONV        ((uint8_t)0x00)

/*
S1-S0: Sample Rate Selection Bit
00 = 240 SPS (12 bits) (Default)
01 = 60 SPS (14 bits)
10 = 15 SPS (16 bits)
11 = 3.75 SPS (18 bits)
*/
#define MCP3422_240SPS  ((uint8_t)0x00)
#define MCP3422_60SPS   ((uint8_t)0x04)
#define MCP3422_15SPS   ((uint8_t)0x08)
#define MCP3422_4SPS    ((uint8_t)0x0C)
#define MCP3422_12BIT   MCP3422_240SPS
#define MCP3422_14BIT   MCP3422_60SPS
#define MCP3422_16BIT   MCP3422_15SPS
#define MCP3422_18BIT   MCP3422_4SPS

/*
G1-G0: PGA Gain Selection Bits
00 = x1 (Default)
01 = x2
10 = x4
11 = x8
*/
#define MCP3422_GAIN1   ((uint8_t)0x00)
#define MCP3422_GAIN2   ((uint8_t)0x01)
#define MCP3422_GAIN4   ((uint8_t)0x02)
#define MCP3422_GAIN8   ((uint8_t)0x03)

#define MCP3422_TX_LENGHT       ((uint8_t)2)

#define MCP3422_DATA12_LENGHT   ((uint8_t)3)
#define MCP3422_DATA14_LENGHT   MCP3422_DATA12_LENGHT
#define MCP3422_DATA16_LENGHT   MCP3422_DATA12_LENGHT
#define MCP3422_DATA18_LENGHT   ((uint8_t)4)

#define TIM7_TIMEOUT    ((uint8_t)1)
#define DMA_RECEIVED    ((uint8_t)0x01)
#define DMA_NO_RECEIVED ((uint8_t)0)
#define DMA_TRANSMITTED ((uint8_t)0x02)
#define DMA_NO_TRANSMITTED (uint8_t)0)
#define TIMEOUT_BUS     ((uint16_t)0x0040) // таймаут освобождени€ шины I2C
#define TIMEOUT_START   ((uint16_t)0x0200) // таймаут дл€ подтверждени€ отправки команды START
#define TIMEOUT_STOP    ((uint16_t)0x0200) // таймаут дл€ подтверждени€ отправки команды STOP
#define TIMEOUT_ADDR    ((uint16_t)0x0800) // таймаут дл€ подтверждени€ отправки адреса MCP3422
#define TIMEOUT_DATA_TX ((uint16_t)0x0800) // таймаут дл€ подтверждени€ отправки данных конфигурационного регистра в MCP3422
#define TIMEOUT_DATA_RX ((uint16_t)0x0800) // таймаут дл€ подтверждени€ получени€ данных от MCP3422
#define MCP3422_FAIL_LIMIT      ((uint8_t)100)
#define MCP3422_FAIL    ((uint8_t)1)

#define BAT_QUANTITY    ((uint8_t)2)    // количество аккумул€торных батарей
#define MCP3422_BIAS    ((uint16_t)1250)        // напр€жение смещени€ ј÷ѕ, м¬
#define MCP3422_VLSB    ((uint16_t)15625)       // напр€жение единицы младшего разр€да ј÷ѕ, н¬
#define C_DIV           (float)41     // коэффициент делени€ напр€жени€ аккумул€тора
//#define C_DIV           (float)17.5     // коэффициент делени€ ќ”

typedef struct
{
  int32_t       V_PS1;
  int32_t       V_PS2;
  int32_t       VbatA;
  int32_t       VbatB;
} MCP3424_Meas_TypeDef;

void ADC_Init(void);
void Load_Net_Config(void);
ErrorStatus Save_Net_Config(void);
ErrorStatus ADC_Config(void);
ErrorStatus ADC_Set_Channel(uint8_t channel);
ErrorStatus Read_ADC(uint8_t *buffer);
void vADC(void *params);
void Timer_I2C(uint16_t timer_us);
ErrorStatus I2C_GetFlagStatus_timer(I2C_TypeDef* I2Cx, uint32_t I2C_FLAG, uint16_t timer);
ErrorStatus I2C_CheckEvent_timer(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT, uint16_t timer);
ErrorStatus I2C_DMARead_timer(I2C_TypeDef* I2Cx, uint32_t DMA_Channel_x, uint32_t buf_len, uint8_t *read_addr, uint16_t timer);
ErrorStatus I2C_DMAWrite_timer(I2C_TypeDef* I2Cx, uint32_t DMA_Channel_x, uint32_t buf_len, uint8_t *write_addr, uint16_t timer);
//void DMA1_Stream2_IRQHandler(void);

#endif // __ADC_H