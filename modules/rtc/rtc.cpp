/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */
#include "FreeRTOS.h"
#include "task.h"
#include "device_commands.h"
#include "rtc.h"

/* Private macros */
/* Internal status registers for RTC */
#define RTC_STATUS_REG      			RTC_BKP_DR19 /* Status Register */
#define RTC_STATUS_INIT_OK  			0x1234       /* RTC initialised */
#define RTC_STATUS_TIME_OK  			0x4321       /* RTC time OK */
#define	RTC_STATUS_ZERO					0x0000

/* Internal RTC defines */
#define TM_RTC_LEAP_YEAR(year) 			((((year) % 4 == 0) && ((year) % 100 != 0)) || ((year) % 400 == 0))
#define TM_RTC_DAYS_IN_YEAR(x)			TM_RTC_LEAP_YEAR(x) ? 366 : 365
#define TM_RTC_OFFSET_YEAR				1970
#define TM_RTC_SECONDS_PER_DAY			86400
#define TM_RTC_SECONDS_PER_HOUR			3600
#define TM_RTC_SECONDS_PER_MINUTE		60
#define TM_RTC_BCD2BIN(x)				((((x) >> 4) & 0x0F) * 10 + ((x) & 0x0F))
#define TM_RTC_CHAR2NUM(x)				((x) - '0')
#define TM_RTC_CHARISNUM(x)				((x) >= '0' && (x) <= '9')

/* Internal functions */
void TM_RTC_Config(TM_RTC_ClockSource_t source);
/* Default RTC status */
uint32_t TM_RTC_Status = RTC_STATUS_ZERO;
/* RTC declarations */
RTC_TimeTypeDef RTC_TimeStruct;
RTC_InitTypeDef RTC_InitStruct;
RTC_DateTypeDef RTC_DateStruct;
NVIC_InitTypeDef NVIC_InitStruct;
EXTI_InitTypeDef EXTI_InitStruct;

RTC_InitTypeDef   RTC_InitStructure;
RTC_TimeTypeDef   RTC_TimeStructure;
RTC_DateTypeDef   RTC_DateStructure;

#define RTC_CLOCK_SOURCE_LSE           /* LSE used as RTC source clock */
#define FIRST_DATA          0x32F2
#define RTC_BKP_DR_NUMBER   0x14

__IO uint32_t uwAsynchPrediv = 0;
__IO uint32_t uwSynchPrediv = 0;
__IO uint32_t uwTimeDisplay = 0;

uint32_t uwErrorIndex = 0;
uint32_t uwIndex = 0;

uint32_t aBKPDataReg[RTC_BKP_DR_NUMBER] =
  {
    RTC_BKP_DR0, RTC_BKP_DR1, RTC_BKP_DR2, 
    RTC_BKP_DR3, RTC_BKP_DR4, RTC_BKP_DR5,
    RTC_BKP_DR6, RTC_BKP_DR7, RTC_BKP_DR8, 
    RTC_BKP_DR9, RTC_BKP_DR10, RTC_BKP_DR11, 
    RTC_BKP_DR12, RTC_BKP_DR13, RTC_BKP_DR14, 
    RTC_BKP_DR15, RTC_BKP_DR16, RTC_BKP_DR17, 
    RTC_BKP_DR18,  RTC_BKP_DR19
  };

/* Days in a month */
uint8_t TM_RTC_Months[2][12] = {
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},	/* Not leap year */
	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}	/* Leap year */
};

/**
  * @brief  Writes data to all Backup data registers.
  * @param  FirstBackupData: data to write to first backup data register.
  * @retval None
  */
static void WriteToBackupReg(uint16_t FirstBackupData)
{
  uint32_t index = 0;

  for (index = 0; index < RTC_BKP_DR_NUMBER; index++)
  {
    RTC_WriteBackupRegister(aBKPDataReg[index], FirstBackupData + (index * 0x5A));
  }

}

