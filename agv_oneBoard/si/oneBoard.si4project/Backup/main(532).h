//*********************************************************************************************
//
// Proprietary and Confidential Information
//
// Copyright (c) 2022 by Canon Korea. All Rights Reserved.
//
// This source code and the algorithms implemented therein constitute
// confidential information and may compromise trade secretes of Canon Korea.
// or its associates, and any unaythorized use thereof is prohibited.
//
//*******************************************************************************************//

//*******************************************************************************************//
//
// File Name:main.h
//
// Date: 2022-06-24
//
// Auther : Shin jin-ho
//
// Description: FA-DUINO-32TA Board Base AGV Development 
//
//*******************************************************************************************//

/*********************************************************************************************
* 1. INCLUDES
*********************************************************************************************/

/*********************************************************************************************
* -. Internal Definition
*********************************************************************************************/

/*********************************************************************************************
* 2. GLOBAL VALUE
*********************************************************************************************/
#define FALSE 0
#define TRUE  1

// ckr_jhshin_220701[BoadRate Setting]
#define BOADRATE_9600 9600
#define BOADRATE_19200	19200
#define BOADRATE_38400	38400
#define BOADRATE_57600	57600
#define BOADRATE_115200	115200

// Defile value chage need
#define LCD_WAIT	100
#define LCD_INIT	110
#define LCD_READY	120
#define LCD_MOVE	130
#define LCD_STOP	140
#define LCD_ERROR	150
#define LCD_TEST	160

#define E100		200	 // ckr_jhshin_220704[PIN Error]
#define E200		201  // ckr_jhshin_220704[RFID Received Module Error]
#define E300		202	 // ckr_jhshin_220704[Bottom Sensor Error]
#define E400		203	 // ckr_jhshin_220704[XBEE Module Error]
#define E500		204	 // ckr_jhshin_220704[Motor Error]
#define E600		205	 // ckr_jhshin_220704[Emergency Stop]

#define EEPROM_ADDR_RFIDTOP		0
#define EEPROM_ADDR_LEFTY		1
#define EEPROM_ADDR_RIGHTY		2
#define EEPROM_ADDR_SPEED		3

#define R_NG		0
#define R_OK		1
#define R_NONE	99

#define DIR_LEFTY			0
#define DIR_RIGHTY			1
#define DIR_SPEED			2


#define IDLE_FUNC_RCV_TURN_LEFT			1
#define IDLE_FUNC_RCV_TURN_RIGHT		2

#define PIN_UP				1
#define PIN_DOWN			2	

#define LOW_SPEED			0
#define HIGH_SPEED			1

#define LEFT_MOTOR_PIN		11
#define RIGHT_MOTOR_PIN		12
#define MOTOR_TABLE_LENGTH	8
#define MOTOR_TABLE_TYPE	3

#define MOT_TBL_SIZE		14
#define MOT_TBL_CEN_IDX		5
#define MOT_TBL_LEFT_IDX	7
#define MOT_TBL_RIGHT_IDX	8
#define MOT_DEFAULT_SPEED	100
#define MOT_DEFAULT_HIGH_SPEED	150

// LCD SETTING VALUE
#define RFID_TAG_L_TURN		101
#define RFID_TAG_R_TURN		102
#define RFID_TAG_STOP		103
#define RFID_TAG_PINUP_GO	104
#define RFID_TAG_PINDN_GO	105
#define RFID_TAG_SPEEDUP	106
#define RFID_TAG_SPEEDDN	107
#define RFID_TAG_LEFTY		111
#define RFID_TAG_RIGHTY		113
#define RFID_TAG_START		201
#define RFID_AGV_TOP_INIT_NUMBER	1

// SENSOR, BTN PIN SETTING
#define PHOTOSEN_TOP		34
#define PHOTOSEN_BOTTOM		35
#define PIN_MOTOR_ENABLE	42
#define PIN_MOTOR_DIR		10	// LOW - PIN DOWN, HIGHT - PIN UP 
#define PIN_MOTOR_PWM		11
#define PIN_BUTTON			37
#define START_BUTTON		36
#define STOP_BUTTON			2
#define FRONT_SENSOR		3 //PDY_2022.08.11



// LINE SENSOR (BOTTOM SENSOR)
#define LINE_SENSOR1		22
#define LINE_SENSOR2		23
#define LINE_SENSOR3		24
#define LINE_SENSOR4		25
#define LINE_SENSOR5		26
#define LINE_SENSOR6		27
#define LINE_SENSOR7		28
#define LINE_SENSOR8		29
#define LINE_SENSOR9		30
#define LINE_SENSOR10		31
#define LINE_SENSOR11		32
#define LINE_SENSOR12		33

