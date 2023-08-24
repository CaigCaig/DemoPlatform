#include "stm32f4xx.h"
#include "com_handler.h"
#include "FreeRTOS.h"
#include "task.h"
#include "leds.h"
#include "mb_def.h"
#include "net_bridge.h"
#include "mcp3422.h"
#include "rs485.h"
#include "ch_dch.h"
#include "ach_adch.h"
#include "hood.h"
#include "24AA025E48.h"
#include "batteries.h"
#include "sockets.h"

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
uint32_t com_sr_mh=0;           // ��������� ������� �������� ������ ��� master_hood
uint32_t com_sr_h1=0;           // ��������� ������� �������� ������ ��� hood1
uint32_t com_sr_h2=0;           // ��������� ������� �������� ������ ��� hood2
uint32_t com_sr_cam=0;          // ��������� ������� �������� ������ ��� ����������
uint32_t cam_conf_answ_f=0;     // ������� �������� �������������/������ ������� ��� ����������
extern uint8_t USART6BufferTxCmd[USART6_CMD_LEN], USART6BufferRx[USART6_CMD_LEN], rs485_com_len, rs485_f;
extern uint8_t USART6ConfBuf[USART6_CONF_LEN], USART6AnswBuf[USART6_ANSW_LEN];
extern uint8_t cam_type_answ[NUM_COM_CAM];
extern char USART6FrameBuf[2][FRAME_PACKET_MAXSIZE];
extern uint32_t USART6_BRR_Copy;
extern uint8_t cur_buf;
extern uint16_t Frame_Packets, tim_frame;
extern uint32_t Frame_Packet_Size;

uint32_t com_sr_adch=0;         // ��������� ������� �������� ������ ��� ADCH
uint32_t com_sr_acha=0;         // ��������� ������� �������� ������ ��� ACHA
uint32_t com_sr_achb=0;         // ��������� ������� �������� ������ ��� ACHB
uint32_t acha_conf_answ_f=0;    // ������� �������� �������������/������ ������� ��� ����������� ��������� A
uint32_t achb_conf_answ_f=0;    // ������� �������� �������������/������ ������� ��� ����������� ��������� B
uint32_t adch_conf_answ_f=0;    // ������� �������� �������������/������ ������� ��� ����������� ������������
extern uint8_t AchaBufferTxCmd[ACHA_CMD_LEN], AchaBufferRx[ACHA_CMD_LEN], acha_com_len, acha_f;
extern uint8_t AchbBufferTxCmd[ACHB_CMD_LEN], AchbBufferRx[ACHB_CMD_LEN], achb_com_len, achb_f;
extern uint8_t AdchBufferTxCmd[ADCH_CMD_LEN], AdchBufferRx[ADCH_CMD_LEN], adch_com_len, adch_f;
extern uint8_t acha_type_answ[NUM_COM_ACH], achb_type_answ[NUM_COM_ACH], adch_type_answ[NUM_COM_ADCH];

extern BatTest_t bat_test;

uint16_t gtmp16;
uint32_t gtmp32=0;