uint32_t TM_RTC_Init(TM_RTC_ClockSource_t source) 
{
  uint32_t status;
  uint8_t stat = 1;
  TM_RTC_t datatime;
	
  /* Enable PWR peripheral clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);
	
  /* Get RTC status */
  status = RTC_ReadBackupRegister(RTC_STATUS_REG);
	
  if (status == RTC_STATUS_TIME_OK) 
  {
    TM_RTC_Status = RTC_STATUS_TIME_OK;
		
    /* Start internal clock if we choose internal clock */
    switch (source)
    {
    case TM_RTC_ClockSource_Internal:
      TM_RTC_Config(source);
      break;
    case TM_RTC_ClockSource_HSE:
      TM_RTC_Config(source);
      break;
    }

    /* Wait for RTC APB registers synchronisation (needed after start-up from Reset) */
    RTC_WaitForSynchro();
		
    /* Clear interrupt flags */
    RTC_ClearITPendingBit(RTC_IT_WUT);
    EXTI->PR = 0x00400000;
		
    /* Get date and time */
    TM_RTC_GetDateTime(&datatime, TM_RTC_Format_BIN);
  } 
  else if (status == RTC_STATUS_INIT_OK) 
  {
    TM_RTC_Status = RTC_STATUS_INIT_OK;
		
    /* Start internal clock if we choose internal clock */
    switch (source)
    {
    case TM_RTC_ClockSource_Internal:
      TM_RTC_Config(source);
      break;
    case TM_RTC_ClockSource_HSE:
      TM_RTC_Config(source);
      break;
    }

    /* Wait for RTC APB registers synchronisation (needed after start-up from Reset) */
    RTC_WaitForSynchro();
		
    /* Clear interrupt flags */
    RTC_ClearITPendingBit(RTC_IT_WUT);
    EXTI->PR = 0x00400000;
		
    /* Get date and time */
    //TM_RTC_GetDateTime(&datatime, TM_RTC_Format_BIN);
  } 
  else 
  {
    TM_RTC_Status = RTC_STATUS_ZERO;
    /* Return status = 0 -> RTC Never initialized before */
    stat = RTC_STATUS_ZERO;
    /* Config RTC */
    TM_RTC_Config(source);
		
    /* Set date and time */
    datatime.date = 1;
    datatime.day = 1;
    datatime.month = 1;
    datatime.year = 0;
    datatime.hours = 0;
    datatime.minutes = 0;
    datatime.seconds = 0;

    /* Set date and time */
    TM_RTC_SetDateTime(&datatime, TM_RTC_Format_BIN);
		
    /* Initialized OK */
    TM_RTC_Status = RTC_STATUS_INIT_OK;
  }
  /* If first time initialized */
  if (stat == RTC_STATUS_ZERO) 
	return 0;
  
  return TM_RTC_Status;  

}

void TM_RTC_SetDataTimeUnix(uint32_t unixTime)
{
  TM_RTC_t data;
  
  TM_RTC_GetDateTimeFromUnix(&data, unixTime);
  TM_RTC_SetDateTime(&data, TM_RTC_Format_BIN);
}

/**
  * @brief  Кореектировака времени.
  * @param  Часовой пояс
  * @retval 
  */
void TM_RTC_Correction(float utc)
{
  TM_RTC_t data;
  TM_RTC_t newData;
  int      utcSec;
  uint32_t unixTime;
  
  /* Получаем текущее время */
  TM_RTC_GetDateTime(&data, TM_RTC_Format_BIN);
  
  /* Рассчитываем поправку */
  utcSec = (int)(3600.0*utc);
  unixTime = data.unix + utcSec;
  
  /* Устанавливаем новое время */
  TM_RTC_GetDateTimeFromUnix(&newData, unixTime);
  TM_RTC_SetDateTime(&newData, TM_RTC_Format_BIN);
}

