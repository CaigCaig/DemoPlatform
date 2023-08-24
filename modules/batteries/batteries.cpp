#include "stm32f4xx.h"
#include "batteries.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "leds.h"
#include "mb_def.h"
#include "mcp3422.h"
#include "com_handler.h"
#include "rs485.h"
#include "ch_dch.h"
#include "ach_adch.h"
#include "hood.h"

__IO uint32_t battery_sr[CHARGERS]={0};
__IO uint32_t tests_state[CHARGERS]={0};
__IO uint32_t mch_timeout[CHARGERS]={0};        // с, таймер времени основного зар€да
__IO uint32_t och_timeout[CHARGERS]={0};        // с, таймер времени дозар€да
__IO uint32_t id_timeout[CHARGERS]={0};         // с, таймер времени начального разр€да
__IO uint32_t rd_timeout[CHARGERS]={0};         // с, таймер времени остаточного разр€да
__IO BatteryTypeDef BatteryOptions[CHARGERS]={0x0000};
__IO BatteryTypeDef_f BatteryOptions_f[CHARGERS]={0};
__IO uint16_t BatteryType=0;                    // дл€ батареи с набором параметров тип равен 1.
extern uint8_t USART6BufferTxCmd[USART6_CMD_LEN], USART6BufferRx[USART6_CMD_LEN], rs485_com_len, rs485_f;
extern uint32_t com_sr_h1;            // статусный регистр прин€ти€ команд дл€ Hood1 
extern uint32_t com_sr_h2;            // статусный регистр прин€ти€ команд дл€ Hood2 
extern uint8_t ChargeState;
extern WatchTypeDef watch[CHARGERS];
extern uint32_t com_sr_acha;            // статусный регистр прин€ти€ команд дл€ Analog Charger A
extern uint32_t com_sr_achb;            // статусный регистр прин€ти€ команд дл€ Analog Charger B
extern uint32_t com_sr_adch;            // статусный регистр прин€ти€ команд дл€ Analog Discharger
extern uint8_t AchaBufferTxCmd[ACHA_CMD_LEN], AchaBufferRx[ACHA_CMD_LEN], acha_com_len, acha_f;
extern uint8_t AchbBufferTxCmd[ACHB_CMD_LEN], AchbBufferRx[ACHB_CMD_LEN], achb_com_len, achb_f;
extern uint8_t AdchBufferTxCmd[ADCH_CMD_LEN], AdchBufferRx[ADCH_CMD_LEN], adch_com_len, adch_f;
extern uint8_t acha_type_answ[NUM_COM_ACH], achb_type_answ[NUM_COM_ACH], adch_type_answ[NUM_COM_ADCH];
//extern int32_t Vbat[BAT_QUANTITY];
extern MCP3424_Meas_TypeDef MCP3424_data;
BatTest_t bat_test;

/*
BatteryTypeDef const Battery1=
{
  53*10,        // C1, A*10
  314,          // V1, V*10
  60*60,        // TC1, sec
  900,          // DTC11, sec
  0,            // DTC12, sec
  53,           // C2, A*10
  240*60,       // TC2, sec
  15*60,        // TW, sec
  88,           // MW, ml
  21*10,        // PV
  53,           // C3
  240*60,       // TC3
  0,            // TCD
  530,          // D1
  3396,         // TD1
  10,           // VR
  5,            // VS
  12*60,        // TS, min
  4*60,         // DTS1, min
  0,            // DTS2, min
};

BatteryTypeDef const Battery2=
{
  36*10,        // C1, A*10
  314,          // V1, V*10
  60*60,        // TC1, sec
  900,          // DTC11, sec
  0,            // DTC12, sec
  36,           // C2, A*10
  240*60,       // TC2, sec
  15*60,        // TW, sec
  88,           // MW, ml
  21*10,        // PV
  36,           // C3
  240*60,       // TC3
  0,            // TCD
  530,          // D1
  3396,         // TD1
  10,           // VR
  5,            // VS
  12*60,        // TS, min
  4*60,         // DTS1, min
  0,            // DTS2, min
};
*/
uint8_t Main_Charge(void)
{
  return OK;
}