// MOTOR PIN SETTING
#define LEFT_MOTOR			6
#define LMOTOR_SET_DRV 		38
#define LMOTOR_SET_BREAK	39
#define LMOTOR_SET_DIR		7

#define RIGHT_MOTOR			8
#define RMOTOR_SET_DRV		40
#define RMOTOR_SET_BREAK	41
#define RMOTOR_SET_DIR		9

#define PIN_HIGH_HIGH		11
#define PIN_HIGH_LOW		10
#define PIN_LOW_HIGH		01
#define PIN_LOW_LOW			0

#define E_STOP_DEFAULT		200
#define E_STOP_CHANGED		300


#define DEBUG_MODE		1	// ># 
//#define PARKDY			1
#define DEBUG_PIN			1	// #P: 
//#define DEBUG_LCD_DP		1	// #L: 
//#define FRONT_SEN			1	// #S: 
//#define DEBUG_MOT			1	// #M: 
#define DIR_ORG				1
volatile unsigned char RFID_RXBUFF;  // RFID Receive Buffer

/*********************************************************************************************
* 3. INLINE FUNCTION
*********************************************************************************************/
#define _RD_START_BTN (digitalRead(START_BUTTON))
#define _RD_PHOTO_TOP (digitalRead(PHOTOSEN_TOP))
#define _RD_PHOTO_BOT (digitalRead(PHOTOSEN_BOTTOM))
#define _EEPROM_RD_TOP_RFID (EEPROM.read(EEPROM_ADDR_RFIDTOP))
#define _EEPROM_RD_LEFTY (EEPROM.read(EEPROM_ADDR_LEFTY))
#define _EEPROM_RD_RIGHTY (EEPROM.read(EEPROM_ADDR_RIGHTY))
#define _EEPROM_RD_MOT_SPEED (EEPROM.read(EEPROM_ADDR_SPEED))

#define _READ_LINE_SEN1 (digitalRead(LINE_SENSOR1))	// READ TAPE : 0, NO TAPE : 1
#define _READ_LINE_SEN2 (digitalRead(LINE_SENSOR2))
#define _READ_LINE_SEN3 (digitalRead(LINE_SENSOR3))
#define _READ_LINE_SEN4 (digitalRead(LINE_SENSOR4))
#define _READ_LINE_SEN5 (digitalRead(LINE_SENSOR5))
#define _READ_LINE_SEN6 (digitalRead(LINE_SENSOR6))
#define _READ_LINE_SEN7 (digitalRead(LINE_SENSOR7))
#define _READ_LINE_SEN8 (digitalRead(LINE_SENSOR8))
#define _READ_LINE_SEN9 (digitalRead(LINE_SENSOR9))
#define _READ_LINE_SEN10 (digitalRead(LINE_SENSOR10))
#define _READ_LINE_SEN11 (digitalRead(LINE_SENSOR11))
#define _READ_LINE_SEN12 (digitalRead(LINE_SENSOR12))
#define _READ_BATTERY (analogRead(A4))

#define _QUEUE_IN(x) x % 20	// queue max size : 20
#define _RFBUF_CNT(x)	(x > 7) ? x = 0 : x 

/*********************************************************************************************
* 4. STRUCT
*********************************************************************************************/
// status info
typedef enum{
    AGV_OFF_STATE=0,
    AGV_INIT_STATE,
    AGV_IDLE_STATE,
    AGV_ERROR_STATE,
    AGV_STATUS_LAST
} AGV_STATUS;

typedef enum{
    AGV_REQ_NONE=0,
    AGV_REQ_MOVE,
    AGV_REQ_LAST
} AGV_REQ;

typedef enum{
	LCD_ADDR_DRV_STS=0,
	LCD_ADDR_PIN_STS,
	LCD_ADDR_RFID_NUM,
	LCD_ADDR_AGV_TOP,
	LCD_ADDR_LEFTY,
	LCD_ADDR_RIGHTY,
	LCD_ADDR_SPEED_ADJ,
	LCD_ADDR_TRIGGER,
	LCD_ADDR_BAT,
	LCD_ADDR_E_STOP,	
	LCD_ADDR_LAST
} LCD_DISP_ADDR;

/*
typedef enum{
   int rfid_tag;
   
}RFID_TAG;
*/

typedef struct AGV_STSTUS_STRUCT {
    AGV_STATUS currStatus;
}STATUS_INFO;

typedef struct AGV_REQUEST_STRUCT{
    AGV_REQ currReq;
}REQUEST_INFO;

