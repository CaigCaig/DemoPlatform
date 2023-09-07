#include "stm32f4xx.h"
#include "com_handler.h"
#include "FreeRTOS.h"
#include "task.h"
#include "leds.h"
#include "mb_def.h"
#include "device_commands.h"
#include "net_bridge.h"
#include "mcp3422.h"
#include "rs485.h"
#include "ch_dch.h"
#include "ach_adch.h"
#include "hood.h"
#include "24AA025E48.h"
#include "batteries.h"
#include "sockets.h"
#include "string.h"
#include "rtc.h"

extern int32_t Vbat[BAT_QUANTITY];
extern uint32_t battery_sr[CHARGERS];
extern uint32_t tests_state[CHARGERS];
extern uint32_t rd_timeout[CHARGERS];
extern BatteryTypeDef BatteryOptions[CHARGERS];
extern BatteryTypeDef_f BatteryOptions_f[CHARGERS];
extern uint16_t BatteryType;
extern uint8_t ChargeState;
uint8_t Net_Update=0;
uint32_t PeriphBusy=0;          // 1-BUSY/0-READY
uint32_t com_sr_mh=0;           // статусный регистр принятия команд для master_hood
uint32_t com_sr_h1=0;           // статусный регистр принятия команд для hood1
uint32_t com_sr_h2=0;           // статусный регистр принятия команд для hood2
uint32_t com_sr_cam=0;          // статусный регистр принятия команд для видеокамер
uint32_t cam_conf_answ_f=0;     // регистр признака подтверждения/ответа команды для видеокамер
extern uint8_t USART6BufferTxCmd[USART6_CMD_LEN], USART6BufferRx[USART6_CMD_LEN], rs485_com_len, rs485_f;
extern uint8_t USART6ConfBuf[USART6_CONF_LEN], USART6AnswBuf[USART6_ANSW_LEN];
extern uint8_t cam_type_answ[NUM_COM_CAM];
extern char USART6FrameBuf[2][FRAME_PACKET_MAXSIZE];
extern uint32_t USART6_BRR_Copy;
extern uint8_t cur_buf;
extern uint16_t Frame_Packets, tim_frame;
extern uint32_t Frame_Packet_Size;

uint32_t com_sr_adch=0;         // статусный регистр принятия команд для ADCH
uint32_t com_sr_acha=0;         // статусный регистр принятия команд для ACHA
uint32_t com_sr_achb=0;         // статусный регистр принятия команд для ACHB
uint32_t acha_conf_answ_f=0;    // регистр признака подтверждения/ответа команды для аналогового зарядника A
uint32_t achb_conf_answ_f=0;    // регистр признака подтверждения/ответа команды для аналогового зарядника B
uint32_t adch_conf_answ_f=0;    // регистр признака подтверждения/ответа команды для аналогового раззарядника
extern uint8_t AchaBufferTxCmd[ACHA_CMD_LEN], AchaBufferRx[ACHA_CMD_LEN], acha_com_len, acha_f;
extern uint8_t AchbBufferTxCmd[ACHB_CMD_LEN], AchbBufferRx[ACHB_CMD_LEN], achb_com_len, achb_f;
extern uint8_t AdchBufferTxCmd[ADCH_CMD_LEN], AdchBufferRx[ADCH_CMD_LEN], adch_com_len, adch_f;
extern uint8_t acha_type_answ[NUM_COM_ACH], achb_type_answ[NUM_COM_ACH], adch_type_answ[NUM_COM_ADCH];

extern BatTest_t bat_test;

uint16_t gtmp16;
uint32_t gtmp32=0;
volatile OPU_coordinates_t opu_coordinates;

OPTICAL_SMdata_t  sm_data;

OPU_TIME_t utc;
OPU_STEPPER_t opu_stepper_motor;

xTaskHandle TaskWatch;