uint8_t Overcharge(void)
{
  return OK;
}

uint8_t Electrolyte_Check(void)
{
  return OK;
}

uint8_t Special_Test(void)
{
  return OK;
}

uint8_t Initial_Discharge(void)
{
  return OK;
}

uint8_t Capacity_Test(void)
{
  return OK;
}

uint8_t Resudual_Discharge(void)
{
  return OK;
}

void vFULL_SERVICE_1(void* params)
{
  tests_state[BATTERY1-1] &= ~FULL_SERVICE_pos;
  vTaskDelete(NULL);
}

void vFULL_SERVICE_2(void* params)
{
  tests_state[BATTERY2-1] &= ~FULL_SERVICE_pos;
  vTaskDelete(NULL);
}

void vMAIN_CHARGE_1(void* params)
{
  bat_test.BATTERY_A = T_MAIN_CHARGE;
  GPIO_WriteBit(power36v_GPIO_Port, power36v_Pin, Bit_SET);
  vTaskDelay(5000);
  portTickType xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  ChargeState |= BATTERY1;
  uint8_t nums_9A =BatteryOptions[CHARGER1].MCHI/90;    // вычисл€ем количество 9ј каналов зар€да
  uint16_t residual = BatteryOptions[CHARGER1].MCHI-nums_9A*90; // разностный ток между заданным значением зар€да и током через дискретные зар€дники
  if (residual>45) residual = residual - 45;    // определ€ем, включать или нет канал зар€да на 4,5ј
  ACH_Set((uint8_t) residual);
  CH_DCH_SET(CHARGER, BatteryOptions[CHARGER1].MCHI);
  mch_timeout[CHARGER1]=BatteryOptions[CHARGER1].MCHT*60;       // вычисл€ем врем€ первичной выдержки в секундах
  watch[CHARGER1].Hour=BatteryOptions[CHARGER1].MCHT/60;
  watch[CHARGER1].Min=BatteryOptions[CHARGER1].MCHT - watch[CHARGER1].Hour*60;
  watch[CHARGER1].Mode=TIMER;
  watch[CHARGER1].State=START;
  do
  {
    vTaskDelayUntil(&xLastWakeTime, 1000);
    mch_timeout[CHARGER1]--;
  }
  while (mch_timeout[CHARGER1]);        // выжидаем первичное врем€ основного зар€да
  if (MCP3424_data.VbatA<(BatteryOptions[CHARGER1].MCHV*100))
//  if (Vbat[CHARGER1]<(BatteryOptions[CHARGER1].MCHV*100))
  {
    mch_timeout[CHARGER1]=BatteryOptions[CHARGER1].DMCHT1*60;   // вычисл€ем врем€ дополнительной выдержки в секундах
    watch[CHARGER1].Hour=BatteryOptions[CHARGER1].DMCHT1/60;
    watch[CHARGER1].Min=BatteryOptions[CHARGER1].DMCHT1 - watch[CHARGER1].Hour*60;
    do
    {
      vTaskDelayUntil(&xLastWakeTime, 1000);
      mch_timeout[CHARGER1]--;
    }
    while (mch_timeout[CHARGER1]||(MCP3424_data.VbatA>=(BatteryOptions[CHARGER1].MCHV*100)));       // выжидаем дополнительное врем€ основного зар€да
//    while (mch_timeout[CHARGER1]||(Vbat[CHARGER1]>=(BatteryOptions[CHARGER1].MCHV*100)));       // выжидаем дополнительное врем€ основного зар€да
  }
  watch[CHARGER1].State=STOP;
  ACH_Set(0);
  CH_DCH_SET(CHARGER, 0);
  GPIO_WriteBit(power36v_GPIO_Port, power36v_Pin, Bit_RESET);
  ChargeState &= ~BATTERY1;
  tests_state[BATTERY1-1] &= ~MAIN_CHARGE_pos;
  vTaskDelete(NULL);
}