TM_RTC_Result_t TM_RTC_SetDateTime(TM_RTC_t* data, TM_RTC_Format_t format) {
	TM_RTC_t tmp;
	
	/* Check date and time validation */
	if (format == TM_RTC_Format_BCD) {
		tmp.date = TM_RTC_BCD2BIN(data->date);
		tmp.month = TM_RTC_BCD2BIN(data->month);
		tmp.year = TM_RTC_BCD2BIN(data->year);
		tmp.hours = TM_RTC_BCD2BIN(data->hours);
		tmp.minutes = TM_RTC_BCD2BIN(data->minutes);
		tmp.seconds = TM_RTC_BCD2BIN(data->seconds);
		tmp.day = TM_RTC_BCD2BIN(data->day);
	} else {
		tmp.date = data->date;
		tmp.month = data->month;
		tmp.year = data->year;
		tmp.hours = data->hours;
		tmp.minutes = data->minutes;
		tmp.seconds = data->seconds;
		tmp.day = data->day;
	}
	
	/* Check year and month */
	if (
		tmp.year > 99 || 
		tmp.month == 0 || 
		tmp.month > 12 ||
		tmp.date == 0 ||
		tmp.date > TM_RTC_Months[TM_RTC_LEAP_YEAR(2000 + tmp.year) ? 1 : 0][tmp.month - 1] ||
		tmp.hours > 23 ||
		tmp.minutes > 59 ||
		tmp.seconds > 59 ||
		tmp.day == 0 ||
		tmp.day > 7
	) {
		/* Invalid date */
		return TM_RTC_Result_Error; 
	}
	
	/* Fill time */
	RTC_TimeStruct.RTC_Hours = data->hours;
	RTC_TimeStruct.RTC_Minutes = data->minutes;
	RTC_TimeStruct.RTC_Seconds = data->seconds;
	/* Fill date */
	RTC_DateStruct.RTC_Date = data->date;
	RTC_DateStruct.RTC_Month = data->month;
	RTC_DateStruct.RTC_Year = data->year;
	RTC_DateStruct.RTC_WeekDay = data->day;
	
	/* Set the RTC time base to 1s and hours format to 24h */
	RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;
	RTC_InitStruct.RTC_AsynchPrediv = RTC_ASYNC_PREDIV;
	RTC_InitStruct.RTC_SynchPrediv = RTC_SYNC_PREDIV;
	RTC_Init(&RTC_InitStruct);

	/* Set time */
	if (format == TM_RTC_Format_BCD) {
		RTC_SetTime(RTC_Format_BCD, &RTC_TimeStruct);
	} else {
		RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
	}
	
	/* Set date */
	if (format == TM_RTC_Format_BCD) {
		RTC_SetDate(RTC_Format_BCD, &RTC_DateStruct);
	} else {
		RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);
	}	
	
	if (TM_RTC_Status != RTC_STATUS_ZERO) {
		/* Write backup registers */
		RTC_WriteBackupRegister(RTC_STATUS_REG, RTC_STATUS_TIME_OK);
	}
	
	/* Return OK */
	return TM_RTC_Result_Ok;
}

TM_RTC_Result_t TM_RTC_SetDateTimeString(char* str) {
	TM_RTC_t tmp;
	uint8_t i = 0;
	
	/* Get date */
	tmp.date = 0;
	while (TM_RTC_CHARISNUM(*(str + i))) {
		tmp.date = tmp.date * 10 + TM_RTC_CHAR2NUM(*(str + i));
		i++;
	}
	i++;
	
	/* Get month */
	tmp.month = 0;
	while (TM_RTC_CHARISNUM(*(str + i))) {
		tmp.month = tmp.month * 10 + TM_RTC_CHAR2NUM(*(str + i));
		i++;
	}
	i++;
	
	/* Get year */
	tmp.year = 0;
	while (TM_RTC_CHARISNUM(*(str + i))) {
		tmp.year = tmp.year * 10 + TM_RTC_CHAR2NUM(*(str + i));
		i++;
	}
	i++;
	
	/* Get day in a week */
	tmp.day = 0;
	while (TM_RTC_CHARISNUM(*(str + i))) {
		tmp.day = tmp.day * 10 + TM_RTC_CHAR2NUM(*(str + i));
		i++;
	}
	i++;
	
	/* Get hours */
	tmp.hours = 0;
	while (TM_RTC_CHARISNUM(*(str + i))) {
		tmp.hours = tmp.hours * 10 + TM_RTC_CHAR2NUM(*(str + i));
		i++;
	}
	i++;
	
	/* Get minutes */
	tmp.minutes = 0;
	while (TM_RTC_CHARISNUM(*(str + i))) {
		tmp.minutes = tmp.minutes * 10 + TM_RTC_CHAR2NUM(*(str + i));
		i++;
	}
	i++;
	
	/* Get seconds */
	tmp.seconds = 0;
	while (TM_RTC_CHARISNUM(*(str + i))) {
		tmp.seconds = tmp.seconds * 10 + TM_RTC_CHAR2NUM(*(str + i));
		i++;
	}
	i++;
	
	/* Return status from set date time function */
	return TM_RTC_SetDateTime(&tmp, TM_RTC_Format_BIN);
}

