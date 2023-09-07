/* Список команд для устройства DEMO */
//#define DEMO_PING                       ((uint8_t)0x10)    // команда контроля установки соединения

#define SM1_ENB_Pin GPIO_Pin_0
#define SM1_ENB_GPIO_Port GPIOE
#define SM1_DIR_Pin GPIO_Pin_1
#define SM1_DIR_GPIO_Port GPIOE
#define SM1_STEP_Pin GPIO_Pin_3
#define SM1_STEP_GPIO_Port GPIOB
#define SM1_ENABLE GPIO_WriteBit(SM1_ENB_GPIO_Port, SM1_ENB_Pin, Bit_RESET)
#define SM1_DISABLE GPIO_WriteBit(SM1_ENB_GPIO_Port, SM1_ENB_Pin, Bit_SET)
#define SM1_STEP_HI GPIO_WriteBit(SM1_STEP_GPIO_Port, SM1_STEP_Pin, Bit_SET)
#define SM1_STEP_LO GPIO_WriteBit(SM1_STEP_GPIO_Port, SM1_STEP_Pin, Bit_RESET)

#define SM2_ENB_Pin GPIO_Pin_2
#define SM2_ENB_GPIO_Port GPIOE
#define SM2_DIR_Pin GPIO_Pin_4
#define SM2_DIR_GPIO_Port GPIOB
#define SM2_STEP_Pin GPIO_Pin_3
#define SM2_STEP_GPIO_Port GPIOE
#define SM2_ENABLE GPIO_WriteBit(SM2_ENB_GPIO_Port, SM2_ENB_Pin, Bit_RESET)
#define SM2_DISABLE GPIO_WriteBit(SM2_ENB_GPIO_Port, SM2_ENB_Pin, Bit_SET)
#define SM2_STEP_HI GPIO_WriteBit(SM2_STEP_GPIO_Port, SM2_STEP_Pin, Bit_SET)
#define SM2_STEP_LO GPIO_WriteBit(SM2_STEP_GPIO_Port, SM2_STEP_Pin, Bit_RESET)

#define SM3_ENB_Pin GPIO_Pin_4
#define SM3_ENB_GPIO_Port GPIOE
#define SM3_DIR_Pin GPIO_Pin_13
#define SM3_DIR_GPIO_Port GPIOC
#define SM3_STEP_Pin GPIO_Pin_5
#define SM3_STEP_GPIO_Port GPIOE
#define SM3_ENABLE GPIO_WriteBit(SM3_ENB_GPIO_Port, SM3_ENB_Pin, Bit_RESET)
#define SM3_DISABLE GPIO_WriteBit(SM3_ENB_GPIO_Port, SM3_ENB_Pin, Bit_SET)
#define SM3_STEP_HI GPIO_WriteBit(SM3_STEP_GPIO_Port, SM3_STEP_Pin, Bit_SET)
#define SM3_STEP_LO GPIO_WriteBit(SM3_STEP_GPIO_Port, SM3_STEP_Pin, Bit_RESET)

typedef enum
{
  OPTICAL = 1,
  OPU
} Device_t;

/*---------------- структуры для ОПУ --------------------*/ 

typedef enum
{
  OPU_REQUEST = 1,
  OPU_SETLON,
  OPU_SETLAT,
  OPU_TIME,
  OPU_LOAD,
  OPU_SETSTEPPER,
  OPU_DATA,
  OPU_CLEARDATA,
  OPU_REQUAIM,
  OPU_SETAIMA,
  OPU_SETAIME,
  OPU_SETZERA,
  OPU_SETZERE,
  OPU_PING = 0x10
} Commands_OPU_t;

typedef enum
{
  OPU_OK = 0,
  OPU_ERROR
} OPU_ERRORS_t;

typedef struct
{
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t min;
  uint8_t sec;
} OPU_TIME_t;

typedef struct
{
  uint16_t spr;          // число шагов двигателя на один оборот
  uint16_t mode;         // режим работы (1 - шаг, 2 - полшага, 4 - четвертьшага и т.д.)
} OPU_STEPPER_t;

