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
// File Name:ckr_oneBoard.c
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
//#include "../ckr_oneBoard.h"
//#include <windows.h>
#include "main.h"
#include <SimpleModbusRtu_mega2560.h>
#include <EEPROM.h>

/*********************************************************************************************
* -. Internal Definition
*********************************************************************************************/
//	1. PIN Sensor 
//	2. Bottome Line Sensor
//	3. Motor - Move
//	4. RFID Tag - Serial Port
//	5. LCD Interface

/*********************************************************************************************
* 2. GLOBAL VALUE
*********************************************************************************************/
AGV_CB agv_cb;
AGV_STATUS agv_status;
AGV_REQ agv_req;
STATUS_INFO status_info;
REQUEST_INFO req_info;
RFID_DATA rfid_data;
PHOTO_SEN photo_sen;
LCD_DATA lcd_data;
//GET_LCD_DATA get_lcd_data;
//LCD_DISP_ADDR lcd_disp_adrr;

bool rfid_check_flag = FALSE;
int currStatus = 0; // ckr_jhshin_220624[current status info set]
int agv_number[10][9]; // ckr_jhshin_220701[Navigation Info]
int agv_top_rfid_num = 1;
int cnt = 0;

unsigned char rfid_buf[7] = {0, 0, 0, 0, 0, 0, 0};

/*********************************************************************************************
* 3. INLINE FUNCTION
*********************************************************************************************/

void serialEvent1(){
//no use

}

//*********************************************************************************************
// Function : serialEvent2
//
// Date : 2022-07-04
//
// Author : jhshin 
//
// Description : RFID Tag Serial Declare
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
void serialEvent2() {

	CH2_RXBUFF = Serial2.read();
	rfid_buf[cnt]=CH2_RXBUFF;  // Input Data to RFID Buffer 
	cnt++;
	
	if(CH2_RXBUFF == 0xBB)  // RFID Buffer 값이 0xBB일때
	{
		cnt = 0;
		if((rfid_buf[0] == 0xAA) && (rfid_buf[2] == 0x00) && (agv_cb.rfid_tag != rfid_buf[1])) 
		{
			agv_cb.rfid_tag = rfid_buf[1];
			rfid_buf[0] = 0;
			rfid_check_flag = TRUE;
			
			Serial.println("Rfid_check_flag!!");
			Serial.println(agv_cb.rfid_tag);                
		}
	}  
}

void serialEvent3(){
//no use

}

/*********************************************************************************************
* 4. FUNCTION
*********************************************************************************************/
static int test(void);
static int ckr_agv_wake(void);
static int ckr_error(void);
static int ckr_idle(void);
static int ckr_agv_init(void);
static int ckr_agv_stop(void);
static int ckr_agv_init_done(void);
static int ckr_agv_retry (void);
static void SENextStatus(AGV_STATUS sts, AGV_REQ events);
static void ckr_rfid_init(void);
static void ckr_init();
static int ckr_agv_go(void);
static void ckr_motor_go(bool direction_info);
static int ckr_get_table_idx(bool direction);
static void ckr_motor_des_init();
static void ckr_eeprom_clear();
static int ckr_eeprom_write();
static int ckr_check_eeprom_changed(void);
static int ckr_rfid_request_checker(void);


typedef int (*SRFunction) (void);

// Function Pointer Matrix
static SRFunction AGVMatrix[AGV_STATUS_LAST][AGV_REQ_LAST] = {
  /*                none            move              e_stop			PIN	          	test*/
  /* Off */		{&ckr_agv_wake,     &ckr_error,       &ckr_error,		&ckr_error,		&ckr_error},
  /* init */    {&ckr_agv_init,     &ckr_error,       &ckr_error,		&ckr_error,		&ckr_error},
  /* idle */    {&ckr_idle,         &ckr_agv_go,	  &ckr_agv_stop,  	&ckr_agv_pin, 	&test},
  /* error */   {&ckr_error,        &ckr_error,       &ckr_error,		&ckr_error,		&ckr_error},
};

