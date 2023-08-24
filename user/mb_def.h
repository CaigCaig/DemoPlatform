/* ������ ������ ��� ����������� ����� (���������� AE)*/
#define SET_NET_OPTIONS                 ((uint8_t)0x01)    // ������� ��������� ���������� ���� (DHCP ���/����, IP, MASK, GATE)
#define SET_BATTERY_OPTIONS             ((uint8_t)0x02)    // ������� ��������� ���������� ������������ ������� � ������������ �����������
#define SET_BATTERY_SERVICE             ((uint8_t)0x03)    // ������� ��������� ���������� ������������ ������� ������������ ����
#define START_BATTERY_SERVICE           ((uint8_t)0x04)    // ������� ������ ������������ �������
#define BAT_VOLTAGE                     ((uint8_t)0x05)    // ������� ��������� ���������� �������
#define FINALIZE_BATTERY_SERVICE        ((uint8_t)0x06)    // ������� ���������� ������������ �������
#define FIX_CHARGER_CHANNEL_ON          ((uint8_t)0x07)    // ������� ��������� ������� ���������
#define FIX_CHARGER_CHANNEL_OFF         ((uint8_t)0x08)    // ������� ���������� ������� ���������
#define FIX_CHARGER_STATE               ((uint8_t)0x09)    // ������� ������ ��������� ������� ���������
#define PING                            ((uint8_t)0x14)    // ������� �������� ��������� ����������

/* ����� ������ ��� ����������� ����� � ������ */
#define L_SET_NET_OPTIONS               ((uint8_t)15)
#define L_SET_NET_OPTIONS_ANSW          ((uint8_t)3)
#define L_SET_BATTERY_OPTIONS           ((uint8_t)75)
#define L_SET_BATTERY_OPTIONS_CONF      ((uint8_t)4)
#define L_SET_BATTERY_SERVICE           ((uint8_t)4)
#define L_SET_BATTERY_SERVICE_CONF      ((uint8_t)4)
#define L_START_BATTERY_SERVICE         ((uint8_t)4)
#define L_START_BATTERY_SERVICE_CONF    ((uint8_t)5)
#define L_BAT_VOLTAGE                   ((uint8_t)3)
#define L_BAT_VOLTAGE_ANSW              ((uint8_t)6)
#define L_FINALIZE_BATTERY_SERVICE      ((uint8_t)3)
#define L_FINALIZE_BATTERY_SERVICE_CONF ((uint8_t)4)
#define L_FINALIZE_BATTERY_SERVICE_ANSW ((uint8_t)4)
#define L_PING                          ((uint8_t)2)

/* ���� ���������� �������� ������ com_mb_sr */
#define COM_SET_NET_OPTIONS             ((uint32_t)0x00000001)    // ������� ��������� ���������� ���� (DHCP ���/����, IP, MASK, GATE)
#define COM_SET_BATTERY_OPTIONS         ((uint32_t)0x00000002)    // ������� ��������� ���������� ������������ ������� � ������������ �����������
#define COM_SET_BATTERY_SERVICE         ((uint32_t)0x00000004)    // ������� ��������� ���������� ������������ ������� ������������ ����
#define COM_START_BATTERY_SERVICE       ((uint32_t)0x00000008)    // ������� ������ ������������ �������
#define COM_BAT_VOLTAGE                 ((uint32_t)0x00000010)    // ������� ��������� ���������� �������

