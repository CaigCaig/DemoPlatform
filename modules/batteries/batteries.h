#ifndef __BATTERIES_H
#define __BATTERIES_H

#define power36v_Pin GPIO_Pin_9
#define power36v_GPIO_Port GPIOE

/*typedef struct
{

  uint16_t C1;          // Ток основного заряда (например если ток 53 Ампера, то передать нужно значение 530)
  uint16_t V1;          // Конечное напряжение основного заряда (например если напряжение 31,4 Вольта, то передать нужно значение 314)
  uint16_t TC1;         // Время основного заряда
  uint16_t DTC11;       // Отклонение времени основного заряда+
  uint16_t DTC12;       // Отклонение времени основного заряда-
  uint16_t C2;          // Ток дополнительного заряда (например если ток 5,3 Ампера, то передать нужно значение 53)
  uint16_t TC2;         // Время дополнительного заряда
  uint16_t TW;          // Время балансировки дистиллированной водой
  uint16_t MW;          // Максимальный объём на ячейку
  uint16_t PV;          // Минимальное напряжение "поляризации" (например если напряжение 21 Вольт, то передать нужно значение 210)
  uint16_t C3;          // Ток специального теста заряда  (например если ток 5,3 Ампера, то передать нужно значение 53)
  uint16_t TC3;         // Время специального теста заряда
  uint16_t TCD;         // Время между зарядом и разрядом
  uint16_t D1;          // Ток разряда (например если ток 53 Ампера, то передать нужно значение 530)
  uint16_t TD1;         // Время разряда для определения емкости
  uint16_t VR;          // Напряжение ячейки шунтирования 1 Ом (например если напряжение 1 Вольт, то передать нужно значение 10)
  uint16_t VS;          // Напряжение ячейки шунтирования 0 Ом (например если напряжение 0,5 Вольт, то передать нужно значение 5)
  uint16_t TS;          // Время шунтирования ячеек
  uint16_t DTS1;        // Отклонение времени шунтирования ячеек+
  uint16_t DTS2;        // Отклонение времени шунтирования ячеек-

}BatteryTypeDef;*/

typedef struct
{

  uint16_t MCHI;        // I(A)*10 Ток основного заряда (например если ток 53 Ампера, то передать нужно значение 530)
  uint16_t MCHV;        // U(V)*10 Конечное напряжение основного заряда (например если напряжение 31,4 Вольта, то передать нужно значение 314)
  uint16_t MCHT;        // min Время основного заряда
  uint16_t DMCHT1;      // min Отклонение времени основного заряда+
  uint16_t DMCHT2;      // min Отклонение времени основного заряда-
  uint16_t OCHI;        // I(A)*10 Ток дополнительного заряда (например если ток 5,3 Ампера, то передать нужно значение 53)
  uint16_t OCHT;        // min Время дополнительного заряда
  uint16_t ECT;         // min Время балансировки дистиллированной водой
  uint16_t ECV;         // ml Максимальный объём на ячейку
  uint16_t STI;         // I(A)*10 Ток специального теста заряда  (например если ток 5,3 Ампера, то передать нужно значение 53)
  uint16_t STT;         // min Время специального теста заряда
  uint16_t IDI;         // I(A)*10 Ток начального разряда (например если ток 53 Ампера, то передать нужно значение 530)
  uint16_t IDV;         // U(V)*10 Конечное напряжение начального разряда (например если напряжение 20 Вольт, то передать нужно значение 200)
  uint16_t CTI;         // I(A)*10 Ток разряда теста ёмкости (например если ток 53 Ампера, то передать нужно значение 530)
  uint16_t CTT;         // sec Время разряда для определения емкости
  uint16_t RDVR;        // U(mV) Напряжение ячейки шунтирования 1 Ом (например если напряжение 1 Вольт, то передать нужно значение 1000)
  uint16_t RDVS;        // U(mV) Напряжение ячейки шунтирования 0 Ом (например если напряжение 0,5 Вольт, то передать нужно значение 500)
  uint16_t RDT;         // min Время шунтирования ячеек
  uint16_t RDT1;        // min Отклонение времени шунтирования ячеек+
  uint16_t RDT2;        // min Отклонение времени шунтирования ячеек-
  uint16_t PV;          // U(V)*10 Минимальное напряжение "поляризации" (например если напряжение 21 Вольт, то передать нужно значение 210)
  uint16_t TCD;         // min Время между зарядом и разрядом
  uint16_t VD;          // U(V)*10 Максимальный уровень разряженного аккумулятора (например если напряжение 20 Вольт, то передать нужно значение 200)
  uint16_t VCMAX;       // U(mV) Максимальный уровень напряжения ячейки (например если напряжение 1.57 Вольт, то передать нужно значение 1570)

}BatteryTypeDef;

