/**
  ******************************************************************************
  * ������, ������������� ��������� ���������.
  * ��� ������ ���������� ������ ��������� (��������� �������� � Periph_State),
  * ����� ���������� ��������� �����, ������������ ��� ���������.
  * ����� ����� ���������� ��� �������������������� ���������, ��� �����
  * ������� �� �������� ���������� �� �����������������.
  ******************************************************************************
  */

#include "stm32f4xx.h"
#include "periph_watchdog.h"
#include "FreeRTOS.h"
#include "task.h"
#include "net_bridge.h"
#include "mcp3422.h"
#include "stm32f4x7_eth.h"
#include "stm32f4x7_eth_bsp.h"

uint32_t Periph_State=0;        // ������� ��������� ��������� (0- ������ ��������, 1- ������ ����������)
extern TaskHandle_t xADC;
extern uint8_t mcp3422_sr; 
extern uint8_t MCP3422_Calc_Fail;

void vPERIPH_WATCHDOG(void *params)
{
  eTaskState eState;
  uint8_t i;

  for (;;)
  {
    for (i=0; i<32; i++)
    {
      switch (i)
      {
      case PS_ADC_STATE_Pos:
        if (Periph_State & PS_ADC_STATE)
        {
          eState = eTaskGetState(xADC);
          if ((eState == eBlocked) || (eState == eReady))
          {
            vTaskSuspend(xADC);
          }
        }
        break;
      }
    }

/* ��������� ����������� ��� MCP3422 �� ���� I2C1 */    
    if (mcp3422_sr)
    {
      if (MCP3422_Calc_Fail <= MCP3422_FAIL_LIMIT)
      {
        if (ADC_Config()!=ERROR)
        {
          vTaskResume(xADC);
        }
        else
        {
          ADC_Init();
        }
      }
      else
      {
        Periph_State |= PS_ADC_STATE;
      }
        

    }

/* ��������� ����������� ������ ethernet */    
    if (ETH_ReadPHYRegister(DP83848_PHY_ADDRESS, PHY_SR) & 0x0001)
    {
      Periph_State &= ~PS_ETH_STATE;
    }
    else
    {
      Periph_State |= PS_ETH_STATE;
/*
      extern TaskHandle_t xNetBridge;
      TaskHandle_t m_ethTaskHandle;
      vTaskDelete(xNetBridge);
      vTaskDelete(m_ethTaskHandle);
      netBridge.configure();
*/
    }

    vTaskDelay(100);
  }
}