void Com_Handler (int conn, char *com_buffer, uint16_t com_buf_len)
{
//  uint8_t Temp_Buf[20];
  uint8_t tmp, i;
    
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
//      SCB->AIRCR = 0x05FA0004;  // ������ ����������� ����� ���������������� ��� ��������� ��������� �������� ����

    }

    if ((*(com_buffer+1) == SET_BATTERY_OPTIONS) && (com_buf_len == L_SET_BATTERY_OPTIONS))
    {
      uint8_t charger_num = *(com_buffer+2);
      if ((charger_num == BATTERY1) || (charger_num == BATTERY2))
      {
        charger_num--;  // ������������ ��������� � �������, ��������� � ����
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
    GPIO_WriteBit(RxTx_Switch_GPIO_Port, RxTx_Switch_Pin, Bit_SET);     // ����������� RS485 �� ��������
    DMA_ClearITPendingBit(DMA2_Stream6, DMA_IT_TCIF4 | DMA_IT_HTIF4 | DMA_IT_TEIF4 | DMA_IT_DMEIF4 | DMA_IT_FEIF4);
//    DMA2_Stream6->CR |= (uint32_t) DMA_Channel_5;
//    DMA1_Stream4->PAR = (uint32_t)(&(USART6->DR));
    DMA2_Stream6->M0AR =  (uint32_t) com_buffer;
    DMA2_Stream6->NDTR = (uint32_t) com_buf_len;
    DMA2_Stream6->CR |= (uint32_t) DMA_SxCR_EN;         // ������� ������� � master_hood
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
// 3- ���� ��������, ����� �� ����� ������ ������������� ����� ������� (12 ����� AD 10 xx DXL DXH DYL DYH SL SH PL PH CS)
        com_sr_cam &=~COM_CAM_GET_CUR_WIN;
        GPIO_WriteBit(RxTx_Switch_GPIO_Port, RxTx_Switch_Pin, Bit_SET);     // ����������� RS485 �� ��������
// ����������� DMA �� ���� ������ ����� �����������
        USART6->CR1 &= ~USART_CR1_RXNEIE;       // ��������� � ����� ����� �� DMA, ������� ��������� ���������� RXNEIE
        USART6->BRR = 0x00000010;               // ��������� �� �������� 5250000 ���/�
        USART6->CR1 |= USART_CR1_OVER8;         // ��������� �������� �� 10500000 ���/�
        USART6->CR3 &= ~(USART_CR3_DMAT | USART_CR3_DMAR);      // ��������� ���������� �� ���� � �������� ������ ����� USART6 �� ���
        while (write(conn, &USART6BufferRx, L_CAM_GET_CUR_WIN_CONF)==ERR_INPROGRESS);
        DMA2_Stream1->CR &= ~DMA_SxCR_EN;
        while (DMA2_Stream1->CR & DMA_SxCR_EN); // DMA_SxCR_EN ����� �������� ��� ��������� �� �����
        DMA2_Stream1->M0AR = (uint32_t) &USART6FrameBuf;
        DMA2_Stream1->NDTR = Frame_Packet_Size+5;
        DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1 | DMA_IT_HTIF1 | DMA_IT_TEIF1 | DMA_IT_DMEIF1 | DMA_IT_FEIF1);
        DMA2_Stream1->CR |= DMA_SxCR_EN;
        rs485_f |= RS485_TX_BUSY;
        DMA_ClearITPendingBit(DMA2_Stream6, DMA_IT_TCIF6 | DMA_IT_HTIF6 | DMA_IT_TEIF6 | DMA_IT_DMEIF6 | DMA_IT_FEIF6);
// 4- ���������� � �������� ������ � ���������� ��������� ������ ����������� (6 ���� AD 10 80 NL NH 00)
        USART6BufferTxCmd[0] = CAM_ID;
        USART6BufferTxCmd[1] = CAM_GET_CUR_WIN;
        USART6BufferTxCmd[2] = 0x80;
        USART6BufferTxCmd[3] = (uint8_t) (Frame_Packets & 0x00ff);
        USART6BufferTxCmd[4] = (uint8_t) (Frame_Packets>>8);
        USART6BufferTxCmd[5] = 0;
        USART6->CR3 |= USART_CR3_DMAT | USART_CR3_DMAR;         // ��������� ���������� �� ���� � �������� ������ ����� USART6 �� ���
        DMA2_Stream6->M0AR =  (uint32_t) &USART6BufferTxCmd[0];
        DMA2_Stream6->NDTR = (uint32_t) L_CAM_GET_CUR_WIN_ANSW;
        DMA2_Stream6->CR |= (uint32_t) DMA_SxCR_EN;
        while (rs485_f & RS485_TX_BUSY);                        // ���, ���� ������ ������� ����������� �� ����������
        tim_frame = 2000;                                       // ��� ����� ����������� �������� ������� 2 ���
        DMA2_Stream1->CR &= ~DMA_SxCR_CIRC;                     // ��������� ����������� ����� ����� ������
        do
        {
          rs485_f &= ~RS485_RX_FULL;            // ���������� ���� �������� ������
          while (!(rs485_f & RS485_RX_FULL))    // �������, ����� �������� ����� �� ������
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
          USART6->CR3 &= ~(USART_CR3_DMAT | USART_CR3_DMAR);                    // ��������� ���������� �� ���� � �������� ������ ����� USART6 �� ���
          gtmp16 = USART6FrameBuf[cur_buf][3] | (USART6FrameBuf[cur_buf][4]<<8); // �� ��������� ������ ���������� ���������� ����������
//          while (write(conn, &USART6FrameBuf[cur_buf][0], Frame_Packet_Size+5)!=ERR_OK);
          write(conn, &USART6FrameBuf[cur_buf][0], (Frame_Packet_Size+5));      // ���������� �������� �� USART6 ����� ����� ethernet
          USART6BufferTxCmd[3] = USART6FrameBuf[cur_buf][3];
          USART6BufferTxCmd[4] = USART6FrameBuf[cur_buf][4];
          USART6BufferTxCmd[5] = 0;
          for (i=0; i<L_CAM_GET_CUR_WIN_ANSW; i++)
          {
            USART6BufferTxCmd[L_CAM_GET_CUR_WIN_ANSW] ^= USART6BufferTxCmd[i]; // ������������ ����������� �����
          }
//          DMA2_Stream6->M0AR =  (uint32_t) &USART6BufferTxCmd[0];
          GPIO_WriteBit(RxTx_Switch_GPIO_Port, RxTx_Switch_Pin, Bit_SET);       // ����������� RS485 �� ��������
          DMA2_Stream6->CR &= ~DMA_SxCR_EN;             // ��������� �������� ������ ����� ���
          while (DMA2_Stream6->CR & DMA_SxCR_EN);       // DMA_SxCR_EN ����� �������� ��� ��������� �� �����
          DMA_ClearITPendingBit(DMA2_Stream6, DMA_IT_TCIF6 | DMA_IT_HTIF6 | DMA_IT_TEIF6 | DMA_IT_DMEIF6 | DMA_IT_FEIF6);       // ������� ����� ����������
          DMA2_Stream6->NDTR = (uint32_t) L_CAM_GET_CUR_WIN_ANSW;
          rs485_f |= RS485_TX_BUSY;             // ������������� ���� ��������� ����������� USART6
          DMA2_Stream6->CR |= DMA_SxCR_EN;      // ��������� �������� ���������� ������
          USART6->CR3 |= USART_CR3_DMAT;
          while (rs485_f & RS485_TX_BUSY);      // ���, ���� ���������� �� ����������� (������� ��������� �������� ���������� ������)
          DMA2_Stream1->CR &= ~DMA_SxCR_EN;     // ��������� ���� ������
          while (DMA2_Stream1->CR & DMA_SxCR_EN);       // DMA_SxCR_EN ����� �������� ��� ��������� �� �����
          DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1 | DMA_IT_HTIF1 | DMA_IT_TEIF1 | DMA_IT_DMEIF1 | DMA_IT_FEIF1);       // ������� ����� ����������
          DMA2_Stream1->M0AR = (uint32_t) &USART6FrameBuf[1-cur_buf][0];        // ��������, � ����� ����� ����� ����������� ������
          DMA2_Stream1->NDTR = Frame_Packet_Size+5;     // ������������� ������ ����������� ������ �� 5 ������ ������� ������ ������ (5 ���� ��������� ����������)
          DMA2_Stream1->CR |= DMA_SxCR_EN;      // ��������� ���� ������
          USART6->CR3 |= USART_CR3_DMAR;        // ��������� ���������� �� ���� ������ ������
          cur_buf = 1-cur_buf;                  // ������ ������� ������ �������
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
    DMA1_Stream3->CR |= (uint32_t) DMA_SxCR_EN;         // ������� ������� � Analog Charger A
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
    DMA1_Stream7->CR |= (uint32_t) DMA_SxCR_EN;         // ������� ������� � Analog Discharger
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

//  for(i=0; i<=com_buf_len; i++)
//  {
//    Temp_Buf[i]=*(com_buffer+i);
//  }
}