void TM_RTC_GetDateTime(TM_RTC_t* data, TM_RTC_Format_t format) {
	uint32_t unix;

	/* Get time */
	if (format == TM_RTC_Format_BIN) {
		RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
	} else {
		RTC_GetTime(RTC_Format_BCD, &RTC_TimeStruct);
	}
	
	/* Format hours */
	data->hours = RTC_TimeStruct.RTC_Hours;
	data->minutes = RTC_TimeStruct.RTC_Minutes;
	data->seconds = RTC_TimeStruct.RTC_Seconds;
	
	/* Get subseconds */
	data->subseconds = RTC->SSR;
	
	/* Get date */
	if (format == TM_RTC_Format_BIN) {
		RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
	} else {
		RTC_GetDate(RTC_Format_BCD, &RTC_DateStruct);
	}
	
	/* Format date */
	data->year = RTC_DateStruct.RTC_Year;
	data->month = RTC_DateStruct.RTC_Month;
	data->date = RTC_DateStruct.RTC_Date;
	data->day = RTC_DateStruct.RTC_WeekDay;
	
	/* Calculate unix offset */
	unix = TM_RTC_GetUnixTimeStamp(data);
	data->unix = unix;
}

uint8_t TM_RTC_GetDaysInMonth(uint8_t month, uint8_t year) {
	/* Check input data */
	if (
		month == 0 ||
		month > 12
	) {
		/* Error */
		return 0;
	}
	
	/* Return days in month */
	return TM_RTC_Months[TM_RTC_LEAP_YEAR(2000 + year) ? 1 : 0][month - 1];
}

uint16_t TM_RTC_GetDaysInYear(uint8_t year) {
	/* Return days in year */
	return TM_RTC_DAYS_IN_YEAR(2000 + year);
}

void TM_RTC_Config(TM_RTC_ClockSource_t source) {
        switch (source)
        {
        case TM_RTC_ClockSource_Internal:
          /* Enable the LSI OSC */
          RCC_LSICmd(ENABLE);

          /* Wait till LSI is ready */
          while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

          /* Select the RTC Clock Source */
          RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
          break;
        case TM_RTC_ClockSource_External:
          /* Enable the LSE OSC */
          RCC_LSEConfig(RCC_LSE_ON);

          /* Wait till LSE is ready */ 
          while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

          /* Select the RTC Clock Source */
          RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
          break;
        case TM_RTC_ClockSource_HSE:
          RCC_RTCCLKConfig(source | RCC_RTCCLKSource_HSE_Div8);
          break;
  }
	
	/* Enable the RTC Clock */
	RCC_RTCCLKCmd(ENABLE);
	
	/* Wait for register synchronization */
	RTC_WaitForSynchro();

	/* Write status */
	RTC_WriteBackupRegister(RTC_STATUS_REG, RTC_STATUS_INIT_OK);
}