//*********************************************************************************************
// Function : setup
//
// Date : 2022-06-30
//
// Author : jhshin 
//
// Description : when board is boot up, just one-time set value
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
void setup() {
	ckr_init();
    
    agv_cb.status.currStatus = AGV_OFF_STATE;
    agv_cb.request.currReq = AGV_REQ_NONE;
}
//*********************************************************************************************
// Function : 
//
// Date : 2022-06-24
//
// Author : jhshin 
//
// Description : SENextStatus
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
void SENextStatus(AGV_STATUS sts, AGV_REQ events)
{
    agv_cb.status.currStatus = (AGV_STATUS)((SRFunction) *(AGVMatrix[sts][events]))();
}

//*********************************************************************************************
// Function : 
//
// Date : 
//
// Author : jhshin 
//
// Description : 
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
void loop()
{
    SENextStatus(agv_cb.status.currStatus, agv_cb.request.currReq);
    delay(5);
}
//*********************************************************************************************
// Function : 
//
// Date : 
//
// Author : jhshin 
//
// Description : 
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
static int ckr_agv_wake(void)
{
  	Serial.println("ckr_agv_wake - Init Done !!!");
	//lcd_display_status_set_value(LCD_WAIT);
  	delay(1000);

	return(AGV_INIT_STATE);
}

//*********************************************************************************************
// Function : 
//
// Date : 
//
// Author : jhshin 
//
// Description : 
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
static int ckr_error(void)
{
    Serial.println("error state");
	lcd_display_status_set_value(LCD_ERROR);
    delay(1000);
    return 0;
}

//*********************************************************************************************
// Function : 
//
// Date : 
//
// Author : jhshin 
//
// Description : 
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
static int ckr_idle(void)
{
	int result = 0;
	int rfid_req = 0;
	
	//Serial.println("ckr_idle");
	delay(1000);
	lcd_display_status_set_value(LCD_READY);

	result = ckr_check_eeprom_changed();
	if(result == R_OK)
	{
		agv_cb.top_rfid_number = lcd_data.uiTranslateData[LCD_ADDR_AGV_TOP];
		agv_cb.lefty_tag_number = lcd_data.uiTranslateData[LCD_ADDR_LEFTY];
		agv_cb.righty_tag_number = lcd_data.uiTranslateData[LCD_ADDR_RIGHTY];

#ifdef DEBUG_MODE
		Serial.println(agv_cb.top_rfid_number);  //khkim3[20220708][test]
		Serial.println(agv_cb.lefty_tag_number); //khkim3[20220708][test]
		Serial.println(agv_cb.righty_tag_number); //khkim3[20220708][test]

		Serial.println(EEPROM.read(EEPROM_ADDR_RFIDTOP));
		Serial.println(EEPROM.read(EEPROM_ADDR_LEFTY));
		Serial.println(EEPROM.read(EEPROM_ADDR_RIGHTY));
#endif /* DEBUG_MODE */
		
		result = ckr_eeprom_write();
		if(result == R_OK)
		{
			lcd_data.uiTranslateData[LCD_ADDR_LAST] = FALSE;
		}
		else
		{
			Serial.println("EEPROM Write NG");
		}
	}

	rfid_req = ckr_rfid_request_checker();

#ifdef NEVER	// START BTN PUSH
	if ((agv_cb.request.currReq == AGV_REQ_NONE) && (agv_cb.start_btn_flag == TRUE))
	{
		Serial.println("start_btn_flag");
		agv_cb.request.currReq = AGV_REQ_MOVE;
	}
	else
#endif /* NEVER */
	{
		Serial.println("AGV_REQ_MOVE, IDLE STATE");
  		agv_cb.request.currReq = rfid_req;  //jhshin for test
		return (AGV_IDLE_STATE);

	}
}

