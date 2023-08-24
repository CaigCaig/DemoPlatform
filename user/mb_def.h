/* Список команд для материнской платы (устройство AE)*/
#define SET_NET_OPTIONS                 ((uint8_t)0x01)    // команда установки параметров сети (DHCP вкл/выкл, IP, MASK, GATE)
#define SET_BATTERY_OPTIONS             ((uint8_t)0x02)    // команда установки параметров обслуживания батареи с определёнными параметрами
#define SET_BATTERY_SERVICE             ((uint8_t)0x03)    // команда установки параметров обслуживания батареи определённого типа
#define START_BATTERY_SERVICE           ((uint8_t)0x04)    // команда начала обслуживания батареи
#define BAT_VOLTAGE                     ((uint8_t)0x05)    // команда измерения напряжения батареи
#define FINALIZE_BATTERY_SERVICE        ((uint8_t)0x06)    // команда завершения обслуживания батареи
#define FIX_CHARGER_CHANNEL_ON          ((uint8_t)0x07)    // команда включения каналов зарядника
#define FIX_CHARGER_CHANNEL_OFF         ((uint8_t)0x08)    // команда выключения каналов зарядника
#define FIX_CHARGER_STATE               ((uint8_t)0x09)    // команда чтения состояния каналов зарядника
#define PING                            ((uint8_t)0x14)    // команда контроля установки соединения

/* Длины команд для материнской платы в байтах */
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

/* Биты статусного регистра команд com_mb_sr */
#define COM_SET_NET_OPTIONS             ((uint32_t)0x00000001)    // команда установки параметров сети (DHCP вкл/выкл, IP, MASK, GATE)
#define COM_SET_BATTERY_OPTIONS         ((uint32_t)0x00000002)    // команда установки параметров обслуживания батареи с определёнными параметрами
#define COM_SET_BATTERY_SERVICE         ((uint32_t)0x00000004)    // команда установки параметров обслуживания батареи определённого типа
#define COM_START_BATTERY_SERVICE       ((uint32_t)0x00000008)    // команда начала обслуживания батареи
#define COM_BAT_VOLTAGE                 ((uint32_t)0x00000010)    // команда измерения напряжения батареи

/* Список команд для шапок (устройства A9, AA, AB)*/
#define water_5ml               ((uint8_t)0x01) // команда набора 5 мл жидкости
#define push_to_cell            ((uint8_t(0x02) // команда на выдавливание содержимого шприца в ячейку
#define pull_from_cell          ((uint8_t(0x03) // команда набора жидкости в шприц из ячейки
#define push_to_drainage        ((uint8_t(0x04) // команда на выдавливание содержимого шприца в дренаж
#define bat_voltage             ((uint8_t(0x05) // команда измерения напряжения батареи
#define v_cell                  ((uint8_t(0x06) // команда измерения напряжения на ячейке
#define t_cell                  ((uint8_t(0x07) // команда измерения температуры на ячейке
#define v_n_cell                ((uint8_t(0x08) // команда измерения напряжения на ячейках от 1 до n
#define t_n_cell                ((uint8_t(0x09) // команда измерения температуры на ячейках от 1 до n
#define shunt_1ohm_on           ((uint8_t(0x0A) // команда подключения шунта 1 Ом к ячейке n
#define shunt_0ohm_on           ((uint8_t(0x0B) // команда подключения шунта 0 Ом к ячейке n
#define shunt_1ohm_off          ((uint8_t(0x0C) // команда отключения шунта 1 Ом от ячейки n
#define shunt_0ohm_off          ((uint8_t(0x0D) // команда отключения шунта 0 Ом от ячейки n
#define shunt_1ohm_all_off      ((uint8_t(0x0E) // команда отключения всех шунтов 1 Ом
#define shunt_0ohm_all_off      ((uint8_t(0x0F) // команда отключения всех шунтов 0 Ом
#define bung_pressure           ((uint8_t(0x10) // команда теста пробок под давлением
#define init_sm                 ((uint8_t(0x11) // команда инициализации ШД
#define switch_bl               ((uint8_t)0x12) // команда вкл/выкл подсветки
#define start_stop_sm           ((uint8_t)0x13) // команда запуска/остановки ШД source
#define hello                   ((uint8_t)0x14) // команда "пинг"
//#define RESIDUAL_DISCHARGE      ((uint8_t)0x15) // команда запуска процедуры остаточного разряда
//#define RD_TIMELEFT             ((uint8_t)0x16) // команда запроса оставшегося времени остаточного заряда

/* Длины команд в байтах */
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