void TM_RTC_Interrupts(TM_RTC_Int_t int_value) {
	uint32_t int_val;
	
	/* Clear pending bit */
	EXTI->PR = 0x00400000;
	
	/* Disable wakeup interrupt */
	RTC_WakeUpCmd(DISABLE);
	
	/* Disable RTC interrupt flag */
	RTC_ITConfig(RTC_IT_WUT, DISABLE);
	
	/* NVIC init for RTC */
	NVIC_InitStruct.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = RTC_PRIORITY;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = RTC_WAKEUP_SUBPRIORITY;
	NVIC_InitStruct.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStruct); 
	
	/* RTC connected to EXTI_Line22 */
	EXTI_InitStruct.EXTI_Line = EXTI_Line22;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStruct.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStruct);
	
	if (int_value != TM_RTC_Int_Disable) {
		/* Enable NVIC */
		NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStruct); 
		/* Enable EXT1 interrupt */
		EXTI_InitStruct.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStruct);

		/* First disable wake up command */
		RTC_WakeUpCmd(DISABLE);

		if (int_value == TM_RTC_Int_60s) {
			int_val = 0x3BFFF; 		/* 60 seconds = 60 * 4096 / 1 = 245760 */
		} else if (int_value == TM_RTC_Int_30s) {
			int_val = 0x1DFFF;		/* 30 seconds */
		} else if (int_value == TM_RTC_Int_15s) {
			int_val = 0xEFFF;		/* 15 seconds */
		} else if (int_value == TM_RTC_Int_10s) {
			int_val = 0x9FFF;		/* 10 seconds */
		} else if (int_value == TM_RTC_Int_5s) {
			int_val = 0x4FFF;		/* 5 seconds */
		} else if (int_value == TM_RTC_Int_2s) {
			int_val = 0x1FFF;		/* 2 seconds */
		} else if (int_value == TM_RTC_Int_1s) {
			int_val = 0x0FFF;		/* 1 second */
		} else if (int_value == TM_RTC_Int_500ms) {
			int_val = 0x7FF;		/* 500 ms */
		} else if (int_value == TM_RTC_Int_250ms) {
			int_val = 0x3FF;		/* 250 ms */
		} else if (int_value == TM_RTC_Int_125ms) {
			int_val = 0x1FF;		/* 125 ms */
		}		

		/* Clock divided by 8, 32768 / 8 = 4096 */
		/* 4096 ticks for 1second interrupt */
		RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div8);
		
		/* Set RTC wakeup counter */
		RTC_SetWakeUpCounter(int_val);
		/* Enable wakeup interrupt */
		RTC_ITConfig(RTC_IT_WUT, ENABLE);
		/* Enable wakeup command */
		RTC_WakeUpCmd(ENABLE);
	}
}

uint32_t TM_RTC_GetUnixTimeStamp(TM_RTC_t* data) {
	uint32_t days = 0, seconds = 0;
	uint16_t i;
	uint16_t year = (uint16_t) (data->year + 2000);
	/* Year is below offset year */
	if (year < TM_RTC_OFFSET_YEAR) {
		return 0;
	}
	/* Days in back years */
	for (i = TM_RTC_OFFSET_YEAR; i < year; i++) {
		days += TM_RTC_DAYS_IN_YEAR(i);
	}
	/* Days in current year */
	for (i = 1; i < data->month; i++) {
		days += TM_RTC_Months[TM_RTC_LEAP_YEAR(year)][i - 1];
	}
	/* Day starts with 1 */
	days += data->date - 1;
	seconds = days * TM_RTC_SECONDS_PER_DAY;
	seconds += data->hours * TM_RTC_SECONDS_PER_HOUR;
	seconds += data->minutes * TM_RTC_SECONDS_PER_MINUTE;
	seconds += data->seconds;
	
	/* seconds = days * 86400; */
	return seconds;
}