//*********************************************************************************************
// Function : 
//
// Date : 
//
// Author : jhshin 
//
// Description : 
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
static int ckr_agv_init(void)
{
	int result = 0;
	
	Serial.println("ckr_agv_init");
	// ckr_jhshin_220701[slave address of mode bus, memory of register, coil memory][communicate with ComfileHMI]    
	startSimpleModbus(1,lcd_data.uiTranslateData, photo_sen.ptTranslateData);
		
	lcd_display_status_set_value(LCD_INIT);

	// EEPROM READ -> struct info copy
	result = ckr_eeprom_read();
	if(result == R_NG)
	{
		Serial.println("eeprom read error!!");
		lcd_display_status_set_value(LCD_ERROR);
		agv_cb.request.currReq = AGV_REQ_NONE;
		return (AGV_INIT_STATE);
	}
	lcd_display_set_init_value();

	agv_cb.direction_info = DIR_LEFTY;
	
	delay(100);
	return (AGV_IDLE_STATE);
}
//*********************************************************************************************
// Function : lcd_display_set_init_value
//
// Date : 
//
// Author : jhshin 
//
// Description : 
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
static void lcd_display_set_init_value()
{
	int tmp_pin_sts = 0;

	tmp_pin_sts = ckr_get_pin_sts();
	
  	Serial.println("lcd_display_set_init_value");
	// LCD Display Setting Value Init
	//lcd_data.uiTranslateData[LCD_ADDR_DRV_STS] = LCD_INIT;			// ckr_jhshin_220701[DRIVE STS]
	lcd_data.uiTranslateData[LCD_ADDR_PIN_STS] = tmp_pin_sts;		// ckr_jhshin_220701[Pin STS]
	lcd_data.uiTranslateData[LCD_ADDR_RFID_NUM] = 0xFF;				// ckr_jhshin_220701[RFID Value]
	lcd_data.uiTranslateData[LCD_ADDR_AGV_TOP] = agv_cb.top_rfid_number;		// ckr_jhshin_220701[AGV NUMBER]
	lcd_data.uiTranslateData[LCD_ADDR_LEFTY] = agv_cb.lefty_tag_number;		// ckr_jhshin_220701[LEFTY RFID NUM]
	lcd_data.uiTranslateData[LCD_ADDR_RIGHTY] = agv_cb.righty_tag_number;		// ckr_jhshin_220701[RIGHTY RFID NUM]
	lcd_data.uiTranslateData[LCD_ADDR_LAST] = 0;					// ckr_jhshin_220701[TRIGGER]
		
}
//*********************************************************************************************
// Function : lcd_display_status_set_value
//
// Date : 
//
// Author : jhshin 
//
// Description : 
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
static void lcd_display_status_set_value(int status)
{
	lcd_data.uiTranslateData[LCD_ADDR_DRV_STS] = status;			// ckr_jhshin_220701[DRIVE STS]
}