/* ������ ������ ��� ����� (���������� A9, AA, AB)*/
#define water_5ml               ((uint8_t)0x01) // ������� ������ 5 �� ��������
#define push_to_cell            ((uint8_t(0x02) // ������� �� ������������ ����������� ������ � ������
#define pull_from_cell          ((uint8_t(0x03) // ������� ������ �������� � ����� �� ������
#define push_to_drainage        ((uint8_t(0x04) // ������� �� ������������ ����������� ������ � ������
#define bat_voltage             ((uint8_t(0x05) // ������� ��������� ���������� �������
#define v_cell                  ((uint8_t(0x06) // ������� ��������� ���������� �� ������
#define t_cell                  ((uint8_t(0x07) // ������� ��������� ����������� �� ������
#define v_n_cell                ((uint8_t(0x08) // ������� ��������� ���������� �� ������� �� 1 �� n
#define t_n_cell                ((uint8_t(0x09) // ������� ��������� ����������� �� ������� �� 1 �� n
#define shunt_1ohm_on           ((uint8_t(0x0A) // ������� ����������� ����� 1 �� � ������ n
#define shunt_0ohm_on           ((uint8_t(0x0B) // ������� ����������� ����� 0 �� � ������ n
#define shunt_1ohm_off          ((uint8_t(0x0C) // ������� ���������� ����� 1 �� �� ������ n
#define shunt_0ohm_off          ((uint8_t(0x0D) // ������� ���������� ����� 0 �� �� ������ n
#define shunt_1ohm_all_off      ((uint8_t(0x0E) // ������� ���������� ���� ������ 1 ��
#define shunt_0ohm_all_off      ((uint8_t(0x0F) // ������� ���������� ���� ������ 0 ��
#define bung_pressure           ((uint8_t(0x10) // ������� ����� ������ ��� ���������
#define init_sm                 ((uint8_t(0x11) // ������� ������������� ��
#define switch_bl               ((uint8_t)0x12) // ������� ���/���� ���������
#define start_stop_sm           ((uint8_t)0x13) // ������� �������/��������� �� source
#define hello                   ((uint8_t)0x14) // ������� "����"
//#define RESIDUAL_DISCHARGE      ((uint8_t)0x15) // ������� ������� ��������� ����������� �������
//#define RD_TIMELEFT             ((uint8_t)0x16) // ������� ������� ����������� ������� ����������� ������

/* ����� ������ � ������ */
#define l_water_5ml             ((uint8_t)4)
#define l_push_to_cell          ((uint8_t)4)
#define l_pull_from_cell        ((uint8_t)4)
#define l_push_to_drainage      ((uint8_t)3)
#define l_bat_voltage           ((uint8_t)3)
#define l_bat_voltage_answ      ((uint8_t)6)
#define l_v_cell                ((uint8_t)4)
#define l_t_cell                ((uint8_t)4)
#define l_v_n_cell              ((uint8_t)4)
#define l_t_n_cell              ((uint8_t)4)
#define l_shunt_1ohm_on         ((uint8_t)4)
#define l_shunt_0ohm_on         ((uint8_t)4)
#define l_shunt_1ohm_off        ((uint8_t)4)
#define l_shunt_0ohm_off        ((uint8_t)4)
#define l_shunt_1ohm_all_off    ((uint8_t)3)
#define l_shunt_0ohm_all_off    ((uint8_t)3)
#define l_bung_pressure         ((uint8_t)5)
#define l_init_sm               ((uint8_t)4)
#define l_switch_bl             ((uint8_t)4)
#define l_start_stop_sm         ((uint8_t)5)
#define l_hello                 ((uint8_t)3)
#define L_RESIDUAL_DISCHARGE    ((uint8_t)5)
#define L_RESIDUAL_DISCHARGE_CONF       ((uint8_t)4)
#define L_RESIDUAL_DISCHARGE_ANSW       ((uint8_t)4)
#define L_RD_TIMELEFT           ((uint8_t)3)
#define L_RD_TIMELEFT_ANSW      ((uint8_t)6)