typedef struct
{
  float azimuth;
  float elevation;
} OPU_direction_t;

typedef struct
{
  float latitude;       // широта
  char n_s;             // северная/южная
  float longitude;      // долгота
  char e_w;             // восточная/западная
} OPU_coordinates_t;

typedef struct
{
  OPU_TIME_t cur_time;
  OPU_TIME_t zero_time;
  OPU_coordinates_t device_coordinates;
  OPU_direction_t start_direction;
  OPU_direction_t end_direction;
  OPU_direction_t current_direction;
} OPU_DATA_t;

typedef struct
{
  char str[4];
  Device_t device;
  Commands_OPU_t command;
  OPU_ERRORS_t error;
  OPU_direction_t current_direction;
} OPU_ACK_REQUEST_t;

typedef struct
{
  char str[4];
  Device_t device;
  Commands_OPU_t command;
  OPU_ERRORS_t error;
} OPU_ACK_SETLON_t;

typedef struct
{
  char str[4];
  Device_t device;
  Commands_OPU_t command;
  OPU_ERRORS_t error;
} OPU_ACK_SETLAT_t;

typedef struct
{
  char str[4];
  Device_t device;
  Commands_OPU_t command;
  OPU_ERRORS_t error;
} OPU_ACK_TIME_t;

typedef struct
{
  char str[4];
  Device_t device;
  Commands_OPU_t command;
  OPU_ERRORS_t error;
} OPU_ACK_LOAD_t;

typedef struct
{
  char str[4];
  Device_t device;
  Commands_OPU_t command;
  OPU_ERRORS_t error;
} OPU_ACK_SETSTEPPER_t;

typedef struct
{
  char str[4];
  Device_t device;
  Commands_OPU_t command;
  OPU_ERRORS_t error;
} OPU_ACK_DATA_t;

typedef struct
{
  char str[4];
  Device_t device;
  Commands_OPU_t command;
  OPU_ERRORS_t error;
} OPU_ACK_CLEARDATA_t;

typedef struct
{
  char str[4];
  Device_t device;
  Commands_OPU_t command;
  OPU_ERRORS_t error;
  OPU_direction_t target_direction;
} OPU_ACK_REQUAIM_t;

typedef struct
{
  char str[4];
  Device_t device;
  Commands_OPU_t command;
  OPU_ERRORS_t error;
} OPU_ACK_SETAIMA_t;

typedef struct
{
  char str[4];
  Device_t device;
  Commands_OPU_t command;
  OPU_ERRORS_t error;
} OPU_ACK_SETAIME_t;

typedef struct
{
  char str[4];
  Device_t device;
  Commands_OPU_t command;
  OPU_ERRORS_t error;
} OPU_ACK_SETZERA_t;

typedef struct
{
  char str[4];
  Device_t device;
  Commands_OPU_t command;
  OPU_ERRORS_t error;
} OPU_ACK_SETZERE_t;

typedef struct
{
  char str[4];
  Device_t device;
  Commands_OPU_t command;
  char ack[4];
} OPU_ACK_Ping_t;

/*---------------- структуры для оптической установки --------------------*/ 

typedef enum
{
  COM_OPTICAL_PING = 0x10,
  COM_OPTICAL_SM = 0x13
} Commands_OPTICAL_t;

typedef enum
{
  OPTICAL_OK = 0,
  OPTICAL_ERROR
} OPTICAL_ERRORS_t;

typedef struct
{
  char str[4];
  Device_t device;
  Commands_OPTICAL_t command;
  char ack[4];
} OPTICAL_ACK_Ping_t;

typedef struct
{
  char str[4];
  Device_t device;
  Commands_OPTICAL_t command;
  OPTICAL_ERRORS_t error;
} OPTICAL_ACK_SM_t;

//--------------------------

typedef enum
{
  OPTICAL_STEPS = 1,
  OPTICAL_ANGLE
} OPTICAL_Steps_Type_t;

typedef struct
{
  OPTICAL_Steps_Type_t data_type;
  int32_t steps;
  float angle;
} OPTICAL_SMdata_t;