//*********************************************************************************************
// Function : ckr_get_pin_sts
//
// Date : 2022-07-07
//
// Author : jhshin 
//
// Description : Check Pin status and backup the status of pin
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
static int ckr_get_pin_sts(void)
{
	int result = 0;
	// 채워넣을 것!!!!
	if(1)		result = PIN_UP;
	else		result = PIN_DOWN;
		
	return result;
}
//*********************************************************************************************
// Function : 
//
// Date : 
//
// Author : jhshin 
//
// Description : 
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
static int ckr_agv_stop(void)
{

	Serial.println("agv_eStop\n");
	lcd_display_status_set_value(LCD_STOP);

	// ckr_jhshin_220705[Motor STOP]
	motor_stop_cmd();

	switch(agv_cb.rfid_tag)
	{
		case RFID_TAG_STOP:
			agv_cb.request.currReq = AGV_REQ_NONE;
			break;
		case RFID_TAG_PINDN_GO:
		case RFID_TAG_PINUP_GO:
			agv_cb.request.currReq = AGV_REQ_PIN;
			break;
	}
	return (AGV_IDLE_STATE);
}
//agv_cb.request.currReq = AGV_REQ_PIN;
//*********************************************************************************************
// Function : 
//
// Date : 
//
// Author : jhshin 
//
// Description : 
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
static void motor_stop_cmd(void)
{
	analogWrite(LEFT_MOTOR_PIN,0);
	analogWrite(RIGHT_MOTOR_PIN,0);
}
//*********************************************************************************************
// Function : ckr_agv_pin
//
// Date : 
//
// Author : jhshin 
//
// Description : PIN UP & DOWN 도영씨 코드에 따라서 변경...
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
static int ckr_agv_pin(void)
{
	int check_result = 0;
	Serial.println("ckr_agv_pin\n");
	
	if(agv_cb.rfid_tag == RFID_TAG_PINUP_GO)
	{
		// PIN UP
		
		//PIN Status Check
		
		//if PIN Check NG, then retry one more time
		if(check_result == R_OK)
		{
			agv_cb.request.currReq = AGV_REQ_MOVE;
		}
		else
		{
			// retry sequence Here!!!
		}
	}
	else if(agv_cb.rfid_tag == RFID_TAG_PINDN_GO)
	{
		// PIN DOWN
		
		//PIN Status Check
		
		//if PIN Check NG, then retry one more time
		if(check_result == R_OK)
		{
			agv_cb.request.currReq = AGV_REQ_MOVE;
		}
		else
		{
			// retry sequence Here!!!
		}		
	}
	else
	{
		Serial.println("Unkown PIN Check");
	}
	return check_result;
}
//*********************************************************************************************
// Function : 
//
// Date : 
//
// Author : jhshin 
//
// Description : 
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
static int test(void)
{
	Serial.println("test");
	lcd_display_status_set_value(LCD_TEST);
	delay(1000);
	return 0;
}
//*********************************************************************************************
// Function : 
//
// Date : 
//
// Author : jhshin 
//
// Description : 
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
static int ckr_agv_init_done(void)
{
	Serial.println("agv_init_done");
	delay(1000);
	return 0;
}
//*********************************************************************************************
// Function : 
//
// Date : 
//
// Author : jhshin 
//
// Description : 
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
static int ckr_agv_retry(void)
{
  Serial.println("agv_retry");
  delay(1000);
  return 0;
}

//*********************************************************************************************
// Function : ckr_agv_go
//
// Date : 
//
// Author : jhshin 
//
// Description : 
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
static int ckr_agv_go(void)
{
	//Serial.println("ckr_agv_go");
	delay(10);
	
	int get_mot_description = 0;
	lcd_display_status_set_value(LCD_MOVE);

	ckr_motor_go(agv_cb.direction_info);	
	
	return (AGV_IDLE_STATE);
}


//*********************************************************************************************
// Function : ckr_motor_go
//
// Date : 2022-07-05
//
// Author : jhshin 
//
// Description : 
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
static void ckr_motor_go(bool direction_info)
{
	int idx = 0;
	
#ifdef NEVER
	Serial.println("ckr_motor_go");
#endif /* NEVER */

	idx = ckr_get_table_idx(direction_info);

	if(direction_info == DIR_LEFTY)
	{
		if(idx == R_NONE)
		{
			// line sensor value is not included in acc table
			// 라인 센서 왼쪽값부터 ON 갯수 카운트해서 유효한 데이터임을 확인하는 함수 여기 추가!!!!
			analogWrite(LEFT_MOTOR_PIN,Mot_Descript_Lefty[1][7]);
			analogWrite(RIGHT_MOTOR_PIN,Mot_Descript_Lefty[2][7]);
			//Serial.print("1. Motor Table: ");
			//Serial.print(idx);
		}
		else
		{
			analogWrite(LEFT_MOTOR_PIN,Mot_Descript_Lefty[1][idx]);
			analogWrite(RIGHT_MOTOR_PIN,Mot_Descript_Lefty[2][idx]);
			//Serial.print("2. Motor Table: ");
			//Serial.print(idx);			
		}
	}
	else
	{
		if(idx == R_NONE)
		{
			// line sensor value is not included in acc table
			// 라인 센서 오른쪽값부터 ON 갯수 카운트해서 유효한 데이터임을 확인하는 함수 여기 추가!!!! 
			analogWrite(LEFT_MOTOR_PIN,Mot_Descript_Lefty[1][7]);
			analogWrite(RIGHT_MOTOR_PIN,Mot_Descript_Lefty[2][7]);
			//Serial.print("3. Motor Table: ");
			//Serial.print(idx);
		}
		else
		{
			analogWrite(LEFT_MOTOR_PIN,Mot_Descript_Righty[1][idx]);
			analogWrite(RIGHT_MOTOR_PIN,Mot_Descript_Righty[2][idx]);
			//Serial.print("4. Motor Table: ");
			//Serial.print(idx);			
		}
			
	}
}