void TM_RTC_GetDateTimeFromUnix(TM_RTC_t* data, uint32_t unix) {
	uint16_t year;
	
	/* Store unix time to unix in struct */
	data->unix = unix;
	/* Get seconds from unix */
	data->seconds = unix % 60;
	/* Go to minutes */
	unix /= 60;
	/* Get minutes */
	data->minutes = unix % 60;
	/* Go to hours */
	unix /= 60;
	/* Get hours */
	data->hours = unix % 24;
	/* Go to days */
	unix /= 24;
	
	/* Get week day */
	/* Monday is day one */
	data->day = (unix + 3) % 7 + 1;

	/* Get year */
	year = 1970;
	while (1) {
		if (TM_RTC_LEAP_YEAR(year)) {
			if (unix >= 366) {
				unix -= 366;
			} else {
				break;
			}
		} else if (unix >= 365) {
			unix -= 365;
		} else {
			break;
		}
		year++;
	}
	/* Get year in xx format */
	data->year = (uint8_t) (year - 2000);
	/* Get month */
	for (data->month = 0; data->month < 12; data->month++) {
		if (TM_RTC_LEAP_YEAR(year) && unix >= (uint32_t)TM_RTC_Months[1][data->month]) {
			unix -= TM_RTC_Months[1][data->month];
		} else if (unix >= (uint32_t)TM_RTC_Months[0][data->month]) {
			unix -= TM_RTC_Months[0][data->month];
		} else {
			break;
		}
	}
	/* Get month */
	/* Month starts with 1 */
	data->month++;
	/* Get date */
	/* Date starts with 1 */
	data->date = unix + 1;
}

void TM_RTC_SetAlarm(TM_RTC_Alarm_t Alarm, TM_RTC_AlarmTime_t* DataTime, TM_RTC_Format_t format) {
	RTC_AlarmTypeDef RTC_AlarmStruct;
	
	/* Disable alarm first */
	TM_RTC_DisableAlarm(Alarm);
	
	/* Set RTC alarm settings */
	/* Set alarm time */
	RTC_AlarmStruct.RTC_AlarmTime.RTC_Hours = DataTime->hours;
	RTC_AlarmStruct.RTC_AlarmTime.RTC_Minutes = DataTime->minutes;
	RTC_AlarmStruct.RTC_AlarmTime.RTC_Seconds = DataTime->seconds;
	RTC_AlarmStruct.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
	
	/* Alarm type is every week the same day in a week */
	if (DataTime->alarmtype == TM_RTC_AlarmType_DayInWeek) {
		/* Alarm trigger every week the same day in a week */
		RTC_AlarmStruct.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay;
		
		/* Week day can be between 1 and 7 */
		if (DataTime->day == 0) {
			RTC_AlarmStruct.RTC_AlarmDateWeekDay = 1;
		} else if (DataTime->day > 7) {
			RTC_AlarmStruct.RTC_AlarmDateWeekDay = 7;
		} else {
			RTC_AlarmStruct.RTC_AlarmDateWeekDay = DataTime->day;
		}
	} else { /* Alarm type is every month the same day */
		/* Alarm trigger every month the same day in a month */
		RTC_AlarmStruct.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
	
		/* Month day can be between 1 and 31 */
		if (DataTime->day == 0) {
			RTC_AlarmStruct.RTC_AlarmDateWeekDay = 1;
		} else if (DataTime->day > 31) {
			RTC_AlarmStruct.RTC_AlarmDateWeekDay = 31;
		} else {
			RTC_AlarmStruct.RTC_AlarmDateWeekDay = DataTime->day;
		}
	}

	switch (Alarm) {
		case TM_RTC_Alarm_A:		
			/* Configure the RTC Alarm A */
			RTC_SetAlarm(format, RTC_Alarm_A, &RTC_AlarmStruct);
		
			/* Enable Alarm A */
			RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
			
			/* Enable Alarm A interrupt */
			RTC_ITConfig(RTC_IT_ALRA, ENABLE);
		
			/* Clear Alarm A pending bit */
			RTC_ClearFlag(RTC_IT_ALRA);
			break;
		case TM_RTC_Alarm_B:
			/* Configure the RTC Alarm B */
			RTC_SetAlarm(format, RTC_Alarm_B, &RTC_AlarmStruct);
		
			/* Enable Alarm B */
			RTC_AlarmCmd(RTC_Alarm_B, ENABLE);
		
			/* Enable Alarm B interrupt */
			RTC_ITConfig(RTC_IT_ALRB, ENABLE);
		
			/* Clear Alarm B pending bit */
			RTC_ClearFlag(RTC_IT_ALRB);
			break;
		default:
			break;
	}
}

