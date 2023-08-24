#ifndef __HOOD_H
#define __HOOD_H

//#define HOOD_TX_LEN  ((uint32_t)6)
//#define HOOD_RX_LEN  ((uint32_t)6)

/* —писок команд дл€ Hood*/

#define PUSH_TO_CELL            0x02    // команда на выдавливание содержимого шприца в €чейку
#define PULL_FROM_CELL          0x03    // команда набора жидкости в шприц из €чейки
#define BAT_VOLTAGE             0x05    // команда измерени€ напр€жени€ батареи
#define V_CELL                  0x06    // команда измерени€ напр€жени€ на €чейке
#define V_N_CELL                0x08    // команда измерени€ напр€жени€ на €чейках от 1 до n
#define SHUNT_1OHM_ON           0x0A    // команда подключени€ шунта 1 ќм к €чейке n
#define SHUNT_0OHM_ON           0x0B    // команда подключени€ шунта 0 ќм к €чейке n
#define SHUNT_1OHM_OFF          0x0C    // команда отключени€ шунта 1 ќм от €чейки n
#define SHUNT_0OHM_OFF          0x0D    // команда отключени€ шунта 0 ќм от €чейки n
#define SHUNT_1OHM_ALL_OFF      0x0E    // команда отключени€ всех шунтов 1 ќм
#define SHUNT_0OHM_ALL_OFF      0x0F    // команда отключени€ всех шунтов 0 ќм
#define INIT_SM                 0x11    // команда инициализации Ўƒ
#define START_STOP_SM           0x13    // команда запуска/остановки Ўƒ source
#define RESIDUAL_DISCHARGE      0x15    // команда запуска процедуры остаточного разр€да
#define RD_TIMELEFT             0x16    // команда запроса оставшегос€ времени остаточного разр€да

/* ƒлины команд в байтах */
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