/* Биты статусного регистра команд com_sr_h1, com_sr_h2 */
#define com_water_5ml                   ((uint32_t)0x00000001)  // команда набора 5 мл жидкости
#define com_push_to_cell                ((uint32_t)0x00000002)  // команда на выдавливание содержимого шприца в ячейку
#define com_pull_from_cell              ((uint32_t)0x00000004)  // команда набора жидкости в шприц из ячейки
#define com_push_to_drainage            ((uint32_t)0x00000008)  // команда на выдавливание содержимого шприца в дренаж
#define com_bat_voltage                 ((uint32_t)0x00000010)  // команда измерения напряжения батареи
#define com_v_cell                      ((uint32_t)0x00000020)  // команда измерения напряжения на ячейке
#define com_t_cell                      ((uint32_t)0x00000040)  // команда измерения температуры на ячейке
#define com_v_n_cell                    ((uint32_t)0x00000080)  // команда измерения напряжения на ячейках от 1 до n
#define com_t_n_cell                    ((uint32_t)0x00000100)  // команда измерения температуры на ячейках от 1 до n
#define com_shunt_1ohm_on               ((uint32_t)0x00000200)  // команда подключения шунта 1 Ом к ячейке n
#define com_shunt_0ohm_on               ((uint32_t)0x00000400)  // команда подключения шунта 0 Ом к ячейке n
#define com_shunt_1ohm_off              ((uint32_t)0x00000800)  // команда отключения шунта 1 Ом от ячейки n
#define com_shunt_0ohm_off              ((uint32_t)0x00001000)  // команда отключения шунта 0 Ом от ячейки n
#define com_shunt_1ohm_all_off          ((uint32_t)0x00002000)  // команда отключения всех шунтов 1 Ом
#define com_shunt_0ohm_all_off          ((uint32_t)0x00004000)  // команда отключения всех шунтов 0 Ом
#define com_bung_pressure               ((uint32_t)0x00008000)  // команда теста пробок под давлением
#define com_init_sm                     ((uint32_t)0x00010000)  // команда инициализации ШД
#define com_switch_bl                   ((uint32_t)0x00020000)  // команда вкл/выкл подсветки
#define com_start_stop_sm               ((uint32_t)0x00040000)  // команда запуска/остановки ШД source
#define com_hello                       ((uint32_t)0x00080000)  // команда ping
#define COM_RESIDUAL_DISCHARGE          ((uint32_t)0x00100000)  // команда запуска процедуры остаточного разряда
#define COM_RD_TIMELEFT                 ((uint32_t)0x00200000)  // команда запроса оставшегося времени остаточного заряда

/* Список команд для видеокамер (устройство AD)*/
#define CAM_ACT                         ((uint8_t)0x01)         // команда активации видеокамеры
#define CAM_WATCH_WIN                   ((uint8_t)0x03)         // команда установки окна наблюдения видеокамеры
#define CAM_RD_REG                      ((uint8_t)0x08)         // команда чтения регистра
#define CAM_WR_REG                      ((uint8_t)0x09)         // команда записи в регистр
#define CAM_GET_VOLUME                  ((uint8_t)0x0C)         // команда определения объёма жидкости
#define CAM_GET_DENSITY                 ((uint8_t)0x0E)         // команда определения плотности жидкости
#define CAM_FRAME_TO_MEM                ((uint8_t)0x0F)         // команда записи кадра в память
#define CAM_GET_CUR_WIN                 ((uint8_t)0x10)         // команда передачи данных полного кадра из видеопамяти
#define CAM_DEF_LABELS                  ((uint8_t)0x11)         // команда определения координат меток

#define NUM_COM_CAM                     ((uint8_t)17)           // общее количество команд для видеокамер

/* Биты статусного регистра команд com_sr_cam */
#define COM_CAM_ACT                     ((uint32_t)0x00000001)
#define COM_CAM_WATCH_WIN               ((uint32_t)0x00000004)
#define COM_CAM_RD_REG                  ((uint32_t)0x00000080)
#define COM_CAM_WR_REG                  ((uint32_t)0x00000100)
#define COM_CAM_GET_VOLUME              ((uint32_t)0x00000800)
#define COM_CAM_GET_DENSITY             ((uint32_t)0x00002000)
#define COM_CAM_FRAME_TO_MEM            ((uint32_t)0x00004000)
#define COM_CAM_GET_CUR_WIN             ((uint32_t)0x00008000)
#define COM_CAM_DEF_LABELS              ((uint32_t)0x00010000)

/* Длины команд в байтах */
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

/* Список команд для Analog Charger (устройства A0, A1)*/
#define ACH_PING                        ((uint8_t)0x01)         // команда установки тока заряда
#define ACH_SET_CUR                     ((uint8_t)0x02)         // команда установки тока заряда

#define NUM_COM_ACH                     ((uint8_t)2)            // общее количество команд для аналогового зарядника

/* Биты статусного регистра команд com_sr_acha, com_sr_achb */
#define COM_ACH_PING                    ((uint32_t)0x00000001)
#define COM_ACH_SET_CUR                 ((uint32_t)0x00000002)

/* Длины команд в байтах */
#define L_ACH_PING                      ((uint8_t)3)
#define L_ACH_PING                      ((uint8_t)3)
#define L_ACH_SET_CUR                   ((uint8_t)6)
#define L_ACH_SET_CUR_ANSW              ((uint8_t)4)

/* Список команд для Analog Discharger (устройство A2)*/
#define ADCH_PING                       ((uint8_t)0x01)         // команда установки тока заряда
#define ADCH_SET_CUR                    ((uint8_t)0x02)         // команда установки тока заряда

#define NUM_COM_ADCH                    ((uint8_t)2)            // общее количество команд для аналогового разрядника

/* Биты статусного регистра команд com_sr_adch */
#define COM_ADCH_PING                   ((uint32_t)0x00000001)
#define COM_ADCH_SET_CUR                ((uint32_t)0x00000002)

/* Длины команд в байтах */
#define L_ADCH_PING                     ((uint8_t)3)
#define L_ADCH_PING                     ((uint8_t)3)
#define L_ADCH_SET_CUR                  ((uint8_t)5)
#define L_ADCH_SET_CUR_ANSW             ((uint8_t)4)