void vMAIN_CHARGE_2(void* params)
{
  tests_state[BATTERY2-1] &= ~MAIN_CHARGE_pos;
  vTaskDelete(NULL);
}

void vOVERCHARGE_1(void* params)
{
  GPIO_WriteBit(power36v_GPIO_Port, power36v_Pin, Bit_SET);
  vTaskDelay(5000);
  portTickType xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  ChargeState |= BATTERY1;
  uint16_t residual = BatteryOptions[CHARGER1].OCHI;    // ток дозар€да
  ACH_Set((uint8_t) residual);
  och_timeout[CHARGER1]=BatteryOptions[CHARGER1].OCHT*60;       // вычисл€ем врем€ выдержки в секундах
  watch[CHARGER1].Hour=BatteryOptions[CHARGER1].OCHT/60;
  watch[CHARGER1].Min=BatteryOptions[CHARGER1].OCHT - watch[CHARGER1].Hour*60;
  watch[CHARGER1].Mode=TIMER;
  watch[CHARGER1].State=START;
  do
  {
    vTaskDelayUntil(&xLastWakeTime, 1000);
    och_timeout[CHARGER1]--;
  }
  while (och_timeout[CHARGER1]);        // выжидаем врем€ дозар€да
  watch[CHARGER1].State=STOP;
  ACH_Set(0);
  GPIO_WriteBit(power36v_GPIO_Port, power36v_Pin, Bit_RESET);
  ChargeState &= ~BATTERY1;
  tests_state[BATTERY1-1] &= ~OVERCHARGE_pos;
  vTaskDelete(NULL);
}

void vOVERCHARGE_2(void* params)
{
  tests_state[BATTERY2-1] &= ~OVERCHARGE_pos;
  vTaskDelete(NULL);
}

void vELECTROLYTE_CHECK_1(void* params)
{
  tests_state[BATTERY1-1] &= ~ELECTROLYTE_CHECK_pos;
  vTaskDelete(NULL);
}

void vELECTROLYTE_CHECK_2(void* params)
{
  tests_state[BATTERY2-1] &= ~ELECTROLYTE_CHECK_pos;
  vTaskDelete(NULL);
}

void vSPECIAL_TEST_1(void* params)
{
  tests_state[BATTERY1-1] &= ~SPECIAL_TEST_pos;
  vTaskDelete(NULL);
}

void vSPECIAL_TEST_2(void* params)
{
  tests_state[BATTERY2-1] &= ~SPECIAL_TEST_pos;
  vTaskDelete(NULL);
}

void vINITIAL_DISCHARGE_1(void* params)
{
  ChargeState |= BATTERY1;
  GPIO_WriteBit(chdch_A_B_GPIO_Port, chdch_A_B_Pin, Bit_SET);
  CH_DCH_SET(DISCHARGER, BatteryOptions[CHARGER1].IDI);
  watch[CHARGER1].Hour=0;
  watch[CHARGER1].Min=0;
  watch[CHARGER1].Sec=0;
  watch[CHARGER1].Mode=WATCH;
  watch[CHARGER1].State=START;
  do
  {
    vTaskDelay(1000);
  }
//  while (Vbat[CHARGER1]>BatteryOptions[CHARGER1].IDV*100);        // выжидаем врем€ дозар€да
  while (MCP3424_data.VbatA > BatteryOptions[CHARGER1].IDV*100);        // выжидаем врем€ дозар€да
  watch[CHARGER1].State=STOP;
  CH_DCH_SET(DISCHARGER, 0);
  GPIO_WriteBit(fan_en_GPIO_Port, fan_en_Pin, Bit_RESET);
  ChargeState &= ~BATTERY1;
  tests_state[BATTERY1-1] &= ~INITIAL_DISCHARGE_pos;
  vTaskDelete(NULL);
}

void vINITIAL_DISCHARGE_2(void* params)
{
  tests_state[BATTERY2-1] &= ~INITIAL_DISCHARGE_pos;
  vTaskDelete(NULL);
}

void vCAPACITY_TEST_1(void* params)
{
  tests_state[BATTERY1-1] &= ~CAPACITY_TEST_pos;
  vTaskDelete(NULL);
}