static int ckr_get_table_idx(bool direction)
{
	int result = R_NONE;
	int i = 0;
	
	if(direction == DIR_LEFTY)
	{
		for(i = 0; i < 21; i++)
		{
			if(agv_cb.BOTTOM_LINE_SEN == Mot_Descript_Lefty[0][i])
			{
				return i;
			}
		}
	}
	else
	{
		for(i = 0; i < 21; i++)
		{
			if(agv_cb.BOTTOM_LINE_SEN == Mot_Descript_Righty[0][i])
			{
				return i;
			}
		}
	}
	return result;	
}
//*********************************************************************************************
// Function : ckr_rfid_init
//
// Date : 
//
// Author : jhshin 
//
// Description : Interface with RFID Sensor Board (Serial2)
//               
// Input Parameters
// BoadRate : 38400
//
// Output Parameters
// none
//
//*********************************************************************************************
static void ckr_rfid_init(void)
{
	Serial2.begin(BOADRATE_38400);
	agv_cb.rfid_tag = 205;
}

//*********************************************************************************************
// Function : ckr_photosensor_init
//
// Date : 
//
// Author : jhshin 
//
// Description : 
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
static void ckr_photosensor_init(void)
{
	char i = 0;

	// ckr_jhshin_220701[Input Number 22 ~ 37][Board Input Port Number - using Line Sensor]
	for(i=22; i<38; i++) {
		pinMode(i, INPUT); 
	} 
}
//*********************************************************************************************
// Function : ckr_eeprom_init
//
// Date : 2022.07.04
//
// Author : jhshin 
//
// Description : EEPROM Init before Use
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
void ckr_eeprom_init()
{
	for (int i = 0 ; i < EEPROM.length() ; i++) {
		EEPROM.write(i, 0);
	}
}
//*********************************************************************************************
// Function : ckr_system_log_init
//
// Date : 
//
// Author : jhshin 
//
// Description : System Log (Serial)
//
// Input Parameters
// Boadrate: 57600
//
// Output Parameters
// none
//
//*********************************************************************************************
static void ckr_system_log_init(void)
{
	Serial.begin(BOADRATE_57600);
}
//*********************************************************************************************
// Function : ckr_display_init
//
// Date : 
//
// Author : jhshin 
//
// Description : LCD Init
//
// Input Parameters
// Boadrate: 57600
//
// Output Parameters
// none
//
//*********************************************************************************************
static void ckr_display_init(void)
{
	usart1init(BOADRATE_57600);
}
//*********************************************************************************************
// Function : ckr_set_lcd
//
// Date : 2022-07-01
//
// Author : jhshin 
//
// Description : LCD Display Info Set
//
// Input Parameters
// none
// Output Parameters
// none
//
//*********************************************************************************************
static void ckr_set_lcd(){
	// function info (status - wake, ,,,)
	// Current Pin status
	// Current RFID Tag info
	//LCD_DISPLAY_DATA
	
	lcd_data.uiTranslateData[0];
	
}
//*********************************************************************************************
// Function : ckr_get_lcd
//
// Date : 2022-07-01
//
// Author : jhshin 
//
// Description : LCD Display Info Get
//
// Input Parameters
// none
// Output Parameters
// none
//
//*********************************************************************************************
static void ckr_get_lcd(){

}
//*********************************************************************************************
// Function : ckr_init
//
// Date : 2022-07-01
//
// Author : jhshin 
//
// Description : AGV Initial
//
// Input Parameters
// none
// Output Parameters
// none
//
//*********************************************************************************************
static void ckr_init()
{
	agv_cb.start_btn_flag = FALSE;
	
	ckr_system_log_init();	// System Log Init
	ckr_rfid_init();		// RFID Tag Init
	ckr_photosensor_init();	// PhotoSensor Init
	ckr_display_init();		// LCD Init - Serial IF Boadrate init
	ckr_motor_des_init();	 // ckr_jhshin_220704[motor_description]
	//ckr_eeprom_init();		 // ckr_jhshin_220704[EEPROM Init]
}
//*********************************************************************************************
// Function : ckr_eeprom_read
//
// Date : 2022-07-04
//
// Author : jhshin 
//
// Description : 
//
// Input Parameters
// none
//
// Output Parameters
// int result
//
//*********************************************************************************************
static int ckr_eeprom_read()
{
	agv_cb.top_rfid_number = EEPROM.read(EEPROM_ADDR_RFIDTOP);
	agv_cb.lefty_tag_number = EEPROM.read(EEPROM_ADDR_LEFTY);
	agv_cb.righty_tag_number = EEPROM.read(EEPROM_ADDR_RIGHTY);
	
	delay(100);

	if((agv_cb.top_rfid_number == NULL) || (agv_cb.lefty_tag_number == NULL) || (agv_cb.righty_tag_number == NULL))
	{
		agv_cb.top_rfid_number = RFID_AGV_TOP_INIT_NUMBER;
		agv_cb.lefty_tag_number = RFID_TAG_LEFTY;
		agv_cb.righty_tag_number = RFID_TAG_RIGHTY;
		//return R_NG;
	}
#ifdef NEVER
	else if(EEPROM_ADDR_RIGHTY+1 == EEPROM.length())
	{
		return R_NG;
	}
#endif /* NEVER */
	else
	{
		return R_OK;
	}
}
//*********************************************************************************************
// Function : ckr_eeprom_write
//
// Date : 2022-07-04
//
// Author : jhshin 
//
// Description : Get value from LCD Display 
//
// Input Parameters
// none
//
// Output Parameters
// int result
//
//*********************************************************************************************
static int ckr_eeprom_write()
{

	ckr_eeprom_clear();
	
	EEPROM.write(EEPROM_ADDR_RFIDTOP, agv_cb.top_rfid_number);
	EEPROM.write(EEPROM_ADDR_LEFTY, agv_cb.lefty_tag_number);
	EEPROM.write(EEPROM_ADDR_RIGHTY, agv_cb.righty_tag_number);

	//Serial.println(EEPROM.length());	// jhshin_20220708[EEPROM SIZE is 4096. 4kbyte]

	delay(100);

#ifdef DEBUG_MODE
	Serial.println(EEPROM.read(EEPROM_ADDR_RFIDTOP));
	Serial.println(EEPROM.read(EEPROM_ADDR_LEFTY));
	Serial.println(EEPROM.read(EEPROM_ADDR_RIGHTY));

	Serial.println(lcd_data.uiTranslateData[LCD_ADDR_AGV_TOP]);
	Serial.println(lcd_data.uiTranslateData[LCD_ADDR_LEFTY]);	
	Serial.println(lcd_data.uiTranslateData[LCD_ADDR_RIGHTY]);

#endif /* DEBUG_MODE */

	return R_OK;

}
//*********************************************************************************************
// Function : ckr_check_eeprom_changed
//
// Date : 
//
// Author : jhshin 
//
// Description : 
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
static int ckr_check_eeprom_changed(void)
{

	//Serial.println(lcd_data.uiTranslateData[LCD_ADDR_LAST]);  //khkim3[20220708][test]
	if(lcd_data.uiTranslateData[LCD_ADDR_LAST])
	{
		return R_OK;
	}
	else 
	{
		return R_NG;
	}
}
//*********************************************************************************************
// Function : ckr_eeprom_clear
//
// Date : 2022-07-04
//
// Author : jhshin 
//
// Description : 
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
static void ckr_eeprom_clear()
{
	int i = 0;

	for(i = 0; i< EEPROM.length(); i++)
	{
		EEPROM.write(i, 0);
	}
	Serial.println("EEPROM clear");  //khkim3[20220708][test]
}