void Com_Handler (int conn, char *com_buffer, uint16_t com_buf_len)
{
  uint32_t key = *(com_buffer) << 24 | *(com_buffer + 1) << 16 |  *(com_buffer + 2) << 8 | *(com_buffer + 3);
  Device_t device = (Device_t)*(com_buffer+4);
  uint8_t command = *(com_buffer+5);
    
  switch (key)
  {
  case 0x4D495054:
    switch (device)
    {

/*---------------- обработка команд для оптической установки ----------------*/ 
    case OPTICAL:
      switch (command)
      {
      case COM_OPTICAL_PING:
        OPTICAL_ACK_Ping_t optical_ping_ack;
        strcpy(optical_ping_ack.str, "MIPT");
        optical_ping_ack.device = OPTICAL;
        optical_ping_ack.command = COM_OPTICAL_PING;
        strcpy(optical_ping_ack.ack, " ACK");
        write(conn, &optical_ping_ack, sizeof(optical_ping_ack));
        break;
      case COM_OPTICAL_SM:
        OPTICAL_ACK_SM_t optical_sm_ack;
        sm_data.data_type = *(OPTICAL_Steps_Type_t*)(com_buffer + COM_OPT_SM_TYPE_OFFSET);
        switch (sm_data.data_type)
        {
        case OPTICAL_STEPS:
          sm_data.steps = *(int32_t *)(com_buffer + COM_OPT_SM_DATA_OFFSET);
          break;
        case OPTICAL_ANGLE:
          sm_data.angle = *(float *)(com_buffer + COM_OPT_SM_DATA_OFFSET);
          break;
        }
        strcpy(optical_sm_ack.str, "MIPT");
        optical_sm_ack.device = OPTICAL;
        optical_sm_ack.command = COM_OPTICAL_SM;
        optical_sm_ack.error = OPTICAL_OK;
        write(conn, &optical_sm_ack, sizeof(optical_sm_ack));
        xTaskCreate(vStepMotors, "Steps", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
        break;
      }
      break;

/* -------------- обработка команд для ОПУ ----------------*/
    case OPU:
      switch (command)
      {
      case OPU_REQUEST:
        OPU_ACK_REQUEST_t opu_ack_request;
        write(conn, &opu_ack_request, sizeof(opu_ack_request));
        break;
      case OPU_SETLON:
        OPU_ACK_SETLON_t opu_ack_setlon;
        opu_coordinates.longitude = *(float *)(com_buffer+SETLON_NUM);
        opu_coordinates.e_w = *(com_buffer + SETLON_LET);
        strcpy(opu_ack_setlon.str, "MIPT"); 
        opu_ack_setlon.device = OPU;
        opu_ack_setlon.command = OPU_SETLON;
        opu_ack_setlon.error = OPU_OK;
        write(conn, &opu_ack_setlon, sizeof(opu_ack_setlon));
        break;
      case OPU_SETLAT:
        OPU_ACK_SETLAT_t opu_ack_setlat;
        opu_coordinates.latitude = *(float *)(com_buffer+SETLAT_NUM);
        opu_coordinates.n_s = *(com_buffer + SETLAT_LET);
        strcpy(opu_ack_setlat.str, "MIPT"); 
        opu_ack_setlat.device = OPU;
        opu_ack_setlat.command = OPU_SETLAT;
        opu_ack_setlat.error = OPU_OK;
        write(conn, &opu_ack_setlat, sizeof(opu_ack_setlat));
        xTaskCreate(vTestSMA, "TestSMA", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
        break;
      case OPU_TIME:
        TM_RTC_t datatime;
        OPU_ACK_TIME_t opu_ack_time;
        utc.year = *(com_buffer + TIME_YEAR)  << 8 | *(com_buffer + TIME_YEAR + 1);
        utc.month = *(com_buffer + TIME_MONTH);
        utc.day = *(com_buffer + TIME_DAY);
        utc.hour = *(com_buffer + TIME_HOUR);
        utc.min = *(com_buffer + TIME_MIN);
        utc.sec = *(com_buffer + TIME_SEC);
        datatime.date = 1;
        datatime.day = utc.day;
        datatime.month = utc.month;
        datatime.year = uint8_t(utc.year - 2000);
        datatime.hours = utc.hour;
        datatime.minutes = utc.min;
        datatime.seconds = utc.sec;
//        vTaskSuspend(TaskWatch);
        TM_RTC_SetDateTime(&datatime, TM_RTC_Format_BIN);
//        vTaskResume(TaskWatch);
/*        uint16_t tmp16;
        stoui(com_buffer+TIME_YEAR, &utc.year, 4);
        stoui(com_buffer+TIME_MONTH, &tmp16, 2);
        utc.month = (uint8_t)tmp16;
        stoui(com_buffer+TIME_DAY, &tmp16, 2);
        utc.day = (uint8_t)tmp16;
        stoui(com_buffer+TIME_HOUR, &tmp16, 2);
        utc.hour = (uint8_t)tmp16;
        stoui(com_buffer+TIME_MIN, &tmp16, 2);
        utc.min = (uint8_t)tmp16;
        stoui(com_buffer+TIME_SEC, &tmp16, 4);
        utc.sec = (uint8_t)tmp16; */
        strcpy(opu_ack_time.str, "MIPT"); 
        opu_ack_time.device = OPU;
        opu_ack_time.command = OPU_TIME;
        opu_ack_time.error = OPU_OK;
        write(conn, &opu_ack_time, sizeof(opu_ack_time));
        break;
      case OPU_LOAD:
        OPU_ACK_LOAD_t opu_ack_load;
        write(conn, &opu_ack_load, sizeof(opu_ack_load));
        break;
      case OPU_SETSTEPPER:
        OPU_ACK_SETSTEPPER_t opu_ack_setstepper;
        opu_stepper_motor.spr = *(com_buffer+6) << 8 | *(com_buffer+7);
        opu_stepper_motor.mode = *(com_buffer+8)  << 8 | *(com_buffer+9);
        strcpy(opu_ack_setstepper.str, "MIPT"); 
        opu_ack_setstepper.device = OPU;
        opu_ack_setstepper.command = OPU_SETSTEPPER;
        opu_ack_setstepper.error = OPU_OK;
        write(conn, &opu_ack_setstepper, sizeof(opu_ack_setstepper));
        break;
      case OPU_DATA:
        break;
      case OPU_CLEARDATA:
        OPU_ACK_CLEARDATA_t opu_ack_cleardata;
        strcpy(opu_ack_cleardata.str, "MIPT"); 
        opu_ack_cleardata.device = OPU;
        opu_ack_cleardata.command = OPU_CLEARDATA;
        opu_ack_cleardata.error = OPU_OK;
        write(conn, &opu_ack_cleardata, sizeof(opu_ack_cleardata));
        break;
      case OPU_REQUAIM:
        break;
      case OPU_SETAIMA:
        OPU_ACK_SETAIMA_t opu_ack_setaima;
        strcpy(opu_ack_setaima.str, "MIPT"); 
        opu_ack_setaima.device = OPU;
        opu_ack_setaima.command = OPU_SETAIMA;
        opu_ack_setaima.error = OPU_OK;
        write(conn, &opu_ack_setaima, sizeof(opu_ack_setaima));
        break;
      case OPU_SETAIME:
        OPU_ACK_SETAIME_t opu_ack_setaime;
        strcpy(opu_ack_setaime.str, "MIPT"); 
        opu_ack_setaime.device = OPU;
        opu_ack_setaime.command = OPU_SETAIME;
        opu_ack_setaime.error = OPU_OK;
        write(conn, &opu_ack_setaime, sizeof(opu_ack_setaime));
        break;
      case OPU_SETZERA:
        OPU_ACK_SETZERA_t opu_ack_setzera;
        strcpy(opu_ack_setzera.str, "MIPT"); 
        opu_ack_setzera.device = OPU;
        opu_ack_setzera.command = OPU_SETZERA;
        opu_ack_setzera.error = OPU_OK;
        write(conn, &opu_ack_setzera, sizeof(opu_ack_setzera));
        break;
      case OPU_SETZERE:
        OPU_ACK_SETZERE_t opu_ack_setzere;
        strcpy(opu_ack_setzere.str, "MIPT"); 
        opu_ack_setzere.device = OPU;
        opu_ack_setzere.command = OPU_SETZERE;
        opu_ack_setzere.error = OPU_OK;
        write(conn, &opu_ack_setzere, sizeof(opu_ack_setzere));
        break;
      case OPU_PING:
        OPU_ACK_Ping_t opu_ack_ping;
        strcpy(opu_ack_ping.str, "MIPT");
        opu_ack_ping.device = OPU;
        opu_ack_ping.command = OPU_PING;
        strcpy(opu_ack_ping.ack, " ACK");
        write(conn, &opu_ack_ping, sizeof(opu_ack_ping));
        break;
      }
      break;
    }
    break;
  }
  
    
#if 0
  if (*(com_buffer) == MB_ID)
  {
    
    if ((*(com_buffer+1) == BAT_VOLTAGE) && (com_buf_len == L_BAT_VOLTAGE))
    {
//      *com_buffer = MB_ID;
//      *(com_buffer+1) = bat_voltage;
      tmp=*(com_buffer+2);
      if (tmp == 0x01)
      {
        *(com_buffer+2) = (uint8_t) (Vbat[0]);
        *(com_buffer+3) = (uint8_t) ((Vbat[0])>>8);
        *(com_buffer+4) = (uint8_t) ((Vbat[0])>>16);
        *(com_buffer+5) = (uint8_t) ((Vbat[0])>>24);
        write(conn, (char *)com_buffer, L_BAT_VOLTAGE_ANSW);
      }
      if (tmp == 0x02)
      {
        *(com_buffer+2) = (uint8_t) (Vbat[1]);
        *(com_buffer+3) = (uint8_t) ((Vbat[1])>>8);
        *(com_buffer+4) = (uint8_t) ((Vbat[1])>>16);
        *(com_buffer+5) = (uint8_t) ((Vbat[1])>>24);
        write(conn, (char *)com_buffer, L_BAT_VOLTAGE_ANSW);
      }
    }

    if ((*(com_buffer+1) == SET_NET_OPTIONS) && (com_buf_len == L_SET_NET_OPTIONS))
    {
      extern eeprom_t eeprom_data;
      extern TaskHandle_t xADC;
      eeprom_data.eeprom_dhcp = *(com_buffer+2);
      eeprom_data.eeprom_ip = (*(com_buffer+3)) | (*(com_buffer+4)<<8) | (*(com_buffer+5)<<16) | (*(com_buffer+6)<<24);
      eeprom_data.eeprom_mask = (*(com_buffer+7)) | (*(com_buffer+8)<<8) | (*(com_buffer+9)<<16) | (*(com_buffer+10)<<24);
      eeprom_data.eeprom_gate = (*(com_buffer+11)) | (*(com_buffer+12)<<8) | (*(com_buffer+13)<<16) | (*(com_buffer+14)<<24);
      vTaskSuspend(xADC);
      if (Save_Net_Config() == SUCCESS) *(com_buffer+2) = 0;
      else *(com_buffer+2) = 1;
      vTaskResume(xADC);
      write(conn, (char *)com_buffer, L_SET_NET_OPTIONS_ANSW);
//      close(conn);
      Net_Update = 1;
/*
      extern TaskHandle_t xNetBridge;
      TaskHandle_t m_ethTaskHandle;
      extern TaskHandle_t xMProtoMQTT;
      ethernetBridge.m_state = ethstate_NotInit;
      vTaskDelete(xNetBridge);
      vTaskDelete(m_ethTaskHandle);
*/

      netBridge.configure();
      return;
//      SCB->AIRCR = 0x05FA0004;  // делаем программный сброс микроконтроллера для активации изменения настроек сети

    }

    if ((*(com_buffer+1) == SET_BATTERY_OPTIONS) && (com_buf_len == L_SET_BATTERY_OPTIONS))
    {
      uint8_t charger_num = *(com_buffer+2);
      if ((charger_num == BATTERY1) || (charger_num == BATTERY2))
      {
        charger_num--;  // аккумуляторы считаются с единицы, зарядники с нуля
        uint8_t i=BATTERY_OPTIONS_OFFSET, num_option;
        uint16_t option;
        BatteryType=0;
        battery_sr[charger_num] = 0;
        for (i=0; i<BATTERY_OPTIONS; i++)
        {
          num_option = *(com_buffer+BATTERY_OPTIONS_OFFSET+i*3);
          if (num_option==0)
          {
            break;
          }
          if ((i<BATTERY_OPTIONS)&&(num_option<=BATTERY_OPTIONS))
          {
            option = *(com_buffer+BATTERY_OPTIONS_OFFSET+i*3+1) + ((*(com_buffer+BATTERY_OPTIONS_OFFSET+i*3+2))<<8);
            switch (num_option)
            {
            case MCHI_num:
                  BatteryOptions[charger_num].MCHI = (uint16_t) option;
                  BatteryOptions_f[charger_num].MCHI= (float) BatteryOptions[charger_num].MCHI/10;
                  battery_sr[charger_num] |= MCHI_pos;
                  break;
            case MCHV_num:
                  BatteryOptions[charger_num].MCHV = (uint16_t) option;
                  BatteryOptions_f[charger_num].MCHV = (float) BatteryOptions[charger_num].MCHV/10;
                  battery_sr[charger_num] |= MCHV_pos;
                  break;
            case MCHT_num:
                  BatteryOptions[charger_num].MCHT = (uint16_t) option;
                  BatteryOptions_f[charger_num].MCHT = (float) BatteryOptions[charger_num].MCHT;
                  battery_sr[charger_num] |= MCHT_pos;
                  break;
            case DMCHT1_num:
                  BatteryOptions[charger_num].DMCHT1 = (uint16_t) option;
                  BatteryOptions_f[charger_num].DMCHT1 = (float) BatteryOptions[charger_num].DMCHT1;
                  battery_sr[charger_num] |= DMCHT1_pos;
                  break;
            case DMCHT2_num:
                  BatteryOptions[charger_num].DMCHT2 = (uint16_t) option;
                  BatteryOptions_f[charger_num].DMCHT2 = (float) BatteryOptions[charger_num].DMCHT2;
                  battery_sr[charger_num] |= DMCHT2_pos;
                  break;
            case OCHI_num:
                  BatteryOptions[charger_num].OCHI = (uint16_t) option;
                  BatteryOptions_f[charger_num].OCHI = (float) BatteryOptions[charger_num].OCHI/10;
                  battery_sr[charger_num] |= OCHI_pos;
                  break;
            case OCHT_num:
                  BatteryOptions[charger_num].OCHT = (uint16_t) option;
                  BatteryOptions_f[charger_num].OCHT = (float) BatteryOptions[charger_num].OCHT;
                  battery_sr[charger_num] |= OCHT_pos;
                  break;
            case ECT_num:
                  BatteryOptions[charger_num].ECT = (uint16_t) option;
                  BatteryOptions_f[charger_num].ECT = (float) BatteryOptions[charger_num].ECT;
                  battery_sr[charger_num] |= ECT_pos;
                  break;
            case ECV_num:
                  BatteryOptions[charger_num].ECV = (uint16_t) option;
                  BatteryOptions_f[charger_num].ECV = (float) BatteryOptions[charger_num].ECV;
                  battery_sr[charger_num] |= ECV_pos;
                  break;
            case STI_num:
                  BatteryOptions[charger_num].STI = (uint16_t) option;
                  BatteryOptions_f[charger_num].STI = (float) BatteryOptions[charger_num].STI/10;
                  battery_sr[charger_num] |= STI_pos;
                  break;
            case STT_num:
                  BatteryOptions[charger_num].STT = (uint16_t) option;
                  BatteryOptions_f[charger_num].STT = (float) BatteryOptions[charger_num].STT;
                  battery_sr[charger_num] |= STT_pos;
                  break;
            case IDI_num:
                  BatteryOptions[charger_num].IDI = (uint16_t) option;
                  BatteryOptions_f[charger_num].IDI = (float) BatteryOptions[charger_num].IDI/10;
                  battery_sr[charger_num] |= IDI_pos;
                  break;
            case IDV_num:
                  BatteryOptions[charger_num].IDV = (uint16_t) option;
                  BatteryOptions_f[charger_num].IDV = (float) BatteryOptions[charger_num].IDV/10;
                  battery_sr[charger_num] |= IDV_pos;
                  break;
            case CTI_num:
                  BatteryOptions[charger_num].CTI = (uint16_t) option;
                  BatteryOptions_f[charger_num].CTI = (float) BatteryOptions[charger_num].CTI/10;
                  battery_sr[charger_num] |= CTI_pos;
                  break;
            case CTT_num:
                  BatteryOptions[charger_num].CTT = (uint16_t) option;
                  BatteryOptions_f[charger_num].CTT = (float) BatteryOptions[charger_num].CTT;
                  battery_sr[charger_num] |= CTT_pos;
                  break;
            case RDVR_num:
                  BatteryOptions[charger_num].RDVR = (uint16_t) option;
                  BatteryOptions_f[charger_num].RDVR = (float) BatteryOptions[charger_num].RDVR/1000;
                  battery_sr[charger_num] |= RDVR_pos;
                  break;
            case RDVS_num:
                  BatteryOptions[charger_num].RDVS = (uint16_t) option;
                  BatteryOptions_f[charger_num].RDVS = (float) BatteryOptions[charger_num].RDVS/1000;
                  battery_sr[charger_num] |= RDVS_pos;
                  break;
            case RDT_num:
                  BatteryOptions[charger_num].RDT = (uint16_t) option;
                  BatteryOptions_f[charger_num].RDT = (float) BatteryOptions[charger_num].RDT;
                  battery_sr[charger_num] |= RDT_pos;
                  break;
            case RDT1_num:
                  BatteryOptions[charger_num].RDT1 = (uint16_t) option;
                  BatteryOptions_f[charger_num].RDT1 = (float) BatteryOptions[charger_num].RDT1;
                  battery_sr[charger_num] |= RDT1_pos;
                  break;
            case RDT2_num:
                  BatteryOptions[charger_num].RDT2 = (uint16_t) option;
                  BatteryOptions_f[charger_num].RDT2 = (float) BatteryOptions[charger_num].RDT2;
                  battery_sr[charger_num] |= RDT2_pos;
                  break;
            case PV_num:
                  BatteryOptions[charger_num].PV = (uint16_t) option;
                  BatteryOptions_f[charger_num].PV = (float) BatteryOptions[charger_num].PV/10;
                  battery_sr[charger_num] |= PV_pos;
                  break;
            case TCD_num:
                  BatteryOptions[charger_num].TCD = (uint16_t) option;
                  BatteryOptions_f[charger_num].TCD = (float) BatteryOptions[charger_num].TCD;
                  battery_sr[charger_num] |= TCD_pos;
                  break;
            case VD_num:
                  BatteryOptions[charger_num].VD = (uint16_t) option;
                  BatteryOptions_f[charger_num].VD = (float) BatteryOptions[charger_num].VD/10;
                  battery_sr[charger_num] |= VD_pos;
                  break;
            case VCMAX_num:
                  BatteryOptions[charger_num].VCMAX = (uint16_t) option;
                  BatteryOptions_f[charger_num].VCMAX = (float) BatteryOptions[charger_num].VCMAX/1000;
                  battery_sr[charger_num] |= VCMAX_pos;
                  break;
            }
          }
          else
          {
            *(com_buffer+BATTERY_OPTIONS_OFFSET) = NOT_ACCEPTED;
          }
        }
        if (battery_sr[charger_num] == BATTERY_FULL_OPTIONS)
        {
          *(com_buffer+BATTERY_OPTIONS_OFFSET) = ACCEPTED;
        }
        else
        {
          *(com_buffer+BATTERY_OPTIONS_OFFSET) = NOT_ACCEPTED;
        }
        write(conn, (char *)com_buffer, L_SET_BATTERY_OPTIONS_CONF);
        return;
      }
      else
      {
        return;
      }
    }

    if ((*(com_buffer+1) == START_BATTERY_SERVICE) && (com_buf_len == L_START_BATTERY_SERVICE))
    {
      if ((*(com_buffer+2) == BATTERY1) || (*(com_buffer+2) == BATTERY2))
      {
        if (!(tests_state[*(com_buffer+2)]))
        {
          switch (*(com_buffer+3))
          {
          case T_FULL_SERVICE:
            tests_state[*(com_buffer+2)-1] |= FULL_SERVICE_pos;
            *(com_buffer+4) = ACCEPTED;
            write(conn, (char *)com_buffer, L_START_BATTERY_SERVICE_CONF);
//            close(conn);
            if (*(com_buffer+2) == BATTERY1)
            {
              xTaskCreate(vFULL_SERVICE_1, "FULL_SERVICE_1", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY, NULL );
            }
            else
            {
              xTaskCreate(vFULL_SERVICE_2, "FULL_SERVICE_2", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY, NULL );
            }
            break;
          case T_MAIN_CHARGE:
            tests_state[*(com_buffer+2)-1] |= MAIN_CHARGE_pos;
            *(com_buffer+4) = ACCEPTED;
            write(conn, (char *)com_buffer, L_START_BATTERY_SERVICE_CONF);
//            close(conn);
            if (*(com_buffer+2) == BATTERY1)
            {
              xTaskCreate(vMAIN_CHARGE_1, "MAIN_CHARGE_1", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY, NULL );
            }
            else
            {
              xTaskCreate(vMAIN_CHARGE_2, "MAIN_CHARGE_2", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY, NULL );
            }
            break;
          case T_OVERCHARGE:
            tests_state[*(com_buffer+2)-1] |= OVERCHARGE_pos;
            *(com_buffer+4) = ACCEPTED;
            write(conn, (char *)com_buffer, L_START_BATTERY_SERVICE_CONF);
//            close(conn);
            if (*(com_buffer+2) == BATTERY1)
            {
              xTaskCreate(vOVERCHARGE_1, "OVERCHARGE_1", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY, NULL );
            }
            else
            {
              xTaskCreate(vOVERCHARGE_2, "OVERCHARGE_2", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY, NULL );
            }
            break;
          case T_ELECTROLYTE_CHECK:
            tests_state[*(com_buffer+2)-1] |= ELECTROLYTE_CHECK_pos;
            *(com_buffer+4) = ACCEPTED;
            write(conn, (char *)com_buffer, L_START_BATTERY_SERVICE_CONF);
//            close(conn);
            if (*(com_buffer+2) == BATTERY1)
            {
              xTaskCreate(vELECTROLYTE_CHECK_1, "ELECTROLYTE_CHECK_1", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY, NULL );
            }
            else
            {
              xTaskCreate(vELECTROLYTE_CHECK_2, "ELECTROLYTE_CHECK_2", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY, NULL );
            }
            break;
          case T_SPECIAL_TEST:
            tests_state[*(com_buffer+2)-1] |= FULL_SERVICE_pos;
            *(com_buffer+4) = ACCEPTED;
            write(conn, (char *)com_buffer, L_START_BATTERY_SERVICE_CONF);
//            close(conn);
            if (*(com_buffer+2) == BATTERY1)
            {
              xTaskCreate(vSPECIAL_TEST_1, "SPECIAL_TEST_1", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY, NULL );
            }
            else
            {
              xTaskCreate(vSPECIAL_TEST_2, "SPECIAL_TEST_2", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY, NULL );
            }
            break;
          case T_INITIAL_DISCHARGE:
            tests_state[*(com_buffer+2)-1] |= INITIAL_DISCHARGE_pos;
            *(com_buffer+4) = ACCEPTED;
            write(conn, (char *)com_buffer, L_START_BATTERY_SERVICE_CONF);
//            close(conn);
            if (*(com_buffer+2) == BATTERY1)
            {
              xTaskCreate(vINITIAL_DISCHARGE_1, "INITIAL_DISCHARGE_1", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY, NULL );
            }
            else
            {
              xTaskCreate(vINITIAL_DISCHARGE_2, "INITIAL_DISCHARGE_2", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY, NULL );
            }
            break;
          case T_CAPACITY_TEST:
            tests_state[*(com_buffer+2)-1] |= CAPACITY_TEST_pos;
            *(com_buffer+4) = ACCEPTED;
            write(conn, (char *)com_buffer, L_START_BATTERY_SERVICE_CONF);
//            close(conn);
            if (*(com_buffer+2) == BATTERY1)
            {
              xTaskCreate(vCAPACITY_TEST_1, "CAPACITY_TEST_1", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY, NULL );
            }
            else
            {
              xTaskCreate(vCAPACITY_TEST_2, "CAPACITY_TEST_2", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY, NULL );
            }
            break;
          case T_RESIDUAL_DISCHARGE:
            tests_state[*(com_buffer+2)-1] |= RESIDUAL_DISCHARGE_pos;
            *(com_buffer+4) = ACCEPTED;
            write(conn, (char *)com_buffer, L_START_BATTERY_SERVICE_CONF);
//            close(conn);
            if (*(com_buffer+2) == BATTERY1)
            {
              xTaskCreate(vRESIDUAL_DISCHARGE_1, "RESIDUAL_DISCHARGE_1", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY, NULL );
            }
            else
            {
              xTaskCreate(vRESIDUAL_DISCHARGE_2, "RESIDUAL_DISCHARGE_2", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY, NULL );
            }
            break;
          }
          return;
        }
        else
        {
          *(com_buffer+4) = ALREADY_RUNNING;
          write(conn, (char *)com_buffer, L_START_BATTERY_SERVICE_CONF);
          return;
//          close(conn);
        }
      }
    }
    else
    {
      *(com_buffer+4) = NOT_ACCEPTED;
      write(conn, (char *)com_buffer, L_START_BATTERY_SERVICE_CONF);
      return;
//      close(conn);
    }
 
    if ((*(com_buffer+1) == FINALIZE_BATTERY_SERVICE) && (com_buf_len == L_FINALIZE_BATTERY_SERVICE))
    {
      if ((*(com_buffer+2) == BATTERY1) || (*(com_buffer+2) == BATTERY2))
      {
        CH_DCH_SET(CHARGER, 0);
        CH_DCH_SET(DISCHARGER, 0);
        ACH_Set(0);
        if (*(com_buffer+2) == BATTERY1)
        {
          GPIO_WriteBit(chdch_A_B_GPIO_Port, chdch_A_B_Pin, Bit_SET);
          ChargeState &= ~BATTERY1;
        }
        else
        {
          GPIO_WriteBit(chdch_A_B_GPIO_Port, chdch_A_B_Pin, Bit_RESET);
          ChargeState &= ~BATTERY2;
        }
        *(com_buffer+3) = ACCEPTED;
      }
      else
      {
        *(com_buffer+3) = NOT_ACCEPTED;
      }
      write(conn, (char *)com_buffer, L_FINALIZE_BATTERY_SERVICE_CONF);
      return;
//      close(conn);
    }
    
    if ((*(com_buffer+1) == PING) && (com_buf_len == L_PING))
    {
      write(conn, (char *)com_buffer, L_PING);
      return;
//      close(conn);
    }


  }

  if ((*(com_buffer) == MH_ID) || (*(com_buffer) == CAM_ID) || (*(com_buffer) == H1_ID) || (*(com_buffer) == H2_ID))
  {
    com_sr_cam = 0;
    cam_conf_answ_f = 0;
//    dma_tc_sr &= ~DMA_TRANSMITTED;
//    DMA2_Stream6->CR &= ~((uint32_t) DMA_SxCR_EN | DMA_SxCR_CHSEL);
    GPIO_WriteBit(RxTx_Switch_GPIO_Port, RxTx_Switch_Pin, Bit_SET);     // настраиваем RS485 на передачу
    DMA_ClearITPendingBit(DMA2_Stream6, DMA_IT_TCIF4 | DMA_IT_HTIF4 | DMA_IT_TEIF4 | DMA_IT_DMEIF4 | DMA_IT_FEIF4);
//    DMA2_Stream6->CR |= (uint32_t) DMA_Channel_5;
//    DMA1_Stream4->PAR = (uint32_t)(&(USART6->DR));
    DMA2_Stream6->M0AR =  (uint32_t) com_buffer;
    DMA2_Stream6->NDTR = (uint32_t) com_buf_len;
    DMA2_Stream6->CR |= (uint32_t) DMA_SxCR_EN;         // передаём команду в master_hood
    uint16_t timeout=0;
    do
    {
      vTaskDelay(1);
      if (com_sr_mh & com_hello)
      {
        com_sr_mh &=~com_hello;
//        *(com_buffer) = USART6BufferRx[0];
//        *(com_buffer+1) = USART6BufferRx[1];
//        *(com_buffer+2) = USART6BufferRx[2];
//        write(conn, (char *)com_buffer, l_hello);
        write(conn, &USART6BufferRx[0], l_hello);
//        close(conn);
        return;
      }
      if (com_sr_h1 & com_hello)
      {
        com_sr_h1 &=~com_hello;
        write(conn, &USART6BufferRx[0], l_hello);
        return;
      }
      if (com_sr_cam & COM_CAM_ACT)
      {
        com_sr_cam &=~COM_CAM_ACT;
        write(conn, &USART6BufferRx[0], L_CAM_ACT);
//        close(conn);
        return;
      }
      if (com_sr_cam & COM_CAM_WATCH_WIN)
      {
        com_sr_cam &=~COM_CAM_WATCH_WIN;
//        for (i=0; i<rs485_com_len; i++)
//        {
//        *(com_buffer+i) = USART6BufferRx[i];
//        }
//        write(conn, (char *)com_buffer, rs485_com_len);
        write(conn, &USART6BufferRx[0], rs485_com_len);
        return;
      }
      if (com_sr_cam & COM_CAM_RD_REG)
      {
        if ((cam_type_answ[CAM_RD_REG-1]&(CAM_TYPE_ANSW|CAM_TYPE_CONF))==(CAM_TYPE_ANSW|CAM_TYPE_CONF))
        {
          write(conn, &USART6AnswBuf[0], L_CAM_RD_REG_ANSW);
          cam_type_answ[CAM_RD_REG-1]=0;
          com_sr_cam &=~COM_CAM_RD_REG;
          return;
//          close(conn);
        }
        if ((!(cam_type_answ[CAM_RD_REG-1]&CAM_TYPE_CONF))&&(cam_type_answ[CAM_RD_REG-1]&CAM_TYPE_CONF))
        {
          write(conn, &USART6ConfBuf[0], L_CAM_RD_REG_CONF);
          cam_type_answ[CAM_RD_REG-1]|=CAM_TYPE_CONF;
          return;
//          close(conn);
        }
      }
      if (com_sr_cam & COM_CAM_WR_REG)
      {
        com_sr_cam &=~COM_CAM_WR_REG;
        write(conn, &USART6BufferRx[0], rs485_com_len);
        cam_type_answ[CAM_WR_REG-1]++;
        if (rs485_com_len==L_CAM_WR_REG_ANSW)
        {
          return;
        }
      }
      if (com_sr_cam & COM_CAM_GET_VOLUME)
      {
        com_sr_cam &=~COM_CAM_GET_VOLUME;
        write(conn, &USART6BufferRx[0], rs485_com_len);
        cam_type_answ[CAM_GET_VOLUME-1]++;
        if (rs485_com_len == L_CAM_GET_VOLUME_ANSW)
        {
          cam_type_answ[CAM_GET_VOLUME-1]=0;
          return;
        }
      }
      if (com_sr_cam & COM_CAM_GET_DENSITY)
      {
        com_sr_cam &=~COM_CAM_GET_DENSITY;
        write(conn, &USART6BufferRx[0], rs485_com_len);
        cam_type_answ[CAM_GET_DENSITY-1]++;
        if (rs485_com_len == L_CAM_GET_DENSITY_ANSW)
        {
          cam_type_answ[CAM_GET_DENSITY-1]=0;
          return;
        }
      }
      if (com_sr_cam & COM_CAM_FRAME_TO_MEM)
      {
        cam_conf_answ_f ^= COM_CAM_FRAME_TO_MEM;
        com_sr_cam &=~COM_CAM_FRAME_TO_MEM;
        write(conn, &USART6BufferRx[0], rs485_com_len);
//        if (rs485_com_len==L_CAM_FRAME_TO_MEM _ANSW)
//        {
//          close(conn);
//        }
        if (!(cam_conf_answ_f & COM_CAM_FRAME_TO_MEM))
        {
          return;
        }
      }
      if (com_sr_cam & COM_CAM_GET_CUR_WIN)
      {
// 3- сюда приходим, когда от камер пришло подтверждение приёма команды (12 байта AD 10 xx DXL DXH DYL DYH SL SH PL PH CS)
        com_sr_cam &=~COM_CAM_GET_CUR_WIN;
        GPIO_WriteBit(RxTx_Switch_GPIO_Port, RxTx_Switch_Pin, Bit_SET);     // настраиваем RS485 на передачу
// настраиваем DMA на приём пакета части изображения
        USART6->CR1 &= ~USART_CR1_RXNEIE;       // переходим в режим приёма по DMA, поэтому отключаем прерывание RXNEIE
        USART6->BRR = 0x00000010;               // переходим на скорость 5250000 бит/с
        USART6->CR1 |= USART_CR1_OVER8;         // удваиваем скорость до 10500000 бит/с
        USART6->CR3 &= ~(USART_CR3_DMAT | USART_CR3_DMAR);      // запрещаем прерывания на приём и передачу данных через USART6 по ДМА
        while (write(conn, &USART6BufferRx, L_CAM_GET_CUR_WIN_CONF)==ERR_INPROGRESS);
        DMA2_Stream1->CR &= ~DMA_SxCR_EN;
        while (DMA2_Stream1->CR & DMA_SxCR_EN); // DMA_SxCR_EN может изменить своё состояние не сразу
        DMA2_Stream1->M0AR = (uint32_t) &USART6FrameBuf;
        DMA2_Stream1->NDTR = Frame_Packet_Size+5;
        DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1 | DMA_IT_HTIF1 | DMA_IT_TEIF1 | DMA_IT_DMEIF1 | DMA_IT_FEIF1);
        DMA2_Stream1->CR |= DMA_SxCR_EN;
        rs485_f |= RS485_TX_BUSY;
        DMA_ClearITPendingBit(DMA2_Stream6, DMA_IT_TCIF6 | DMA_IT_HTIF6 | DMA_IT_TEIF6 | DMA_IT_DMEIF6 | DMA_IT_FEIF6);
// 4- подготовка к передаче ответа о готовности принимать данные изображения (6 байт AD 10 80 NL NH 00)
        USART6BufferTxCmd[0] = CAM_ID;
        USART6BufferTxCmd[1] = CAM_GET_CUR_WIN;
        USART6BufferTxCmd[2] = 0x80;
        USART6BufferTxCmd[3] = (uint8_t) (Frame_Packets & 0x00ff);
        USART6BufferTxCmd[4] = (uint8_t) (Frame_Packets>>8);
        USART6BufferTxCmd[5] = 0;
        USART6->CR3 |= USART_CR3_DMAT | USART_CR3_DMAR;         // разрешаем прерывания на приём и передачу данных через USART6 по ДМА
        DMA2_Stream6->M0AR =  (uint32_t) &USART6BufferTxCmd[0];
        DMA2_Stream6->NDTR = (uint32_t) L_CAM_GET_CUR_WIN_ANSW;
        DMA2_Stream6->CR |= (uint32_t) DMA_SxCR_EN;
        while (rs485_f & RS485_TX_BUSY);                        // ждём, пока данные запроса изображения не отправятся
        tim_frame = 2000;                                       // для приёма изображения выделяем времени 2 сек
        DMA2_Stream1->CR &= ~DMA_SxCR_CIRC;                     // запрещаем циклический режим приёма данных
        do
        {
          rs485_f &= ~RS485_RX_FULL;            // сбрасываем флаг принятых данных
          while (!(rs485_f & RS485_RX_FULL))    // ожидаем, когда поступит ответ от камеры
          {
            vTaskDelay(1);
            if (tim_frame)
            {
              tim_frame--;
            }
            else
            {
              USART6->CR1 |= USART_CR1_RXNEIE;
              USART6->CR1 &= ~USART_CR1_OVER8;
              USART6->BRR = USART6_BRR_Copy;
              USART6->CR3 &= ~USART_CR3_DMAR;
//              GPIO_WriteBit(RxTx_Switch_GPIO_Port, RxTx_Switch_Pin, Bit_RESET);
              return;
            }
          }
          USART6->CR3 &= ~(USART_CR3_DMAT | USART_CR3_DMAR);                    // запрещаем прерывания на приём и передачу данных через USART6 по ДМА
          gtmp16 = USART6FrameBuf[cur_buf][3] | (USART6FrameBuf[cur_buf][4]<<8); // из принятого пакета определяем количество оставшихся
//          while (write(conn, &USART6FrameBuf[cur_buf][0], Frame_Packet_Size+5)!=ERR_OK);
          write(conn, &USART6FrameBuf[cur_buf][0], (Frame_Packet_Size+5));      // отправляем принятый по USART6 пакет через ethernet
          USART6BufferTxCmd[3] = USART6FrameBuf[cur_buf][3];
          USART6BufferTxCmd[4] = USART6FrameBuf[cur_buf][4];
          USART6BufferTxCmd[5] = 0;
          for (i=0; i<L_CAM_GET_CUR_WIN_ANSW; i++)
          {
            USART6BufferTxCmd[L_CAM_GET_CUR_WIN_ANSW] ^= USART6BufferTxCmd[i]; // подсчитываем контрольную сумму
          }
//          DMA2_Stream6->M0AR =  (uint32_t) &USART6BufferTxCmd[0];
          GPIO_WriteBit(RxTx_Switch_GPIO_Port, RxTx_Switch_Pin, Bit_SET);       // настраиваем RS485 на передачу
          DMA2_Stream6->CR &= ~DMA_SxCR_EN;             // запрещаем передачу данных через ДМА
          while (DMA2_Stream6->CR & DMA_SxCR_EN);       // DMA_SxCR_EN может изменить своё состояние не сразу
          DMA_ClearITPendingBit(DMA2_Stream6, DMA_IT_TCIF6 | DMA_IT_HTIF6 | DMA_IT_TEIF6 | DMA_IT_DMEIF6 | DMA_IT_FEIF6);       // очищаем флаги прерываний
          DMA2_Stream6->NDTR = (uint32_t) L_CAM_GET_CUR_WIN_ANSW;
          rs485_f |= RS485_TX_BUSY;             // устанавливаем флаг занятости передатчика USART6
          DMA2_Stream6->CR |= DMA_SxCR_EN;      // запускаем передачу командного пакета
          USART6->CR3 |= USART_CR3_DMAT;
          while (rs485_f & RS485_TX_BUSY);      // ждём, пока передатчик не освободится (ожидаем окончания передачи командного пакета)
          DMA2_Stream1->CR &= ~DMA_SxCR_EN;     // запрещаем приём данных
          while (DMA2_Stream1->CR & DMA_SxCR_EN);       // DMA_SxCR_EN может изменить своё состояние не сразу
          DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1 | DMA_IT_HTIF1 | DMA_IT_TEIF1 | DMA_IT_DMEIF1 | DMA_IT_FEIF1);       // очищаем флаги прерываний
          DMA2_Stream1->M0AR = (uint32_t) &USART6FrameBuf[1-cur_buf][0];        // выбираем, в какой буфер будут приниматься данные
          DMA2_Stream1->NDTR = Frame_Packet_Size+5;     // устанавливаем размер принимаемых данных на 5 больше размера пакета данных (5 байт служебная информация)
          DMA2_Stream1->CR |= DMA_SxCR_EN;      // запускаем приём данных
          USART6->CR3 |= USART_CR3_DMAR;        // разрешаем прерывание на приём пакета данных
          cur_buf = 1-cur_buf;                  // меняем приёмные буферы местами
        } while (gtmp16);                        // 
        gtmp32++;
        tim_frame = 0;
        USART6->CR1 |= USART_CR1_RXNEIE;
        USART6->CR1 &= ~USART_CR1_OVER8;
        USART6->BRR = USART6_BRR_Copy;
        USART6->CR3 &= ~USART_CR3_DMAR;
        rs485_f &= ~RS485_RX_FULL;
        return;
      }
      if (com_sr_cam & COM_CAM_DEF_LABELS)
      {
        com_sr_cam &=~COM_CAM_DEF_LABELS;
        write(conn, &USART6BufferRx[0], rs485_com_len);
        cam_type_answ[CAM_DEF_LABELS-1]++;
        if (rs485_com_len == L_CAM_DEF_LABELS_ANSW)
        {
          cam_type_answ[CAM_DEF_LABELS-1]=0;
          return;
        }
      }

      timeout++;
    } while (timeout<TIMEOUT_CAM);
    com_sr_cam = 0;
    USART6->CR1 |= USART_CR1_RXNEIE;
    USART6->CR1 &= ~USART_CR1_OVER8;
    USART6->BRR = USART6_BRR_Copy;
    for (i=0; i<NUM_COM_CAM; i++)
    {
      cam_type_answ[i] = 0;
    }
//    close(conn);
  }

  if (*(com_buffer) == ACHA_ID)
  {
    com_sr_acha = 0;
    acha_conf_answ_f = 0;
    DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_TCIF4 | DMA_IT_HTIF4 | DMA_IT_TEIF4 | DMA_IT_DMEIF4 | DMA_IT_FEIF4);
    DMA1_Stream3->M0AR =  (uint32_t) com_buffer;
    DMA1_Stream3->NDTR = (uint32_t) com_buf_len;
    DMA1_Stream3->CR |= (uint32_t) DMA_SxCR_EN;         // передаём команду в Analog Charger A
    uint16_t timeout=0;
    do
    {
      vTaskDelay(1);
      if (com_sr_acha & com_hello)
      {
        com_sr_acha &=~com_hello;
        write(conn, &AchaBufferRx[0], l_hello);
        return;
      }
      if (com_sr_acha & COM_ACH_SET_CUR)
      {
        com_sr_acha &=~COM_ACH_SET_CUR;
        write(conn, &AchaBufferRx[0], L_ACH_SET_CUR);
        return;
      }
      timeout++;
    } while (timeout<TIMEOUT_ACH);
    com_sr_acha = 0;
    for (i=0; i<NUM_COM_ACH; i++)
    {
      acha_type_answ[i] = 0;
    }
    close(conn);
  }

  if (*(com_buffer) == ADCH_ID)
  {
    com_sr_adch = 0;
    adch_conf_answ_f = 0;
    DMA_ClearITPendingBit(DMA1_Stream7, DMA_IT_TCIF4 | DMA_IT_HTIF4 | DMA_IT_TEIF4 | DMA_IT_DMEIF4 | DMA_IT_FEIF4);
    DMA1_Stream7->M0AR =  (uint32_t) com_buffer;
    DMA1_Stream7->NDTR = (uint32_t) com_buf_len;
    DMA1_Stream7->CR |= (uint32_t) DMA_SxCR_EN;         // передаём команду в Analog Discharger
    uint16_t timeout=0;
    do
    {
      vTaskDelay(1);
      if (com_sr_adch & com_hello)
      {
        com_sr_adch &=~com_hello;
        write(conn, &AdchBufferRx[0], l_hello);
        return;
      }
      if (com_sr_adch & COM_ADCH_SET_CUR)
      {
        com_sr_adch &=~COM_ADCH_SET_CUR;
        write(conn, &AdchBufferRx[0], L_ADCH_SET_CUR);
        return;
      }
      timeout++;
    } while (timeout<TIMEOUT_ADCH);
    com_sr_adch = 0;
    for (i=0; i<NUM_COM_ADCH; i++)
    {
      adch_type_answ[i] = 0;
    }
    close(conn);
    
  }