void vCAPACITY_TEST_2(void* params)
{
  tests_state[BATTERY2-1] &= ~CAPACITY_TEST_pos;
  vTaskDelete(NULL);
}

void vRESIDUAL_DISCHARGE_1(void* params)
{
  GPIO_WriteBit(chdch_A_B_GPIO_Port, chdch_A_B_Pin, Bit_SET);
  CH_DCH_SET(DISCHARGER, BatteryOptions[BATTERY1-1].IDI);
  USART6BufferTxCmd[0] = H1_ID;
  USART6BufferTxCmd[1] = RESIDUAL_DISCHARGE;
  USART6BufferTxCmd[2] = BatteryOptions[BATTERY1-1].RDT/60;
  USART6BufferTxCmd[3] = BatteryOptions[BATTERY1-1].RDT - USART6BufferTxCmd[2]*60;
  rd_timeout[BATTERY1-1]=BatteryOptions[BATTERY1-1].RDT*60;              // вычисл€ем врем€ первичной выдержки в секундах
  USART6BufferTxCmd[4] = USART6BufferTxCmd[0] ^ USART6BufferTxCmd[1] ^ USART6BufferTxCmd[2] ^ USART6BufferTxCmd[3];
  RS485_Send(USART6BufferTxCmd, L_RESIDUAL_DISCHARGE);
  uint16_t timeout=0;
  do
  {
    vTaskDelay(1);
    if (com_sr_h1 & COM_RESIDUAL_DISCHARGE)
    {
      ChargeState |= BATTERY1;
      com_sr_h1 &=~COM_RESIDUAL_DISCHARGE;
    }
  timeout++;
  } while (timeout<TIMEOUT_HOOD);

  portTickType xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  watch[BATTERY1-1].Hour=BatteryOptions[BATTERY1-1].RDT/60;
  watch[BATTERY1-1].Min=BatteryOptions[BATTERY1-1].RDT - watch[BATTERY1-1].Hour*60;
  watch[BATTERY1-1].Mode=TIMER;
  watch[BATTERY1-1].State=START;
  do
  {
    vTaskDelayUntil(&xLastWakeTime, 1000);
    rd_timeout[BATTERY1-1]--;
  }
  while (rd_timeout[CHARGER1]);         // выжидаем первичное врем€ остаточного разр€да
  USART6BufferTxCmd[0] = H1_ID;
  USART6BufferTxCmd[1] = RD_TIMELEFT;
  USART6BufferTxCmd[2] = H1_ID ^ RD_TIMELEFT;
  RS485_Send(USART6BufferTxCmd, L_RD_TIMELEFT); // отправл€ем запрос на уточнение оставшегос€ времени
  timeout=0;
  do
  {
    vTaskDelay(1);
    if (com_sr_h1 & COM_RD_TIMELEFT)
    {
      watch[BATTERY1-1].Hour=USART6BufferRx[3];
      watch[BATTERY1-1].Min=USART6BufferRx[4];
      rd_timeout[BATTERY1-1]=USART6BufferRx[3]*3600+USART6BufferRx[4]*60;
      com_sr_h1 &=~COM_RD_TIMELEFT;
      break;
    }
    timeout++;
  } while (timeout<TIMEOUT_HOOD);
  if (timeout<TIMEOUT_HOOD)
  {
    do
    {
      vTaskDelayUntil(&xLastWakeTime, 1000);
      rd_timeout[BATTERY1-1]--;
    }
    while (rd_timeout[BATTERY1-1]);         // выжидаем оставшеес€ врем€ остаточного разр€да
  }
  watch[BATTERY1-1].State=STOP;
  ChargeState &= ~BATTERY1;
  tests_state[BATTERY1-1] &= ~RESIDUAL_DISCHARGE_pos;
  vTaskDelete(NULL);
}

void vRESIDUAL_DISCHARGE_2(void* params)
{
  tests_state[BATTERY2-1] &= ~RESIDUAL_DISCHARGE_pos;
  vTaskDelete(NULL);
}