//*********************************************************************************************
// Function : ckr_motor_des_init
//
// Date : 2022-07-04
//
// Author : jhshin 
//
// Description : Left, Right Motor Init
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
static void ckr_motor_des_init()
{
	pinMode(agv_cb.mot_des.LEFT_MOTOR_SPEED, OUTPUT);
	pinMode(agv_cb.mot_des.LEFT_MOT_SET_DRIVE, OUTPUT);
	pinMode(agv_cb.mot_des.LEFT_MOT_BREAK, OUTPUT);
	pinMode(agv_cb.mot_des.LEFT_MOT_DIR, OUTPUT); 

	digitalWrite(agv_cb.mot_des.LEFT_MOT_SET_DRIVE,HIGH);
	digitalWrite(agv_cb.mot_des.LEFT_MOT_BREAK,HIGH);
	digitalWrite(agv_cb.mot_des.LEFT_MOT_DIR,HIGH);
	
	pinMode(agv_cb.mot_des.RIGHT_MOTOR_SPEED, OUTPUT);
	pinMode(agv_cb.mot_des.RIGHT_MOT_SET_DRIVE, OUTPUT);
	pinMode(agv_cb.mot_des.RIGHT_MOT_BREAK, OUTPUT);
	pinMode(agv_cb.mot_des.RIGHT_MOT_DIR, OUTPUT); 

	digitalWrite(agv_cb.mot_des.RIGHT_MOT_SET_DRIVE,HIGH);
	digitalWrite(agv_cb.mot_des.RIGHT_MOT_BREAK,HIGH);
	digitalWrite(agv_cb.mot_des.RIGHT_MOT_DIR,HIGH);
}