typedef struct AGV_DRIVING_INFORMATION {
	//bool mot_table_change_flag;
	bool drv_direction_info;	// lefty or righty - default: 0 : lefty
	bool move_start_by_rfid_tag;// RFID_TAG_START
	bool speed_mode;			// high , normal speed - default : 0 : normal speed
	int turn_direction;			// when RFID_TAG is turn in IDLE function ( 0 : not set, 1: left turn, 2: right turn)
	int pre_speed_left;
	int pre_speed_right;
}DRIVING_INFO;
// AGV Struct Info
/*
typedef struct AGV_MOTOR_STRUCT {
  int LEFT_MOTOR;
  int LMOTOR_SET_DRV;
  int LMOTOR_SET_BREAK;
  int LMOTOR_SET_DIR;
  
  int RIGHT_MOTOR;
  int RMOTOR_SET_DRV;
  int RMOTOR_SET_BREAK;
  int RMOTOR_SET_DIR;
}MOTOR_DES;
*/
typedef struct {
    char MY_AGV_NUM;    // my AGV Number
    int BOTTOM_LINE_SEN;    // Bottom Sensor Data in
    //bool direction_info;
    unsigned char rf_buf[7];	// ckr_jhshin_220704[RFID Receive Buffer]
    int top_rfid_number;		 // ckr_jhshin_220704[TOP_RFID Value- AGV Number]
    int lefty_tag_number;		 // ckr_jhshin_220704[lefty tag number]
    int righty_tag_number;		 // ckr_jhshin_220704[righty_tag_number]
    int mot_speed_adj;			 // ckr_jhshin_220704[motor speed adjust]
    STATUS_INFO status;
    REQUEST_INFO request;
    int rfid_tag;
#if 0 //def PARKDY
	int front_sensor_sts;		// Front Sensor: Default (00), Left ON: (10), Right ON (01)
#endif
	int photo_sen_status;		// chr_jhshin_220711[photo sensor status checker]
	bool start_btn_flag;			// ckr_jhshin_220712[motor go or stop flag]
	bool pin_btn_pressed;		// ckr_jhshin_220712[pin btn flag]
	DRIVING_INFO drv_info;		// ckr_jhshin_220712[Driving information from RFID TAG]
} AGV_CB;



typedef enum {
    RFID_STOP=0,
    RFID_START,
    RFID_LEFTSIDE,
    RFID_RIGHTSIDE,
    RFID_TURN_LEFT,
    RFID_TURN_RIGHT,
    RFID_PINUP,
    RFID_PINDOWN
}RFID_NUMBER;

typedef struct {
	  //uint16_t rfid_buf[7];    // RFID Buffer
	  //int send_data;				// ckr_jhshin_220704[send_data]
} RFID_DATA;

typedef struct {
	uint16_t uiTranslateData[16]; // UI transmit Data
} LCD_DATA;

typedef struct {
  uint8_t ptTranslateData[10];  // Photo Sensor transmit Data
}PHOTO_SEN;


#ifdef NEVER
int Mot_Descript_Lefty[MOTOR_TABLE_TYPE][MOTOR_TABLE_LENGTH] = 
{
	{ 0x03C0, 0x0780, 0x0F00, 0x01E0, 0x01F0, 0, 	  0, 	  0 },	 	 // ckr_jhshin_220704[STRAIGHT : 5EA]
	{ 0x1E00, 0x3C00, 0x7800, 0xF000, 0xE000, 0xC000, 0x8000, 0 },	 	 // ckr_jhshin_220704[LEFTY : 7EA]
	{ 0x00FC, 0x007E, 0x003F, 0x001F, 0x000F, 0x0007, 0x0003, 0x0001 }	 // ckr_jhshin_220704[RIGHTY : 8EA]
};

int Mot_Descript_Righty[MOTOR_TABLE_TYPE][MOTOR_TABLE_LENGTH] = 
{
	{ 0x03C0, 0x0780, 0x0F00, 0x01E0, 0x01F0, 0, 	  0, 	  0 },	 	 // ckr_jhshin_220704[STRAIGHT : 5EA]
	{ 0x1E00, 0x3C00, 0x7800, 0xF000, 0xE000, 0xC000, 0x8000, 0 },	 	 // ckr_jhshin_220704[LEFTY : 7EA]
	{ 0x00FC, 0x007E, 0x003F, 0x001F, 0x000F, 0x0007, 0x0003, 0x0001 }	 // ckr_jhshin_220704[RIGHTY : 8EA]
};
#endif /* NEVER */

int Mot_Descript_Lefty[3][21] = 
{
	{90,90,90,60,60,60,50,50,50,50,50,50,50,50,50,50,50,50,40,40,40},	// left side accel
	{40,40,40,50,50,50,50,50,50,50,50,60,60,60,60,60,60,60,90,90,90},	// right side accel
	{0x0000,0x0001,0x0003,0x0007,0x000F,0x001E,0x003C,0x0078,0x00F0,0x01E0,0x03C0,0x0780,0x0F00,0x1E00,0x3C00,0x7800,0xF000,0xE000,0xC000,0x8000,0x0000}
};