void TM_RTC_DisableAlarm(TM_RTC_Alarm_t Alarm) {
	switch (Alarm) {
		case TM_RTC_Alarm_A:
			/* Disable Alarm A */
			RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
			
			/* Disable Alarm A interrupt */
			RTC_ITConfig(RTC_IT_ALRA, DISABLE);
		
			/* Clear Alarm A pending bit */
			RTC_ClearFlag(RTC_IT_ALRA);
			break;
		case TM_RTC_Alarm_B:
			/* Disable Alarm B */
			RTC_AlarmCmd(RTC_Alarm_B, DISABLE);
		
			/* Disable Alarm B interrupt */
			RTC_ITConfig(RTC_IT_ALRB, DISABLE);
		
			/* Clear Alarm B pending bit */
			RTC_ClearFlag(RTC_IT_ALRB);
			break;
		default:
			break;
	}
	
	/* Clear RTC Alarm pending bit */
	EXTI->PR = 0x00020000;
	
	/* Configure EXTI 17 as interrupt */
	EXTI_InitStruct.EXTI_Line = EXTI_Line17;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	
	/* Initialite Alarm EXTI interrupt */
	EXTI_Init(&EXTI_InitStruct);

	/* Configure the RTC Alarm Interrupt */
	NVIC_InitStruct.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = RTC_PRIORITY;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = RTC_ALARM_SUBPRIORITY;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	
	/* Initialize RTC Alarm Interrupt */
	NVIC_Init(&NVIC_InitStruct);
}

void TM_RTC_WriteBackupRegister(uint8_t location, uint32_t value) {
	/* Check input, 0 to 18 registers are allowed */
	if (location > 18) {
		return;
	}
	
	/* Write data to backup register */
	*(uint32_t *)((&RTC->BKP0R) + 4 * location) = value;
}

uint32_t TM_RTC_ReadBackupRegister(uint8_t location){
	/* Check input, 0 to 18 registers are allowed */
	if (location > 18) {
		return 0;
	}
	
	/* Read data from backup register */
	return *(uint32_t *)((&RTC->BKP0R) + 4 * location);
}

/* Callbacks */
__weak void TM_RTC_RequestHandler(void) {
	/* If user needs this function, then they should be defined separatelly in your project */
}

__weak void TM_RTC_AlarmAHandler(void) {
	/* If user needs this function, then they should be defined separatelly in your project */
}

__weak void TM_RTC_AlarmBHandler(void) {
	/* If user needs this function, then they should be defined separatelly in your project */
}

/* Private RTC IRQ handlers */
void RTC_WKUP_IRQHandler(void) {
	/* Check for RTC interrupt */
	if (RTC_GetITStatus(RTC_IT_WUT) != RESET) {
		/* Clear interrupt flags */
		RTC_ClearITPendingBit(RTC_IT_WUT);
		
		/* Call user function */
		TM_RTC_RequestHandler();
	}
	
	/* Clear EXTI line 22 bit */
	EXTI->PR = 0x00400000;
}

void RTC_Alarm_IRQHandler(void) {
	/* RTC Alarm A check */
	if (RTC_GetITStatus(RTC_IT_ALRA) != RESET) {
		/* Clear RTC Alarm A interrupt flag */
		RTC_ClearITPendingBit(RTC_IT_ALRA);
		
		/* Call user function for Alarm A */
		TM_RTC_AlarmAHandler();
	}
	
	/* RTC Alarm B check */
	if (RTC_GetITStatus(RTC_IT_ALRB) != RESET) {
		/* Clear RTC Alarm A interrupt flag */
		RTC_ClearITPendingBit(RTC_IT_ALRB);
		
		/* Call user function for Alarm B */
		TM_RTC_AlarmBHandler();
	}
	
	/* Clear EXTI line 17 bit */
	EXTI->PR = 0x00020000;
}