#endif
 }

COM_StatusTypeDef stoui(char *s, uint16_t *value, uint8_t len)
{
  uint8_t tmp8;
  uint8_t mult = len;
  uint16_t tmpi = 0;
  for (char i = 0; i < len; i++)
  {
    tmp8 = (uint8_t)(*(s+i)) - 0x30;
    switch (mult)
    {
    case 0:
      break;
    case 1:
      tmpi += tmp8;
      break;
    case 2:
      tmpi += tmp8*10;
      break;
    case 3:
      tmpi += tmp8*100;
      break;
    case 4:
      tmpi += tmp8*1000;
      break;
    case 5:
      tmpi += tmp8*10000;
      break;
    }
    mult--;
  }
  *value = tmpi;
  return COM_OK;
}

void vTestSMA(void *params)
{
  SM1_ENABLE;
  uint32_t steps = uint32_t(opu_coordinates.latitude) * opu_stepper_motor.mode;
  for (uint32_t i = 0; i < steps; i++)
  {
    SM1_STEP_HI;
    vTaskDelay(1);
    SM1_STEP_LO;
    vTaskDelay(1);
  }
  SM1_DISABLE;
  vTaskDelete(NULL);
}

void vStepMotors(void *params)
{
  uint32_t steps;
  SM1_ENABLE;
  SM2_ENABLE;
  SM3_ENABLE;
  switch (sm_data.data_type)
  {
  case OPTICAL_STEPS:
    if (sm_data.steps < 0) 
    {
      GPIO_WriteBit(SM1_DIR_GPIO_Port, SM1_DIR_Pin, Bit_SET);
      GPIO_WriteBit(SM2_DIR_GPIO_Port, SM2_DIR_Pin, Bit_SET);
      GPIO_WriteBit(SM3_DIR_GPIO_Port, SM3_DIR_Pin, Bit_SET);
    }
    else
    {
      GPIO_WriteBit(SM1_DIR_GPIO_Port, SM1_DIR_Pin, Bit_RESET);
      GPIO_WriteBit(SM2_DIR_GPIO_Port, SM2_DIR_Pin, Bit_RESET);
      GPIO_WriteBit(SM3_DIR_GPIO_Port, SM3_DIR_Pin, Bit_RESET);
    }
    steps = (uint32_t)(abs(sm_data.steps));
    break;
  case OPTICAL_ANGLE:
    if (sm_data.angle < 0) 
    {
      GPIO_WriteBit(SM1_DIR_GPIO_Port, SM1_DIR_Pin, Bit_SET);
      GPIO_WriteBit(SM2_DIR_GPIO_Port, SM2_DIR_Pin, Bit_SET);
      GPIO_WriteBit(SM3_DIR_GPIO_Port, SM3_DIR_Pin, Bit_SET);
    }
    else
    {
      GPIO_WriteBit(SM1_DIR_GPIO_Port, SM1_DIR_Pin, Bit_RESET);
      GPIO_WriteBit(SM2_DIR_GPIO_Port, SM2_DIR_Pin, Bit_RESET);
      GPIO_WriteBit(SM3_DIR_GPIO_Port, SM3_DIR_Pin, Bit_RESET);
    }
    steps = (uint32_t)(abs((int32_t)(sm_data.angle/1.8)));
    break;
  }

  vTaskDelay(500);
  for (uint32_t i = 0; i < 4 * steps; i++)
  {
    SM1_STEP_HI;
    SM2_STEP_HI;
    SM3_STEP_HI;
    vTaskDelay(1);
    SM1_STEP_LO;
    SM2_STEP_LO;
    SM3_STEP_LO;
    vTaskDelay(1);
  }
  SM1_DISABLE;
  SM2_DISABLE;
  SM3_DISABLE;
  vTaskDelete(NULL);
}