typedef struct
{

  float MCHI;        // I(A) Ток основного заряда
  float MCHV;        // U(V) Конечное напряжение основного заряда
  float MCHT;        // min Время основного заряда
  float DMCHT1;      // min Отклонение времени основного заряда+
  float DMCHT2;      // min Отклонение времени основного заряда-
  float OCHI;        // I(A) Ток дополнительного заряда
  float OCHT;        // min Время дополнительного заряда
  float ECT;         // min Время балансировки дистиллированной водой
  float ECV;         // ml Максимальный объём на ячейку
  float STI;         // I(A) Ток специального теста заряда
  float STT;         // min Время специального теста заряда
  float IDI;         // I(A) Ток начального разряда
  float IDV;         // U(V) Конечное напряжение начального разряда
  float CTI;         // I(A) Ток разряда теста ёмкости
  float CTT;         // sec Время разряда для определения емкости
  float RDVR;        // U(V) Напряжение ячейки шунтирования 1 Ом
  float RDVS;        // U(V) Напряжение ячейки шунтирования 0 Ом
  float RDT;         // min Время шунтирования ячеек
  float RDT1;        // min Отклонение времени шунтирования ячеек+
  float RDT2;        // min Отклонение времени шунтирования ячеек-
  float PV;          // U(V)0 Минимальное напряжение "поляризации"
  float TCD;         // min Время между зарядом и разрядом
  float VD;          // U(V)0 Максимальный уровень разряженного аккумулятора
  float VCMAX;       // U(V) Максимальный уровень напряжения ячейки

}BatteryTypeDef_f;

#define BATTERY_OPTIONS         ((uint8_t)24)
#define BATTERY_OPTIONS_OFFSET  ((uint8_t)3)

#define MCHI_pos        ((uint32_t)0x00000001)
#define MCHV_pos        ((uint32_t)0x00000002)
#define MCHT_pos        ((uint32_t)0x00000004)
#define DMCHT1_pos      ((uint32_t)0x00000008)
#define DMCHT2_pos      ((uint32_t)0x00000010)
#define OCHI_pos        ((uint32_t)0x00000020)
#define OCHT_pos        ((uint32_t)0x00000040)
#define ECT_pos         ((uint32_t)0x00000080)
#define ECV_pos         ((uint32_t)0x00000100)
#define STI_pos         ((uint32_t)0x00000200)
#define STT_pos         ((uint32_t)0x00000400)
#define IDI_pos         ((uint32_t)0x00000800)
#define IDV_pos         ((uint32_t)0x00001000)
#define CTI_pos         ((uint32_t)0x00002000)
#define CTT_pos         ((uint32_t)0x00004000)
#define RDVR_pos        ((uint32_t)0x00008000)
#define RDVS_pos        ((uint32_t)0x00010000)
#define RDT_pos         ((uint32_t)0x00020000)
#define RDT1_pos        ((uint32_t)0x00040000)
#define RDT2_pos        ((uint32_t)0x00080000)
#define PV_pos          ((uint32_t)0x00100000)
#define TCD_pos         ((uint32_t)0x00200000)
#define VD_pos          ((uint32_t)0x00400000)
#define VCMAX_pos       ((uint32_t)0x00800000)