//*********************************************************************************************
// Function : ckr_rfid_request_checker
//
// Date : 
//
// Author : jhshin 
//
// Description : 
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
static int ckr_rfid_request_checker(void)
{
	int req_check = 0;
	
	if(rfid_check_flag == TRUE)
	{
		Serial.print("ckr_agv_go & rfid check!! ");
		Serial.println(agv_cb.rfid_tag);

		switch (agv_cb.rfid_tag)
		{ 
			case RFID_TAG_L_TURN :
			case RFID_TAG_R_TURN:
			case RFID_TAG_STOP:
			case RFID_TAG_PINDN_GO:
			case RFID_TAG_PINUP_GO: 			
				req_check = AGV_REQ_STOP;
				break;
			case RFID_TAG_SPEEDUP:
				req_check = AGV_REQ_MOVE;
				agv_cb.MOTOR_SPEED = HIGH_SPEED;
				break;
			case RFID_TAG_SPEEDDN:
				req_check = AGV_REQ_MOVE;
				agv_cb.MOTOR_SPEED = LOW_SPEED;
				break;		
			case agv_cb.lefty_tag_number:
				agv_cb.direction_info = DIR_LEFTY;
				req_check = AGV_REQ_MOVE;
				break;	
			case agv_cb.righty_tag_number:
				agv_cb.direction_info = DIR_RIGHTY;
				req_check = AGV_REQ_MOVE;
				break;
			case RFID_TAG_START:
				req_check = AGV_REQ_MOVE;
				break;
			default:
				Serial.println("Unkown Tag");
				Serial.print(agv_cb.rfid_tag);
				break;
		}
		rfid_check_flag = FALSE;
	}
	return req_check;
}

