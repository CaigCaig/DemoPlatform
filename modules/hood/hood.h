#ifndef __HOOD_H
#define __HOOD_H

//#define HOOD_TX_LEN  ((uint32_t)6)
//#define HOOD_RX_LEN  ((uint32_t)6)

/* ������ ������ ��� Hood*/

#define PUSH_TO_CELL            0x02    // ������� �� ������������ ����������� ������ � ������
#define PULL_FROM_CELL          0x03    // ������� ������ �������� � ����� �� ������
#define BAT_VOLTAGE             0x05    // ������� ��������� ���������� �������
#define V_CELL                  0x06    // ������� ��������� ���������� �� ������
#define V_N_CELL                0x08    // ������� ��������� ���������� �� ������� �� 1 �� n
#define SHUNT_1OHM_ON           0x0A    // ������� ����������� ����� 1 �� � ������ n
#define SHUNT_0OHM_ON           0x0B    // ������� ����������� ����� 0 �� � ������ n
#define SHUNT_1OHM_OFF          0x0C    // ������� ���������� ����� 1 �� �� ������ n
#define SHUNT_0OHM_OFF          0x0D    // ������� ���������� ����� 0 �� �� ������ n
#define SHUNT_1OHM_ALL_OFF      0x0E    // ������� ���������� ���� ������ 1 ��
#define SHUNT_0OHM_ALL_OFF      0x0F    // ������� ���������� ���� ������ 0 ��
#define INIT_SM                 0x11    // ������� ������������� ��
#define START_STOP_SM           0x13    // ������� �������/��������� �� source
#define RESIDUAL_DISCHARGE      0x15    // ������� ������� ��������� ����������� �������
#define RD_TIMELEFT             0x16    // ������� ������� ����������� ������� ����������� �������

/* ����� ������ � ������ */
#define L_PUSH_TO_CELL          4
#define L_PULL_FROM_CELL        4
#define L_BAT_VOLTAGE           3
#define L_V_CELL                4
#define L_V_N_CELL              4
#define L_SHUNT_1OHM_ON         4
#define L_SHUNT_0OHM_ON         4
#define L_SHUNT_1OHM_OFF        4
#define L_SHUNT_0OHM_OFF        4
#define L_SHUNT_1OHM_ALL_OFF    3
#define L_SHUNT_0OHM_ALL_OFF    3
#define L_INIT_SM               4
#define L_START_STOP_SM         5
#define L_RESIDUAL_DISCHARGE    5
#define L_RESIDUAL_DISCHARGE_conf       4
#define L_RESIDUAL_DISCHARGE_answ       4
#define L_RD_TIMELEFT           3
#define L_RD_TIMELEFT_answ      5

#endif // ___HOOD_H