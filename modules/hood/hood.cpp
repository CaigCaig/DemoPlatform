#include "stm32f4xx.h"
#include "hood.h"
#include "rs485.h"
#include "FreeRTOS.h"
#include "task.h"
#include "mb_def.h"
#include "com_handler.h"

extern uint32_t com_sr_h1;            // статусный регистр принятия команд для Hood1
extern uint32_t com_sr_h2;            // статусный регистр принятия команд для Hood2
