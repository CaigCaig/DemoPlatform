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

typedef enum
{
  ACCEPTED = 0,
  NOT_ACCEPTED,
  ALREADY_RUNNING,
} Acceptance;

// 1-BUSY/0-READY
#define RS485_BUSY              ((uint32_t)0x00000001)



void Com_Handler(int conn, char *com_buffer, uint16_t com_buf_len);

#endif // __COM_HANDLER_H