/* ���� ���������� �������� ������ com_sr_h1, com_sr_h2 */
#define com_water_5ml                   ((uint32_t)0x00000001)  // ������� ������ 5 �� ��������
#define com_push_to_cell                ((uint32_t)0x00000002)  // ������� �� ������������ ����������� ������ � ������
#define com_pull_from_cell              ((uint32_t)0x00000004)  // ������� ������ �������� � ����� �� ������
#define com_push_to_drainage            ((uint32_t)0x00000008)  // ������� �� ������������ ����������� ������ � ������
#define com_bat_voltage                 ((uint32_t)0x00000010)  // ������� ��������� ���������� �������
#define com_v_cell                      ((uint32_t)0x00000020)  // ������� ��������� ���������� �� ������
#define com_t_cell                      ((uint32_t)0x00000040)  // ������� ��������� ����������� �� ������
#define com_v_n_cell                    ((uint32_t)0x00000080)  // ������� ��������� ���������� �� ������� �� 1 �� n
#define com_t_n_cell                    ((uint32_t)0x00000100)  // ������� ��������� ����������� �� ������� �� 1 �� n
#define com_shunt_1ohm_on               ((uint32_t)0x00000200)  // ������� ����������� ����� 1 �� � ������ n
#define com_shunt_0ohm_on               ((uint32_t)0x00000400)  // ������� ����������� ����� 0 �� � ������ n
#define com_shunt_1ohm_off              ((uint32_t)0x00000800)  // ������� ���������� ����� 1 �� �� ������ n
#define com_shunt_0ohm_off              ((uint32_t)0x00001000)  // ������� ���������� ����� 0 �� �� ������ n
#define com_shunt_1ohm_all_off          ((uint32_t)0x00002000)  // ������� ���������� ���� ������ 1 ��
#define com_shunt_0ohm_all_off          ((uint32_t)0x00004000)  // ������� ���������� ���� ������ 0 ��
#define com_bung_pressure               ((uint32_t)0x00008000)  // ������� ����� ������ ��� ���������
#define com_init_sm                     ((uint32_t)0x00010000)  // ������� ������������� ��
#define com_switch_bl                   ((uint32_t)0x00020000)  // ������� ���/���� ���������
#define com_start_stop_sm               ((uint32_t)0x00040000)  // ������� �������/��������� �� source
#define com_hello                       ((uint32_t)0x00080000)  // ������� ping
#define COM_RESIDUAL_DISCHARGE          ((uint32_t)0x00100000)  // ������� ������� ��������� ����������� �������
#define COM_RD_TIMELEFT                 ((uint32_t)0x00200000)  // ������� ������� ����������� ������� ����������� ������

/* ������ ������ ��� ���������� (���������� AD)*/
#define CAM_ACT                         ((uint8_t)0x01)         // ������� ��������� �����������
#define CAM_WATCH_WIN                   ((uint8_t)0x03)         // ������� ��������� ���� ���������� �����������
#define CAM_RD_REG                      ((uint8_t)0x08)         // ������� ������ ��������
#define CAM_WR_REG                      ((uint8_t)0x09)         // ������� ������ � �������
#define CAM_GET_VOLUME                  ((uint8_t)0x0C)         // ������� ����������� ������ ��������
#define CAM_GET_DENSITY                 ((uint8_t)0x0E)         // ������� ����������� ��������� ��������
#define CAM_FRAME_TO_MEM                ((uint8_t)0x0F)         // ������� ������ ����� � ������
#define CAM_GET_CUR_WIN                 ((uint8_t)0x10)         // ������� �������� ������ ������� ����� �� �����������
#define CAM_DEF_LABELS                  ((uint8_t)0x11)         // ������� ����������� ��������� �����

#define NUM_COM_CAM                     ((uint8_t)17)           // ����� ���������� ������ ��� ����������

/* ���� ���������� �������� ������ com_sr_cam */
#define COM_CAM_ACT                     ((uint32_t)0x00000001)
#define COM_CAM_WATCH_WIN               ((uint32_t)0x00000004)
#define COM_CAM_RD_REG                  ((uint32_t)0x00000080)
#define COM_CAM_WR_REG                  ((uint32_t)0x00000100)
#define COM_CAM_GET_VOLUME              ((uint32_t)0x00000800)
#define COM_CAM_GET_DENSITY             ((uint32_t)0x00002000)
#define COM_CAM_FRAME_TO_MEM            ((uint32_t)0x00004000)
#define COM_CAM_GET_CUR_WIN             ((uint32_t)0x00008000)
#define COM_CAM_DEF_LABELS              ((uint32_t)0x00010000)