int Mot_Descript_Righty[3][21] = 
{
	{90,90,90,60,60,60,60,60,60,60,50,50,50,50,50,50,50,50,50,50,50},	// left side accel
	{40,40,40,50,50,50,50,50,50,50,50,50,50,50,50,60,60,60,60,60,60},	// right side accel
	{0x0000,0x0001,0x0003,0x0007,0x000F,0x001E,0x003C,0x0078,0x00F0,0x01E0,0x03C0,0x0780,0x0F00,0x1E00,0x3C00,0x7800,0xF000,0xE000,0xC000,0x8000,0x0000}
};

int HIGH_Mot_Descript_Lefty[3][21] = 
{
	{90,90,90,60,60,60,50,50,50,50,50,50,50,50,50,50,50,50,40,40,40},	// left side accel
	{40,40,40,50,50,50,50,50,50,50,50,60,60,60,60,60,60,60,90,90,90},	// right side accel
	{0x0000,0x0001,0x0003,0x0007,0x000F,0x001E,0x003C,0x0078,0x00F0,0x01E0,0x03C0,0x0780,0x0F00,0x1E00,0x3C00,0x7800,0xF000,0xE000,0xC000,0x8000,0x0000}	
};

int HIGH_Mot_Descript_Righty[3][21] = 
{
	{90,90,90,60,60,60,60,60,60,60,50,50,50,50,50,50,50,50,50,50,50},	// left side accel
	{40,40,40,50,50,50,50,50,50,50,50,50,50,50,50,60,60,60,60,60,60},	// right side accel
	{0x0000,0x0001,0x0003,0x0007,0x000F,0x001E,0x003C,0x0078,0x00F0,0x01E0,0x03C0,0x0780,0x0F00,0x1E00,0x3C00,0x7800,0xF000,0xE000,0xC000,0x8000,0x0000}
};

int Mot_Descript_Turn[2][2] = 
{
	{0, 100},	// left side accel
	{100, 0}	// right side accel
};
	
int MOT_LEFTY[3][MOT_TBL_SIZE] = 
{

	{40,	40,		45,		50,		50,		50,		50,		50,		50,		45,		60,		60,		60, 	60},	// Left side
	{60,	60,		60,		50,		50,		50,		50,		50,		50,		50,		45,		40,		40, 	40},	// Right side
//	{50,50,50,50,50,50,50,50,50,50,50,50,50, 50},
//	{50,50,50,50,50,50,50,50,50,50,50,50,50, 50},
	{0x800,	0xC00,	0x600,	0x300,	0x180,	0xC0,	0x60,	0x30,	0x18,	0xC,	0x6,	0x3,	0x1, 	0x1}	// Bottom sensor
};

int MOT_RIGHTY[3][MOT_TBL_SIZE] = 
{
	{60,60,60,60,60,50,50,50,50,50,50,50,50,50},
	{50,50,50,50,50,50,50,50,50,50,60,60,60,60},
	{0xFFE, 0xFFC, 0xFF9, 0xFF3, 0xFE7, 0xFCF, 0xF9F, 0xF3F, 0xE7F, 0xCFF, 0x9FF, 0x3FF, 0x7FF,0x7FF}	
};

int LEFT_SIDE_SEARCH_SENON[3][MOT_TBL_SIZE] = 
{
	{50,	50,		40,		50,		50,		50,		50,		50,		50,		50,		50,		60,		60,		80},	// left side
	{50,	50,		80,		60,		60,		50,		50,		50,		50,		50,		50,		50,		50,		40},	// right side
	{0xFFF, 0x0, 	0x800, 	0x400, 	0x200, 	0x100, 	0x80, 	0x40, 	0x20, 	0x10, 	0x08, 	0x04, 	0x02, 	0x01}
	//{0xFFF, 0x0, 	0x01, 	0x02, 	0x04, 	0x08, 	0x10, 	0x20, 	0x40, 	0x80, 	0x100, 	0x200, 	0x400, 	0x800}
};

int RIGHTT_SIDE_SEARCH_SENON[3][MOT_TBL_SIZE] = 
{
	{50,	50,		40,		50,		50,		50,		50,		50,		50,		50,		50,		60,		60,		80},	// left side
	{50,	50,		80,		60,		60,		50,		50,		50,		50,		50,		50,		50,		50,		40},	// right side
	{0xFFF, 0x0, 	0x800, 	0x400, 	0x200, 	0x100, 	0x80, 	0x40, 	0x20, 	0x10, 	0x08, 	0x04, 	0x02, 	0x01}
	//{0xFFF, 0x0, 	0x01, 	0x02, 	0x04, 	0x08, 	0x10, 	0x20, 	0x40, 	0x80, 	0x100, 	0x200, 	0x400, 	0x800}
};