/**
  * @brief  Установливает время срабатывания профилактики сульфатации для отладки
  * @retval 
  */
void RTC_SetDebugProfTime(void)
{
  TM_RTC_t currentTime;
  uint32_t unixTime;
  
  TM_RTC_GetDateTime(&currentTime, TM_RTC_Format_BIN);
  
  unixTime = TM_RTC_GetUnixTimeStamp(&currentTime);
    
  unixTime += 30;
  
  TM_RTC_WriteBackupRegister(RTC_BKP_DR0, unixTime);
}

/**
  * @brief  Установливает время срабатывания профилактики сульфатации
  */
void RTC_SetProfTime(char *str)
{
  TM_RTC_t tmp;
  uint32_t unixTime;
  uint8_t i = 0;
	
  /* Get date */
  tmp.date = 0;
  while (TM_RTC_CHARISNUM(*(str + i))) {
	tmp.date = tmp.date * 10 + TM_RTC_CHAR2NUM(*(str + i));
	i++;
  }
  i++;
	
  /* Get month */
  tmp.month = 0;
  while (TM_RTC_CHARISNUM(*(str + i))) {
	tmp.month = tmp.month * 10 + TM_RTC_CHAR2NUM(*(str + i));
	i++;
  }
  //i++;
  i+=3;
  
  /* Get year */
  tmp.year = 0;
  while (TM_RTC_CHARISNUM(*(str + i))) {
	tmp.year = tmp.year * 10 + TM_RTC_CHAR2NUM(*(str + i));
	i++;
  }
  i++;
  i++;
	
  /* Get hours */
  tmp.hours = 0;
  while (TM_RTC_CHARISNUM(*(str + i))) {
	tmp.hours = tmp.hours * 10 + TM_RTC_CHAR2NUM(*(str + i));
	i++;
  }
  i++;
	
  /* Get minutes */
  tmp.minutes = 0;
  while (TM_RTC_CHARISNUM(*(str + i))) {
	tmp.minutes = tmp.minutes * 10 + TM_RTC_CHAR2NUM(*(str + i));
	i++;
  }
  i++;
	
  /* Get seconds */
  tmp.seconds = 0;
	
  /* Устанавливаем +1 для профилактики */
  tmp.year += 1;
  unixTime = TM_RTC_GetUnixTimeStamp(&tmp);  
  TM_RTC_WriteBackupRegister(RTC_BKP_DR0, unixTime);
}

/**
  * @brief  
  * @retval 
  */
uint32_t RTC_GetUnixTime(void)
{
  TM_RTC_t currentTime;
  
  TM_RTC_GetDateTime(&currentTime, TM_RTC_Format_BIN);
  return TM_RTC_GetUnixTimeStamp(&currentTime);
}

//
void RTC_GetSecAndSubsec(uint32_t* sec, uint16_t* subsec)
{
    TM_RTC_t currentTime;
  
    TM_RTC_GetDateTime(&currentTime, TM_RTC_Format_BIN);
    *sec = TM_RTC_GetUnixTimeStamp(&currentTime);
    *subsec = uint16_t(1000 - (float)currentTime.subseconds * 1000.0/1023.0);    
}

void vRTC(void *params)
{
  extern OPU_TIME_t utc;
  TM_RTC_t currentTime;
  for (;;)
  {
    TM_RTC_GetDateTime(&currentTime, TM_RTC_Format_BIN);
    utc.year = 2000 + currentTime.year;
    utc.month = currentTime.month;
    utc.day = currentTime.day;
    utc.hour = currentTime.hours;
    utc.min = currentTime.minutes;
    utc.sec = currentTime.seconds;
    vTaskDelay(200);
  }
}

/*
void vTM_RTC_SetDateTime(void *params)
{
  
  vTaskDelete(NULL);
}
*/