/* ����� ������ � ������ */
#define L_CAM_ACT                       ((uint8_t)4)
#define L_CAM_WATCH_WIN                 ((uint8_t)12)
#define L_CAM_WATCH_WIN_CONF            ((uint8_t)4)
#define L_CAM_WATCH_WIN_ANSW            ((uint8_t)4)
#define L_CAM_RD_REG                    ((uint8_t)5)
#define L_CAM_RD_REG_CONF               ((uint8_t)4)
#define L_CAM_RD_REG_ANSW               ((uint8_t)7)
#define L_CAM_WR_REG                    ((uint8_t)5)
#define L_CAM_WR_REG_CONF               ((uint8_t)4)
#define L_CAM_WR_REG_ANSW               ((uint8_t)7)
#define L_CAM_GET_VOLUME                ((uint8_t)4)
#define L_CAM_GET_VOLUME_CONF           ((uint8_t)4)
#define L_CAM_GET_VOLUME_ANSW           ((uint8_t)6)
#define L_CAM_GET_DENSITY               ((uint8_t)4)
#define L_CAM_GET_DENSITY_CONF          ((uint8_t)4)
#define L_CAM_GET_DENSITY_ANSW          ((uint8_t)6)
#define L_CAM_FRAME_TO_MEM              ((uint8_t)4)
#define L_CAM_FRAME_TO_MEM_CONF         ((uint8_t)4)
#define L_CAM_FRAME_TO_MEM_ANSW         ((uint8_t)4)
#define L_CAM_GET_CUR_WIN               ((uint8_t)4)
#define L_CAM_GET_CUR_WIN_CONF          ((uint8_t)12)
#define L_CAM_GET_CUR_WIN_ANSW          ((uint8_t)6)
#define L_CAM_DEF_LABELS                ((uint8_t)4)
#define L_CAM_DEF_LABELS_CONF           ((uint8_t)4)
#define L_CAM_DEF_LABELS_ANSW           ((uint8_t)10)

#define CAM_TYPE_CONF                   ((uint8_t)0x01)
#define CAM_TYPE_ANSW                   ((uint8_t)0x02)
#define ETH_TYPE_CONF                   ((uint8_t)0x04)
#define ETH_TYPE_ANSW                   ((uint8_t)0x08)

#define FRAME_PACKET_MAXSIZE            ((uint16_t)1500)

/* ������ ������ ��� Analog Charger (���������� A0, A1)*/
#define ACH_PING                        ((uint8_t)0x01)         // ������� ��������� ���� ������
#define ACH_SET_CUR                     ((uint8_t)0x02)         // ������� ��������� ���� ������

#define NUM_COM_ACH                     ((uint8_t)2)            // ����� ���������� ������ ��� ����������� ���������

/* ���� ���������� �������� ������ com_sr_acha, com_sr_achb */
#define COM_ACH_PING                    ((uint32_t)0x00000001)
#define COM_ACH_SET_CUR                 ((uint32_t)0x00000002)

/* ����� ������ � ������ */
#define L_ACH_PING                      ((uint8_t)3)
#define L_ACH_PING                      ((uint8_t)3)
#define L_ACH_SET_CUR                   ((uint8_t)6)
#define L_ACH_SET_CUR_ANSW              ((uint8_t)4)

/* ������ ������ ��� Analog Discharger (���������� A2)*/
#define ADCH_PING                       ((uint8_t)0x01)         // ������� ��������� ���� ������
#define ADCH_SET_CUR                    ((uint8_t)0x02)         // ������� ��������� ���� ������

#define NUM_COM_ADCH                    ((uint8_t)2)            // ����� ���������� ������ ��� ����������� ����������

/* ���� ���������� �������� ������ com_sr_adch */
#define COM_ADCH_PING                   ((uint32_t)0x00000001)
#define COM_ADCH_SET_CUR                ((uint32_t)0x00000002)

/* ����� ������ � ������ */
#define L_ADCH_PING                     ((uint8_t)3)
#define L_ADCH_PING                     ((uint8_t)3)
#define L_ADCH_SET_CUR                  ((uint8_t)5)
#define L_ADCH_SET_CUR_ANSW             ((uint8_t)4)