#define BATTERY_MCH_OPTIONS     MCHI_pos | MCHV_pos | MCHT_pos | DMCHT1_pos | DMCHT2_pos | VD_pos | VCMAX_pos
#define BATTERY_OCH_OPTIONS     OCHI_pos | OCHT_pos | VCMAX_pos
#define BATTERY_EC_OPTIONS      ECT_pos | ECV_pos
#define BATTERY_ST_OPTIONS      STI_pos | STT_pos
#define BATTERY_ID_OPTIONS      IDI_pos | IDV_pos
#define BATTERY_CT_OPTIONS      CTI_pos | CTT_pos
#define BATTERY_RD_OPTIONS      RDVR_pos | RDVS_pos | RDT_pos | RDT1_pos | RDT2_pos  | PV_pos
#define BATTERY_FULL_OPTIONS    BATTERY_MCH_OPTIONS | \
                                BATTERY_OCH_OPTIONS | \
                                BATTERY_EC_OPTIONS | \
                                BATTERY_ST_OPTIONS | \
                                BATTERY_ID_OPTIONS | \
                                BATTERY_CT_OPTIONS | \
                                BATTERY_RD_OPTIONS | \
                                TCD_pos

typedef enum
{
  MCHI_num = 1,
  MCHV_num,
  MCHT_num,
  DMCHT1_num,
  DMCHT2_num,
  OCHI_num,
  OCHT_num,
  ECT_num,
  ECV_num,
  STI_num,
  STT_num,
  IDI_num,
  IDV_num,
  CTI_num,
  CTT_num,
  RDVR_num,
  RDVS_num,
  RDT_num,
  RDT1_num,
  RDT2_num,
  PV_num,
  TCD_num,
  VD_num,
  VCMAX_num,
} BatteryOptionsNum;

#define BATTERY1                ((uint8_t)1)    // номер аккумулятора 1
#define BATTERY2                ((uint8_t)2)    // номер аккумулятора 2

// нумерация тестов батарей
typedef enum
{
  T_FULL_SERVICE = 0,
  T_MAIN_CHARGE,
  T_OVERCHARGE,
  T_ELECTROLYTE_CHECK,
  T_SPECIAL_TEST,
  T_INITIAL_DISCHARGE,
  T_CAPACITY_TEST,
  T_RESIDUAL_DISCHARGE,
} BATTERY_TESTS;

typedef struct
{
  BATTERY_TESTS BATTERY_A;
  BATTERY_TESTS BATTERY_B;
} BatTest_TypeDef;

typedef BatTest_TypeDef BatTest_t;

#define FULL_SERVICE_pos        ((uint32_t)0x00000001)
#define MAIN_CHARGE_pos         ((uint32_t)0x00000002)
#define OVERCHARGE_pos          ((uint32_t)0x00000004)
#define ELECTROLYTE_CHECK_pos   ((uint32_t)0x00000008)
#define SPECIAL_TEST_pos        ((uint32_t)0x00000010)
#define INITIAL_DISCHARGE_pos   ((uint32_t)0x00000020)
#define CAPACITY_TEST_pos       ((uint32_t)0x00000040)
#define RESIDUAL_DISCHARGE_pos  ((uint32_t)0x00000080)

uint8_t Full_Service(void);
uint8_t Main_Charge(void);
uint8_t Overcharge(void);
uint8_t Electrolyte_Check(void);
uint8_t Special_Test(void);
uint8_t Initial_Discharge(void);
uint8_t Capacity_Test(void);
uint8_t Resudual_Discharge(void);

void vFULL_SERVICE_1(void *params);
void vMAIN_CHARGE_1(void *params);
void vOVERCHARGE_1(void *params);
void vELECTROLYTE_CHECK_1(void *params);
void vSPECIAL_TEST_1(void *params);
void vINITIAL_DISCHARGE_1(void *params);
void vCAPACITY_TEST_1(void *params);
void vRESIDUAL_DISCHARGE_1(void *params);

void vFULL_SERVICE_2(void *params);
void vMAIN_CHARGE_2(void *params);
void vOVERCHARGE_2(void *params);
void vELECTROLYTE_CHECK_2(void *params);
void vSPECIAL_TEST_2(void *params);
void vINITIAL_DISCHARGE_2(void *params);
void vCAPACITY_TEST_2(void *params);
void vRESIDUAL_DISCHARGE_2(void *params);

//void Com_Handler(int conn, char *com_buffer, uint16_t com_buf_len);

#endif // __BATTERIES_H