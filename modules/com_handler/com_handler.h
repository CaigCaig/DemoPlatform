#ifndef __COM_HANDLER_H
#define __COM_HANDLER_H

#define MH_ID                   ((uint8_t)0xa9)    // ������ ���� (�����������������) ������ UART ��� MasterHood
#define H1_ID                   ((uint8_t)0xaa)    // ������ ���� (�����������������) ������ UART ��� Hood_1
#define H2_ID                   ((uint8_t)0xab)    // ������ ���� (�����������������) ������ UART ��� Hood_2
#define CH_ID                   ((uint8_t)0xac)    // ������ ���� (�����������������) ������ UART ��� Charger
#define CAM_ID                  ((uint8_t)0xad)    // ������ ���� (�����������������) ������ UART ��� ����������
#define MB_ID                   ((uint8_t)0xae)    // ������ ���� (�����������������) ������ UART ��� �������� �����
#define ADCH_ID                 ((uint8_t)0xa6)    // ������ ���� (�����������������) ������ UART ��� Analog Discharger
#define ACHA_ID                 ((uint8_t)0xa7)    // ������ ���� (�����������������) ������ UART ��� Analog Charger A
#define ACHB_ID                 ((uint8_t)0xa8)    // ������ ���� (�����������������) ������ UART ��� Analog Charger B

#define TIMEOUT_CAM             1000    // ������� ��� ������ �� ����� � �������������
#define TIMEOUT_ACH             1000    // ������� ��� ������ �� ���� ���������� ����������
#define TIMEOUT_ADCH            1000    // ������� ��� ������ �� ����� ����������� ����������
#define TIMEOUT_HOOD            1000    // ������� ��� ������ �� ����� Hood

#define TIME_YEAR               6
#define TIME_MONTH              8
#define TIME_DAY                9
#define TIME_HOUR               10
#define TIME_MIN                11
#define TIME_SEC                12
#define SETLAT_NUM              6
#define SETLAT_LET              10
#define SETLAT_NUM              6
#define SETLAT_LET              10
#define SETLON_NUM              6
#define SETLON_LET              10
#define COM_OPT_SM_TYPE_OFFSET  6
#define COM_OPT_SM_DATA_OFFSET  7

typedef enum
{
  ACCEPTED = 0,
  NOT_ACCEPTED,
  ALREADY_RUNNING,
} Acceptance;

typedef enum 
{
  COM_OK       = 0x00U,
  COM_ERROR    = 0x01U,
  COM_BUSY     = 0x02U,
  COM_TIMEOUT  = 0x03U
} COM_StatusTypeDef;


// 1-BUSY/0-READY
#define RS485_BUSY              ((uint32_t)0x00000001)

void Com_Handler(int conn, char *com_buffer, uint16_t com_buf_len);
COM_StatusTypeDef stoui(char *s, uint16_t *value, uint8_t len);

void vStepMotors(void *params);
void vTestSMA(void *params);

#endif // __COM_HANDLER_H