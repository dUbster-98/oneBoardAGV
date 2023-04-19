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

#define EEPROM_ADDR_RFIDTOP		0
#define EEPROM_ADDR_LEFTY		1
#define EEPROM_ADDR_RIGHTY		2

#define R_NG		0
#define R_OK		1
#define R_NONE	99

#define DIR_LEFTY			0
#define DIR_RIGHTY			1

#define PIN_UP				1
#define PIN_DOWN			0	

#define LOW_SPEED			0
#define HIGH_SPEED			1

#define LEFT_MOTOR_PIN		11
#define RIGHT_MOTOR_PIN		12
#define MOTOR_TABLE_LENGTH	8
#define MOTOR_TABLE_TYPE	3

#define MOT_TBL_CEN_IDX		5
#define MOT_TBL_LEFT_IDX	7
#define MOT_TBL_RIGHT_IDX	8

// LCD SETTING VALUE
#define RFID_TAG_L_TURN		101
#define RFID_TAG_R_TURN		102
#define RFID_TAG_STOP		103
#define RFID_TAG_PINDN_GO	104
#define RFID_TAG_PINUP_GO	105
#define RFID_TAG_SPEEDUP	106
#define RFID_TAG_SPEEDDN	107
#define RFID_TAG_LEFTY		111
#define RFID_TAG_RIGHTY		113
#define RFID_TAG_START		201
#define RFID_AGV_TOP_INIT_NUMBER	1

// SENSOR, BTN PIN SETTING
#define PHOTOSEN_TOP		22
#define PHOTOSEN_BOTTOM		23
#define PIN_MOTOR_ENABLE	42
#define PIN_MOTOR_DIR		10	// LOW - PIN DOWN, HIGHT - PIN UP 
#define PIN_MOTOR_PWM		11
#define PIN_BUTTON			37
#define START_BUTTON		36
#define STOP_BUTTON			2

// MOTOR PIN SETTING
#define LEFT_MOTOR			7
#define LMOTOR_SET_DRV 		38
#define LMOTOR_SET_BREAK	39
#define LMOTOR_SET_DIR		40

#define RIGHT_MOTOR			6
#define RMOTOR_SET_DRV		41
#define RMOTOR_SET_BREAK	42
#define RMOTOR_SET_DIR		43

#define DEBUG_MODE			1
volatile unsigned char CH2_RXBUFF;  // RFID Receive Buffer

/*********************************************************************************************
* 3. INLINE FUNCTION
*********************************************************************************************/

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
    AGV_REQ_STOP,
    AGV_REQ_PIN,
    AGV_REQ_TEST,
    AGV_REQ_LAST
} AGV_REQ;

typedef enum{
	LCD_ADDR_DRV_STS=0,
	LCD_ADDR_PIN_STS,
	LCD_ADDR_RFID_NUM,
	LCD_ADDR_AGV_TOP,
	LCD_ADDR_LEFTY,
	LCD_ADDR_RIGHTY,
	LCD_ADDR_LAST
} LCD_DISP_ADDR;

/*
typedef enum{
   int rfid_tag;
   
}RFID_TAG;
*/

typedef struct AGV_STSTUS_STRUCT {
    AGV_STATUS currStatus;
	AGV_STATUS preStatus;
	AGV_REQ currReq;
	AGV_REQ preReq;
}STATUS_INFO;

typedef struct AGV_REQUEST_STRUCT{
    AGV_REQ currReq;
}REQUEST_INFO;

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
    bool MOTOR_SPEED;   // normal mode = false, high speed = true;
    int BOTTOM_LINE_SEN;    // Bottom Sensor Data in
    bool start_btn_flag;
    bool direction_info;
    unsigned char rf_buf[7];	// ckr_jhshin_220704[RFID Receive Buffer]
    int top_rfid_number;		 // ckr_jhshin_220704[TOP_RFID Value- AGV Number]
    int lefty_tag_number;		 // ckr_jhshin_220704[lefty tag number]
    int righty_tag_number;		 // ckr_jhshin_220704[righty_tag_number]
    STATUS_INFO status;
    REQUEST_INFO request;
    int rfid_tag;
	int photo_sen_status;		// chr_jhshin_220711[photo sensor status checker]
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
	uint16_t uiTranslateData[7]; // UI transmit Data
} LCD_DATA;

#ifdef NEVER
typedef struct {
	uint16_t uiTranslateData[16]; // UI transmit Data
} GET_LCD_DATA;
#endif /* NEVER */

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
	{0x0000,0x0001,0x0003,0x0007,0x000F,0x001E,0x003C,0x0078,0x00F0,0x01E0,0x03C0,0x0780,0x0F00,0x1E00,0x3C00,0x7800,0xF000,0xE000,0xC000,0x8000,0x0000},
	{90,90,90,60,60,60,50,50,50,50,50,50,50,50,50,50,50,50,40,40,40},	// left side accel
	{40,40,40,50,50,50,50,50,50,50,50,60,60,60,60,60,60,60,90,90,90}	// right side accel
};

int Mot_Descript_Righty[3][21] = 
{
	{0x0000,0x0001,0x0003,0x0007,0x000F,0x001E,0x003C,0x0078,0x00F0,0x01E0,0x03C0,0x0780,0x0F00,0x1E00,0x3C00,0x7800,0xF000,0xE000,0xC000,0x8000,0x0000},
	{90,90,90,60,60,60,60,60,60,60,50,50,50,50,50,50,50,50,50,50,50},	// left side accel
	{40,40,40,50,50,50,50,50,50,50,50,50,50,50,50,60,60,60,60,60,60}	// right side accel
};
	
