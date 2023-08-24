#ifndef __BATTERIES_H
#define __BATTERIES_H

#define power36v_Pin GPIO_Pin_9
#define power36v_GPIO_Port GPIOE

/*typedef struct
{

  uint16_t C1;          // ��� ��������� ������ (�������� ���� ��� 53 ������, �� �������� ����� �������� 530)
  uint16_t V1;          // �������� ���������� ��������� ������ (�������� ���� ���������� 31,4 ������, �� �������� ����� �������� 314)
  uint16_t TC1;         // ����� ��������� ������
  uint16_t DTC11;       // ���������� ������� ��������� ������+
  uint16_t DTC12;       // ���������� ������� ��������� ������-
  uint16_t C2;          // ��� ��������������� ������ (�������� ���� ��� 5,3 ������, �� �������� ����� �������� 53)
  uint16_t TC2;         // ����� ��������������� ������
  uint16_t TW;          // ����� ������������ ���������������� �����
  uint16_t MW;          // ������������ ����� �� ������
  uint16_t PV;          // ����������� ���������� "�����������" (�������� ���� ���������� 21 �����, �� �������� ����� �������� 210)
  uint16_t C3;          // ��� ������������ ����� ������  (�������� ���� ��� 5,3 ������, �� �������� ����� �������� 53)
  uint16_t TC3;         // ����� ������������ ����� ������
  uint16_t TCD;         // ����� ����� ������� � ��������
  uint16_t D1;          // ��� ������� (�������� ���� ��� 53 ������, �� �������� ����� �������� 530)
  uint16_t TD1;         // ����� ������� ��� ����������� �������
  uint16_t VR;          // ���������� ������ ������������ 1 �� (�������� ���� ���������� 1 �����, �� �������� ����� �������� 10)
  uint16_t VS;          // ���������� ������ ������������ 0 �� (�������� ���� ���������� 0,5 �����, �� �������� ����� �������� 5)
  uint16_t TS;          // ����� ������������ �����
  uint16_t DTS1;        // ���������� ������� ������������ �����+
  uint16_t DTS2;        // ���������� ������� ������������ �����-

}BatteryTypeDef;*/

typedef struct
{

  uint16_t MCHI;        // I(A)*10 ��� ��������� ������ (�������� ���� ��� 53 ������, �� �������� ����� �������� 530)
  uint16_t MCHV;        // U(V)*10 �������� ���������� ��������� ������ (�������� ���� ���������� 31,4 ������, �� �������� ����� �������� 314)
  uint16_t MCHT;        // min ����� ��������� ������
  uint16_t DMCHT1;      // min ���������� ������� ��������� ������+
  uint16_t DMCHT2;      // min ���������� ������� ��������� ������-
  uint16_t OCHI;        // I(A)*10 ��� ��������������� ������ (�������� ���� ��� 5,3 ������, �� �������� ����� �������� 53)
  uint16_t OCHT;        // min ����� ��������������� ������
  uint16_t ECT;         // min ����� ������������ ���������������� �����
  uint16_t ECV;         // ml ������������ ����� �� ������
  uint16_t STI;         // I(A)*10 ��� ������������ ����� ������  (�������� ���� ��� 5,3 ������, �� �������� ����� �������� 53)
  uint16_t STT;         // min ����� ������������ ����� ������
  uint16_t IDI;         // I(A)*10 ��� ���������� ������� (�������� ���� ��� 53 ������, �� �������� ����� �������� 530)
  uint16_t IDV;         // U(V)*10 �������� ���������� ���������� ������� (�������� ���� ���������� 20 �����, �� �������� ����� �������� 200)
  uint16_t CTI;         // I(A)*10 ��� ������� ����� ������� (�������� ���� ��� 53 ������, �� �������� ����� �������� 530)
  uint16_t CTT;         // sec ����� ������� ��� ����������� �������
  uint16_t RDVR;        // U(mV) ���������� ������ ������������ 1 �� (�������� ���� ���������� 1 �����, �� �������� ����� �������� 1000)
  uint16_t RDVS;        // U(mV) ���������� ������ ������������ 0 �� (�������� ���� ���������� 0,5 �����, �� �������� ����� �������� 500)
  uint16_t RDT;         // min ����� ������������ �����
  uint16_t RDT1;        // min ���������� ������� ������������ �����+
  uint16_t RDT2;        // min ���������� ������� ������������ �����-
  uint16_t PV;          // U(V)*10 ����������� ���������� "�����������" (�������� ���� ���������� 21 �����, �� �������� ����� �������� 210)
  uint16_t TCD;         // min ����� ����� ������� � ��������
  uint16_t VD;          // U(V)*10 ������������ ������� ������������ ������������ (�������� ���� ���������� 20 �����, �� �������� ����� �������� 200)
  uint16_t VCMAX;       // U(mV) ������������ ������� ���������� ������ (�������� ���� ���������� 1.57 �����, �� �������� ����� �������� 1570)

}BatteryTypeDef;

typedef struct
{

  float MCHI;        // I(A) ��� ��������� ������
  float MCHV;        // U(V) �������� ���������� ��������� ������
  float MCHT;        // min ����� ��������� ������
  float DMCHT1;      // min ���������� ������� ��������� ������+
  float DMCHT2;      // min ���������� ������� ��������� ������-
  float OCHI;        // I(A) ��� ��������������� ������
  float OCHT;        // min ����� ��������������� ������
  float ECT;         // min ����� ������������ ���������������� �����
  float ECV;         // ml ������������ ����� �� ������
  float STI;         // I(A) ��� ������������ ����� ������
  float STT;         // min ����� ������������ ����� ������
  float IDI;         // I(A) ��� ���������� �������
  float IDV;         // U(V) �������� ���������� ���������� �������
  float CTI;         // I(A) ��� ������� ����� �������
  float CTT;         // sec ����� ������� ��� ����������� �������
  float RDVR;        // U(V) ���������� ������ ������������ 1 ��
  float RDVS;        // U(V) ���������� ������ ������������ 0 ��
  float RDT;         // min ����� ������������ �����
  float RDT1;        // min ���������� ������� ������������ �����+
  float RDT2;        // min ���������� ������� ������������ �����-
  float PV;          // U(V)0 ����������� ���������� "�����������"
  float TCD;         // min ����� ����� ������� � ��������
  float VD;          // U(V)0 ������������ ������� ������������ ������������
  float VCMAX;       // U(V) ������������ ������� ���������� ������

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

#define BATTERY1                ((uint8_t)1)    // ����� ������������ 1
#define BATTERY2                ((uint8_t)2)    // ����� ������������ 2

// ��������� ������ �������
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