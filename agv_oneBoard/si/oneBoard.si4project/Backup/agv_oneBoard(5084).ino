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
bool pin_status_toggle = FALSE;
bool rfid_check_flag = FALSE;
#if 0 //def PARKDY
bool front_sen_flag = FALSE;
bool bottom_sen_flag = FALSE;
#endif

int currStatus = 0; // ckr_jhshin_220624[current status info set]
int agv_number[10][9]; // ckr_jhshin_220701[Navigation Info]
int agv_top_rfid_num = 1;
int cnt = 0;
int duration = 10000, time_out = 0;

unsigned int MOT_TABLE_IDX_BACKUP = 0;
unsigned char rfid_buf[7] = {0, 0, 0, 0, 0, 0, 0};
unsigned long curr_time = 0, pre_time = 0;


int table_idx_history[20] = {0,0};
int tbl_idx_cnt = 0;

/*********************************************************************************************
* 3. INLINE FUNCTION
*********************************************************************************************/
// 1. LCD: Callback Function
// 2. STOP BTN : Callbak Function
// 3. XBEE : ISR?
// 4. RFID : Serial - ISR 
// 5. PHOTOSENSOR : INPUT PIN

//*********************************************************************************************
// Function : serialEvent2
//
// Date : 2022-07-04
//
// Author : jhshin 
//
// Description : RFID Tag , Fron Sensor, Bottom Line sensor
//
// Input Parameters
// RFID : AA TAG 00 BB
// FRONT SENSOR : CC SENSOR DD
// BOTTOM LINE SENSOR : EE BOT_SEN FE
// Output Parameters
// none//
//*********************************************************************************************
void serialEvent2() {

	RFID_RXBUFF = Serial2.read();
	
	rfid_buf[cnt]=RFID_RXBUFF;  // Input Data to RFID Buffer 
	cnt++;
	
	if(RFID_RXBUFF == 0xBB)  // RFID Buffer 값이 0xBB일때
	{
		cnt = 0;
		if((rfid_buf[0] == 0xAA) && (rfid_buf[2] == 0x00) && (agv_cb.rfid_tag != rfid_buf[1])) 
		{
			agv_cb.rfid_tag = rfid_buf[1];
			for(int i = 0; i < 7; i++){
				rfid_buf[i] = 0;
			}
			
			rfid_check_flag = TRUE;
			lcd_data.uiTranslateData[LCD_ADDR_RFID_NUM] = agv_cb.rfid_tag;
			Serial.println("Rfid_check_flag!!");
			Serial.println(agv_cb.rfid_tag);
			//Serial.println(lcd_data.uiTranslateData[LCD_ADDR_RFID_NUM]);
		}
	}

#if 0 //def PARKDY
	// FRONT SENSOR : CC SENSOR DD
	// FRONT SENSOR VALUE IN : default(00), left on(10), right on (01)
	if(RFID_RXBUFF == 0xDD)
	{
		if(rfid_buf[0] == 0xCC)
		{
			bool front_sen_flag = TRUE;
			agv_cb.front_sensor_sts = rfid_buf[1];
		}
	}
	
	// BOTTOM LINE SENSOR : EE BOT_SEN FE
	// BOTTOM LINE SENSOR VALUE IN
	if(RFID_RXBUFF == 0xFE)
	{
		if(rfid_buf[0] == 0xEE)
		{
			bool bottom_sen_flag = TRUE;			
			agv_cb.BOTTOM_LINE_SEN = rfid_buf[1];
		}
	}
#endif /* DEBUG_MODE */
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
static int ckr_motor_go(void);
static int ckr_get_table_idx(bool direction);
static void ckr_motor_des_init();
static void ckr_eeprom_clear();
static int ckr_eeprom_write();
static void ckr_check_eeprom_changed(void);
static int ckr_rfid_request_checker(int curr_function);
static void stop_btn_callback(void);
static void ckr_LR_motor_description_set(void);
static bool ckr_pin_btn_check(int rfid_tag);
static int ckr_eeprom_read(void);
static void ckr_pin_up_move(void);
static void ckr_pin_stop(void);
static int ckr_pin_check_stop(int status);
static void ckr_pin_down_move(void);
static int check_photo_sen_status(int rfid_input);
static int ckr_read_bottom_line(void);
static void ckr_bottom_line_sensor_checker(void);


typedef int (*SRFunction) (void);

// Function Pointer Matrix
static SRFunction AGVMatrix[AGV_STATUS_LAST][AGV_REQ_LAST] = {
  /*                none            move
  /* Off */		{&ckr_agv_wake,     &ckr_error},
  /* init */    {&ckr_agv_init,     &ckr_error},
  /* idle */    {&ckr_idle,         &ckr_agv_go},
  /* error */   {&ckr_error,        &ckr_error},
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
	
  	delay(100);

	return(AGV_INIT_STATE);
}

//*********************************************************************************************
// Function : 
//
// Date : 
//
// Author : jhshin 
//
// Description : If Error Occured, stay in error status until system reboot..
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
	// motor stop 

    delay(1000);
	
	agv_cb.request.currReq = AGV_REQ_NONE;
    return (AGV_ERROR_STATE);
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
	int rfid_req = 0;
	bool pin_sts = 0;

	//Serial.println("ckr_idle");
	
#if 0 //def TEST
	lcd_display_status_set_value(LCD_READY);
#endif /* DEBUG_MODE */

	// EEPROM UPDATE CHECK
	ckr_check_eeprom_changed();

	// RFID TAGGING PROCESS
	if(rfid_check_flag == TRUE)
	{
		rfid_req = ckr_rfid_request_checker(LCD_READY);
		if(rfid_req != 0)
		{
			Serial.print("IDLE - RFID CHECK : ");
			Serial.println(rfid_req);
			agv_cb.request.currReq = rfid_req;
		}
	}

	// PIN BTN PUSH EVENT PROCESS
	if(0) {//digitalRead(PIN_BUTTON) == LOW){
		pin_sts = ckr_pin_btn_check(0);
		if(pin_sts == FALSE)
		{
			lcd_display_status_set_value(E100);	// PIN ERROR DISPLAY
			agv_cb.request.currReq = AGV_REQ_NONE;
			return (AGV_ERROR_STATE);
		}
	}

	// START BTN PUSH EVENT PROCESS
	if ((agv_cb.motor_go_btn_flag == FALSE) && ckr_start_btn_check())
	{
		Serial.println("1. start btn pushed");
		//agv_cb.request.currReq = AGV_REQ_MOVE;
	}
	else
	{
		if(agv_cb.drv_info.move_start_by_rfid_tag == TRUE)
		{
			Serial.println("2. start tag");
			agv_cb.request.currReq = AGV_REQ_MOVE;
		}
		else
		{
	  		agv_cb.request.currReq = AGV_REQ_NONE;  //jhshin for test
		}
	}

	return (AGV_IDLE_STATE);
}
//*********************************************************************************************
// Function : ckr_start_btn_check
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
static bool ckr_start_btn_check(void)
{
	if(digitalRead(START_BUTTON) == LOW)
	{
		agv_cb.motor_go_btn_flag = TRUE;
#ifdef DEBUG_MODE
		Serial.println("START_BUTTON Pressed");
#endif /* DEBUG_MODE */
	}
	else
	{
		agv_cb.motor_go_btn_flag = FALSE;
#ifdef DEBUG_MODE
		Serial.println("START_BUTTON Not Pressed");
#endif /* DEBUG_MODE */
	}
	return agv_cb.motor_go_btn_flag;
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
		
#if 0 //def TEST
	lcd_display_status_set_value(LCD_INIT);
#endif /* DEBUG_MODE */

	// EEPROM READ -> struct info copy
	result = ckr_eeprom_read();
	if(result == R_NG)
	{
		Serial.println("eeprom read error!!");
#if 0 //def TEST
		lcd_display_status_set_value(LCD_ERROR);
#endif /* DEBUG_MODE */
		agv_cb.request.currReq = AGV_REQ_NONE;
		return (AGV_INIT_STATE);
	}
#if 0 //def TEST
	lcd_display_set_init_value();
#endif /* DEBUG_MODE */

	agv_cb.drv_info.drv_direction_info = DIR_LEFTY;
	
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
	if(status != lcd_data.uiTranslateData[LCD_ADDR_DRV_STS])
	{
		Serial.println(status);
		Serial.println(lcd_data.uiTranslateData[LCD_ADDR_DRV_STS]);		
		Serial.println("lcd_display_status_set_value");
		
		lcd_data.uiTranslateData[LCD_ADDR_DRV_STS] = status;			// ckr_jhshin_220701[DRIVE STS]
	}
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

	if((_RD_PHOTO_TOP == 0) && (_RD_PHOTO_BOT == 1))	// PIN is UP status		
	{
		Serial.print("ckr_get_pin_sts : ");
		Serial.println(PIN_UP);	
		result = PIN_UP;
	}
	else
	{
	
		Serial.print("ckr_get_pin_sts : ");
		Serial.println(PIN_DOWN);
		result = PIN_DOWN;
	}
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
static void motor_stop_cmd(void)
{
	Serial.println("agv_stop cmd");
	ckr_LR_motor_description_set();
	ckr_R_motor_table_set(0);
	ckr_L_motor_table_set(0);

}

//*********************************************************************************************
// Function : ckr_pin_down_move
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
static void ckr_pin_down_move(void)
{
	if(lcd_data.uiTranslateData[LCD_ADDR_PIN_STS] != PIN_DOWN)
	{
		lcd_data.uiTranslateData[LCD_ADDR_PIN_STS] = PIN_DOWN;
#ifdef DEBUG_MODE
		Serial.println("lcd_display_status_set_ckr_pin_down_move");
#endif /* DEBUG_MODE */
	}

	digitalWrite(PIN_MOTOR_DIR, LOW);
	digitalWrite(PIN_MOTOR_ENABLE, LOW);
	analogWrite(PIN_MOTOR_PWM, 255);
	//delay(1000);
}
//*********************************************************************************************
// Function : ckr_pin_check_stop
//
// Date : 
//
// Author : jhshin 
//
// Description : TOP == 0 then PIN is UP... BOTTOM == 0 then, PIN is Down
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
static int ckr_pin_check_stop(int status)
{
	bool result = FALSE;
	int top_sen = 0, bot_sen = 0;
	switch (status)
	{
		case PIN_HIGH_HIGH:
#ifdef DEBUG_MODE
			Serial.println("PIN_HIGH_HIGH");
#endif /* DEBUG_MODE */
			top_sen = 1;
			bot_sen = 0;
			break;
		case PIN_LOW_HIGH:
#ifdef DEBUG_MODE
			Serial.println("PIN_LOW_HIGH");
#endif /* DEBUG_MODE */
			top_sen = 1;
			bot_sen = 0;
			break;
		case PIN_HIGH_LOW:
#ifdef DEBUG_MODE
			Serial.println("PIN_HIGH_LOW");
#endif /* DEBUG_MODE */
			top_sen = 0;
			bot_sen = 1;
			break;
		default:
			break;
	}
	
	do
	{
		curr_time = millis();
		if((_RD_PHOTO_TOP == top_sen) && (_RD_PHOTO_BOT == bot_sen))
		{
			// sensor stop
#ifdef DEBUG_MODE
			Serial.println("Pin move over!!");
#endif /* DEBUG_MODE */
			ckr_pin_stop();
			result = TRUE;
			break;
		}

		if((curr_time - pre_time) < duration){
			time_out = 1;
		}
		else{
#ifdef DEBUG_MODE
			Serial.println("Pin time out!!");
#endif /* DEBUG_MODE */
			time_out = 0;
			result = FALSE;
			break;
		}		
	}
	while (time_out);
	
	pre_time = curr_time;
	Serial.println("PIN Check END");
	
#ifdef DEBUG_MODE
	Serial.print(curr_time);
	Serial.print(" / ");
	Serial.println(pre_time);
#endif /* DEBUG_MODE */

	return result;
}

//*********************************************************************************************
// Function : ckr_pin_up_move
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
static void ckr_pin_up_move(void)
{
	if(lcd_data.uiTranslateData[LCD_ADDR_PIN_STS] != PIN_UP)
	{
		Serial.println("ckr_pin_up_move");
		lcd_data.uiTranslateData[LCD_ADDR_PIN_STS] = PIN_UP;
	}
	
	digitalWrite(PIN_MOTOR_DIR, HIGH);
	digitalWrite(PIN_MOTOR_ENABLE, LOW);
	analogWrite(PIN_MOTOR_PWM, 255);
	//delay(100);
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
static void ckr_pin_stop(void)
{
	digitalWrite(PIN_MOTOR_DIR, LOW);
	digitalWrite(PIN_MOTOR_ENABLE, LOW);
	analogWrite(PIN_MOTOR_PWM, 0);
	//delay(100);
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
	int req_response = 0;
	
	//Serial.println("ckr_agv_go");
	int get_mot_description = 0;
	lcd_display_status_set_value(LCD_MOVE);

	req_response = ckr_motor_go();
	agv_cb.request.currReq = req_response;
	
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
#if 0
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
#endif

//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
static int ckr_motor_go(void)
{
	int rfid_req = 0, result = 0, line_read_val = 0, i = 0, j = 0, k = 0, tbl_tmp = 0;;
	bool find_idx = FALSE;
	curr_time = millis();
	
#ifdef DEBUG_MODE
	Serial.println("ckr_motor_go");
#endif /* DEBUG_MODE */


	if(rfid_check_flag == TRUE)
	{
		rfid_req = ckr_rfid_request_checker(LCD_MOVE);
		if(rfid_req != 0)
		{
			result = rfid_req;
		}
	}
#ifdef DEBUG_MODE
	Serial.println("motor_go_btn_flag : " + agv_cb.motor_go_btn_flag + ", move_start_by_rfid_tag" + agv_cb.drv_info.move_start_by_rfid_tag);
#endif /* DEBUG_MODE */

	line_read_val = ckr_read_bottom_line(); // Bottom sensor read value
	Serial.print("bot line: ");
	Serial.println(line_read_val);

	// Unknown RFID Input 
	if(agv_cb.motor_go_btn_flag == 1)// || (agv_cb.drv_info.move_start_by_rfid_tag == TRUE))
	{
		//ckr_bottom_line_sensor_checker(); // jhshin[ParkDY Motor code][20220722]

		for(i = 0; i< 13 ; i++)
		{
			if((MOT_LEFTY_2[DIR_SPEED][i] & line_read_val) == MOT_LEFTY_2[DIR_SPEED][i])
			{
				ckr_L_motor_table_set(MOT_LEFTY_2[DIR_LEFTY][i]);
				ckr_R_motor_table_set(MOT_LEFTY_2[DIR_RIGHTY][i]);
				find_idx = TRUE;
				Serial.print("TBL: ");
				Serial.println(i);
				table_idx_history[_QUEUE_IN(tbl_idx_cnt++)] = find_idx;
				break;
			}
		}
		if(!find_idx)
		{
			for(j = 0; j<14; j++)
			{
				if((LEFT_SIDE_SEARCH_SENON[DIR_SPEED][j] & line_read_val) == LEFT_SIDE_SEARCH_SENON[DIR_SPEED][j])
				{
					ckr_L_motor_table_set(LEFT_SIDE_SEARCH_SENON[DIR_LEFTY][j]);
					ckr_R_motor_table_set(LEFT_SIDE_SEARCH_SENON[DIR_RIGHTY][j]);
					Serial.print("Exception TBL: ");
					Serial.println(j);
#ifdef DEBUG_MODE
					for(k = 0; k < 20; k++)
					{
						tbl_tmp += table_idx_history[k];
					}
					tbl_tmp  = (tbl_tmp / 20);
#endif /* DEBUG_MODE */
					break;
				}
			}
		}
		}
		else if((agv_cb.motor_go_btn_flag == 0) && (agv_cb.drv_info.move_start_by_rfid_tag == FALSE))
		{
			Serial.println("MOT STOP"); 
			motor_stop_cmd();
			//delay(500);
			agv_cb.request.currReq = AGV_REQ_NONE;
		}
		else	// agv_cb.motor_go_btn_flag == 0 && agv_cb.drv_info.move_start_by_rfid_tag == TRUE
		{
		
			//ckr_LR_motor_description_set();
			//ckr_R_motor_table_set(100);
			//ckr_L_motor_table_set(100);
			for(i = 0; i< 13 ; i++)
			{
				if((MOT_LEFTY_2[DIR_SPEED][i] & line_read_val) == MOT_LEFTY_2[DIR_SPEED][i])
				{
					ckr_L_motor_table_set(MOT_LEFTY_2[DIR_LEFTY][i]);
					ckr_R_motor_table_set(MOT_LEFTY_2[DIR_RIGHTY][i]);
					find_idx = TRUE;
					break;
				}
			}
			if(!find_idx)
			{
				for(j = 0; j<14; j++)
				{
					if((LEFT_SIDE_SEARCH_SENON[DIR_SPEED][j] & line_read_val) == LEFT_SIDE_SEARCH_SENON[DIR_SPEED][j])
					{
						ckr_L_motor_table_set(LEFT_SIDE_SEARCH_SENON[DIR_LEFTY][j]);
						ckr_R_motor_table_set(LEFT_SIDE_SEARCH_SENON[DIR_RIGHTY][j]);
						break;
					}
				}
				if(j > 13)
				{
					Serial.println("># Error Case Motor Table!!!");
				}
			}
	
			result = AGV_REQ_MOVE;
		}
		delay(50);
#ifdef DEBUG_MODE
		Serial.print("MOT P Time: ");
		Serial.println(curr_time - pre_time);
#endif /* DEBUG_MODE */

		result = AGV_REQ_MOVE;
	}

	pre_time = curr_time;
	return result;
}

int leftLinsensor = 0, rightLinsensor=0, leftFlag = 0, rightFlag = 0;

static void ckr_bottom_line_sensor_checker(void)
{
	int line_read_val = 0, leftLinsensor = 0, rightLinsensor = 0, tmp = 0;

	line_read_val = ckr_read_bottom_line();

	tmp = (line_read_val & 0b111111000000);
	leftLinsensor = tmp >> 6;
	rightLinsensor = (line_read_val & 0b000000111111);

	if(agv_cb.drv_info.drv_direction_info == DIR_LEFTY)	{
		leftyDrive(line_read_val);
	}
	else{
		rightDrive(line_read_val);
	}
	
}
void rightDrive(int line_val)
{
	switch(line_val)
	{
		// All Sensor Read Line (white)
		case 0b000000000000:
			Serial.print("		gogo			 ");
			goMotor();
			break;
		// Nothing
		case 0b111111111111:
			if(leftFlag > rightFlag)
			{
				Serial.print(" 		  LEFT 2	  ");
				leftFastMotor();
			}
			else if(leftFlag < rightFlag)
			{
				Serial.print(" 		  right 2	   ");
				rightFastMotor();
			}
			break;

		case 0b111111111110:
			Serial.print("			 LEFTY 2	  ");
			leftFlag++;
			leftFastMotor();
			break;

		case 0b111111111100:		
			Serial.println("		  LEFTY 1		");
			leftFlag++;
			leftMotor();
			break;
		case 0b111111111001:
		case 0b111111110011:  
		case 0b111111100111:
			Serial.println("		  gogo		");
			leftFlag=0;
			rightFlag=0;
			goMotor();
			break;
		case 0b111111001111:
			rightFlag++;
			rightMotor();
			break;   
		case 0b111110011111:
			rightFlag++;
			rightFastMotor();
			break;
		case 0b111100111111:
		case 0b111001111111:  
		case 0b110011111111:
		case 0b100111111111:
		case 0b001111111111:
		case 0b011111111111:
			Serial.println("		  righty 2		 ");
			rightFlag++;
			rightFastMotor();
			break;	  

		default:
			Serial.println("Default");

			if(leftLinsensor =4 && rightLinsensor <4)
			{
				rightMotor();
			}
			else if(leftLinsensor<rightLinsensor) //(leftFlag > rightFlag)
			{
				Serial.println("		  lefty 1		");
				leftMotor();
			}
			else if(leftLinsensor>rightLinsensor) //(leftFlag < rightFlag)
			{
				Serial.println("		  righty 1		 ");
				rightMotor();
			}
			else 
			{
				Serial.println("gogo");
				goMotor();
			}
			break;
	}
}

void leftyDrive(int line_read_val)
{
	switch(line_read_val)
	{
		case 0b000000000000:
			Serial.print("		gogo			 ");
			goMotor();
			break;

		case 0b111111111111:
			if(leftFlag > rightFlag)
			{
				Serial.print("		  LEFT 2	  ");
				leftFastMotor();
			}
			else if(leftFlag < rightFlag)
			{
				Serial.print("		  right 2	   ");
				rightFastMotor();
			}
			break;
		case 0b111111111110:
			Serial.print("			 LEFTY 2	  ");
			leftFlag++;
			leftFastMotor();
			break;
		case 0b111111111100:
		case 0b111111111001:
			Serial.println("		  LEFTY 1		");
			leftFlag++;
			leftMotor();
			break;
		case 0b111111110011:	
		case 0b111111100111:
		case 0b111111001111:
		case 0b111110011111:
			Serial.println("gogo");
			leftFlag=0;
			rightFlag=0;
			goMotor();
			break;

		case 0b111100111111:
		case 0b111001111111:
			Serial.println("		  righty 1		 ");
			rightFlag++;
			rightMotor();
			break;
		case 0b110011111111:
		case 0b100111111111:
			Serial.println("		  righty 2		 ");
			rightFlag++;
			rightFastMotor();
			break;	  

		case 0b001111111111:
		case 0b011111111111:
			Serial.println("		  righty 2		 ");
			rightFlag++;
			rightFastMotor();
			break;

		default:
		Serial.println("??");

		if(leftLinsensor<rightLinsensor) //(leftFlag > rightFlag)
		{
			Serial.println("		  lefty 1		");
			leftMotor();
		}
		else if(leftLinsensor>rightLinsensor) //(leftFlag < rightFlag)
		{
			Serial.println("		  righty 1		 ");
			rightMotor();
		}
		else 
		{
			Serial.println("gogo");
			goMotor();
		}
		break;
	}
}

void goMotor(void)
{
	analogWrite(LEFT_MOTOR,100);
	analogWrite(RIGHT_MOTOR,100);
}
void leftMotor(void)
{
	analogWrite(LEFT_MOTOR,50);
	analogWrite(RIGHT_MOTOR,100);
	// delay(2000);
}
void leftFastMotor(void)
{
	analogWrite(LEFT_MOTOR,30);
	analogWrite(RIGHT_MOTOR,100);
}
void rightMotor(void)
{
	analogWrite(LEFT_MOTOR,100);
	analogWrite(RIGHT_MOTOR,50);
}
void rightFastMotor(void)
{
	analogWrite(LEFT_MOTOR,100);
	analogWrite(RIGHT_MOTOR,30);
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
static void ckr_LR_motor_description_set(void)
{
	digitalWrite(LMOTOR_SET_DRV,HIGH);
	digitalWrite(LMOTOR_SET_BREAK,HIGH);
	digitalWrite(LMOTOR_SET_DIR,HIGH);
	
	digitalWrite(RMOTOR_SET_DRV,HIGH);
	digitalWrite(RMOTOR_SET_BREAK,HIGH);
	digitalWrite(RMOTOR_SET_DIR,HIGH);
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
static void ckr_L_motor_table_set(int mot_speed)
{
	//if(mot_speed != agv_cb.drv_info.pre_speed_left)
	{
		//agv_cb.drv_info.mot_table_change_flag = TRUE;
		agv_cb.drv_info.pre_speed_left = mot_speed;
		analogWrite(LEFT_MOTOR, mot_speed);
		Serial.print("LEFT MOT");
		Serial.println(mot_speed);
	}
}
static void ckr_R_motor_table_set(int mot_speed)
{
	//if(mot_speed != agv_cb.drv_info.pre_speed_right)
	{
		//agv_cb.drv_info.mot_table_change_flag = TRUE;
		agv_cb.drv_info.pre_speed_right = mot_speed;
		analogWrite(RIGHT_MOTOR, mot_speed);
		Serial.print("RIGHT MOT");
		Serial.println(mot_speed);

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
	//agv_cb.rfid_tag = 205;
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
#ifdef NEVER
	char i = 0;

	// ckr_jhshin_220701[Input Number 22 ~ 37][Board Input Port Number - using Line Sensor]
	for(i=22; i<38; i++) {
		pinMode(i, INPUT); 
	} 
#endif /* DEBUG_MODE */
	pinMode(PHOTOSEN_TOP, INPUT);
	pinMode(PHOTOSEN_BOTTOM, INPUT);
	pinMode(PIN_BUTTON, INPUT);
	
	pinMode(PIN_MOTOR_ENABLE, OUTPUT);
	pinMode(PIN_MOTOR_DIR, OUTPUT);
	pinMode(PIN_MOTOR_PWM, OUTPUT);

#if 0 //def TEST
	digitalWrite(PIN_MOTOR_ENABLE, HIGH);
	digitalWrite(PIN_MOTOR_DIR, HIGH);
	digitalWrite(PIN_MOTOR_PWM, HIGH);
#endif /* DEBUG_MODE */

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
	agv_cb.motor_go_btn_flag = FALSE;
	ckr_system_log_init();	// System Log Init
	ckr_rfid_init();		// RFID Tag Init
	ckr_photosensor_init();	// PhotoSensor Init
	ckr_display_init();		// LCD Init - Serial IF Boadrate init
	ckr_motor_des_init();	// ckr_jhshin_220704[motor_description]
	//ckr_eeprom_init();	// ckr_jhshin_220704[EEPROM Init]

	agv_cb.drv_info.turn_direction = 0; // ckr_jhshin_220704[IDLE function receive RFID_TAG_PINDN_GO..]
	agv_cb.drv_info.pre_speed_right = 1;
	agv_cb.drv_info.pre_speed_left = 1;
	attachInterrupt(0, stop_btn_callback, CHANGE);	// STOP BTN Callback
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
static int ckr_eeprom_read(void)
{
	agv_cb.top_rfid_number = _EEPROM_RD_TOP_RFID;
	agv_cb.lefty_tag_number = _EEPROM_RD_LEFTY;
	agv_cb.righty_tag_number = _EEPROM_RD_RIGHTY;
	
	delay(100);

	if((agv_cb.top_rfid_number == NULL) || (agv_cb.lefty_tag_number == NULL) || (agv_cb.righty_tag_number == NULL))
	{
		agv_cb.top_rfid_number = RFID_AGV_TOP_INIT_NUMBER;
		agv_cb.lefty_tag_number = RFID_TAG_LEFTY;
		agv_cb.righty_tag_number = RFID_TAG_RIGHTY;
		//return R_NG;
	}
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
static void ckr_check_eeprom_changed(void)
{
	int result = R_NG;
	
	//Serial.println(lcd_data.uiTranslateData[LCD_ADDR_LAST]);  //khkim3[20220708][test]
	if(lcd_data.uiTranslateData[LCD_ADDR_LAST])
	{
		agv_cb.top_rfid_number = lcd_data.uiTranslateData[LCD_ADDR_AGV_TOP];
		agv_cb.lefty_tag_number = lcd_data.uiTranslateData[LCD_ADDR_LEFTY];
		agv_cb.righty_tag_number = lcd_data.uiTranslateData[LCD_ADDR_RIGHTY];

		Serial.println(agv_cb.top_rfid_number);  //khkim3[20220708][test]
		Serial.println(agv_cb.lefty_tag_number); //khkim3[20220708][test]
		Serial.println(agv_cb.righty_tag_number); //khkim3[20220708][test]

		Serial.println(EEPROM.read(EEPROM_ADDR_RFIDTOP));
		Serial.println(EEPROM.read(EEPROM_ADDR_LEFTY));
		Serial.println(EEPROM.read(EEPROM_ADDR_RIGHTY));
		
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
	// BOTTOM LINE SENSOR PIN SETTING
	pinMode(LINE_SENSOR1, INPUT);
	pinMode(LINE_SENSOR2, INPUT);
	pinMode(LINE_SENSOR3, INPUT);
	pinMode(LINE_SENSOR4, INPUT);
	pinMode(LINE_SENSOR5, INPUT);
	pinMode(LINE_SENSOR6, INPUT);
	pinMode(LINE_SENSOR7, INPUT);
	pinMode(LINE_SENSOR8, INPUT);
	pinMode(LINE_SENSOR9, INPUT);
	pinMode(LINE_SENSOR10, INPUT);
	pinMode(LINE_SENSOR11, INPUT);
	pinMode(LINE_SENSOR12, INPUT);

	// MOTOR PIN SETTING
	pinMode(LEFT_MOTOR, OUTPUT);
	pinMode(LMOTOR_SET_DRV, OUTPUT);
	pinMode(LMOTOR_SET_BREAK, OUTPUT);
	pinMode(LMOTOR_SET_DIR, OUTPUT); 

	digitalWrite(LMOTOR_SET_DRV,HIGH);
	digitalWrite(LMOTOR_SET_BREAK,HIGH);
	digitalWrite(LMOTOR_SET_DIR,LOW);

	pinMode(RIGHT_MOTOR, OUTPUT);
	pinMode(RMOTOR_SET_DRV, OUTPUT);
	pinMode(RMOTOR_SET_BREAK, OUTPUT);
	pinMode(RMOTOR_SET_DIR, OUTPUT); 

	digitalWrite(RMOTOR_SET_DRV,HIGH);
	digitalWrite(RMOTOR_SET_BREAK,HIGH);
	digitalWrite(RMOTOR_SET_DIR,HIGH);
	
}

//*********************************************************************************************
// Function : ckr_rfid_request_checker
//
// Date : 
//
// Author : jhshin 
//
// Description : RFID값을 체크하지만, 현재의 상태에 따라서 동작이 달라진다... IDLE일때는 상태 변화만.. AGV_MOVE일댄 모터 구동이...
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
static int ckr_rfid_request_checker(int curr_function)
{
	bool pin_sts = FALSE;
	int req_check = 0;
	
	if(curr_function == LCD_READY)	// ckr_jhshin_220713[Calling from IDLE function]
	{
		Serial.print("ckr_rfid_request_checker : IDLE :");
		Serial.println(agv_cb.rfid_tag);
		switch (agv_cb.rfid_tag)
		{ 
			case RFID_TAG_STOP:	// ckr_jhshin_220713[just wait in IDLE function] 
				agv_cb.drv_info.move_start_by_rfid_tag = FALSE;
				req_check = AGV_REQ_NONE;
				break;
				
			case RFID_TAG_PINDN_GO:
				pin_sts = ckr_pin_btn_check(RFID_TAG_PINDN_GO);
				if(pin_sts == FALSE)
				{
					lcd_display_status_set_value(E100);	// PIN ERROR DISPLAY
					req_check = AGV_REQ_NONE;
				}
				else
				{
					req_check = AGV_REQ_MOVE;		// ckr_jhshin_220713[PIN DOWN & GO] 	
				}
				break;

				
			case RFID_TAG_PINUP_GO:
				pin_sts = ckr_pin_btn_check(RFID_TAG_PINUP_GO);
				if(pin_sts == FALSE)
				{
					lcd_display_status_set_value(E100);	// PIN ERROR DISPLAY
					agv_cb.request.currReq = AGV_REQ_NONE;
				}
				else
				{
					req_check = AGV_REQ_MOVE;		// ckr_jhshin_220713[PIN UP & GO] 
				}
				break;

				
			case RFID_TAG_SPEEDUP:
				agv_cb.drv_info.speed_mode = HIGH_SPEED;
				req_check = AGV_REQ_NONE;
				break;
				
			case RFID_TAG_SPEEDDN:
				agv_cb.drv_info.speed_mode = LOW_SPEED;
				req_check = AGV_REQ_NONE;
				break;		
				
			case RFID_TAG_L_TURN :
				agv_cb.drv_info.turn_direction = IDLE_FUNC_RCV_TURN_LEFT;
				req_check = AGV_REQ_MOVE;
				break;
				
			case RFID_TAG_R_TURN:
				agv_cb.drv_info.turn_direction = IDLE_FUNC_RCV_TURN_RIGHT;
				req_check = AGV_REQ_MOVE;
				break;			
				
			case RFID_TAG_START:
				agv_cb.drv_info.move_start_by_rfid_tag = TRUE;
				req_check = AGV_REQ_MOVE;
				break;
				
			default:
				Serial.println("Unkown Tag");
				Serial.print(agv_cb.rfid_tag);
				req_check = AGV_REQ_NONE;
				
				if(agv_cb.rfid_tag == agv_cb.lefty_tag_number)
				{
					agv_cb.drv_info.drv_direction_info = DIR_LEFTY;
					req_check = AGV_REQ_MOVE;
				}
				else if(agv_cb.rfid_tag == agv_cb.righty_tag_number)
				{
					agv_cb.drv_info.drv_direction_info = DIR_RIGHTY;
					req_check = AGV_REQ_MOVE;
				}
				break;
		}
	}
	else if(curr_function == LCD_MOVE)	// ckr_jhshin_220713[Calling from AGV_GO function] 
	{
		Serial.print("ckr_rfid_request_checker : AGV_GO :");
		Serial.println(agv_cb.rfid_tag);
		switch (agv_cb.rfid_tag)
		{ 
			case RFID_TAG_STOP:	// ckr_jhshin_220713[just wait in IDLE function] 
				agv_cb.drv_info.move_start_by_rfid_tag = FALSE;
				lcd_display_status_set_value(LCD_STOP);
				motor_stop_cmd();	// ckr_jhshin_220705[Motor STOP]
				req_check = AGV_REQ_NONE;
				break;

			case RFID_TAG_PINDN_GO:
				motor_stop_cmd();	// ckr_jhshin_220705[Motor STOP]
				delay(1000);
				pin_sts = ckr_pin_btn_check(RFID_TAG_PINDN_GO);
				if(pin_sts == FALSE)
				{
					lcd_display_status_set_value(E100);	// PIN ERROR DISPLAY
					req_check = AGV_REQ_NONE;
				}
				else
				{
					req_check = AGV_REQ_MOVE;		// ckr_jhshin_220713[PIN DOWN & GO] 	
				}
				break;
				
			case RFID_TAG_PINUP_GO:
				motor_stop_cmd();	// ckr_jhshin_220705[Motor STOP]
				delay(1000);
				pin_sts = ckr_pin_btn_check(RFID_TAG_PINUP_GO);
				if(pin_sts == FALSE)
				{
					lcd_display_status_set_value(E100);	// PIN ERROR DISPLAY
					agv_cb.request.currReq = AGV_REQ_NONE;
				}
				else
				{
					req_check = AGV_REQ_MOVE;		// ckr_jhshin_220713[PIN UP & GO] 
				}
				break;

			case RFID_TAG_SPEEDUP:
				agv_cb.drv_info.speed_mode = HIGH_SPEED;
				req_check = AGV_REQ_MOVE;
				break;

			case RFID_TAG_SPEEDDN:
				agv_cb.drv_info.speed_mode = LOW_SPEED;
				req_check = AGV_REQ_MOVE;
				break;		

			case RFID_TAG_L_TURN :
				motor_stop_cmd();	// ckr_jhshin_220705[Motor STOP]
				delay(1000);
				agv_cb.drv_info.turn_direction = IDLE_FUNC_RCV_TURN_LEFT;
				req_check = AGV_REQ_MOVE;
				break;
				
			case RFID_TAG_R_TURN:
				motor_stop_cmd();	// ckr_jhshin_220705[Motor STOP]
				delay(1000);
				agv_cb.drv_info.turn_direction = IDLE_FUNC_RCV_TURN_RIGHT;
				req_check = AGV_REQ_MOVE;
				break;			
				
			default:
				Serial.println("Unkown Tag");
				Serial.print(agv_cb.rfid_tag);
				req_check = AGV_REQ_NONE;
				
				if(agv_cb.rfid_tag == agv_cb.lefty_tag_number)
				{
					agv_cb.drv_info.drv_direction_info = DIR_LEFTY;
					req_check = AGV_REQ_MOVE;
				}
				else if(agv_cb.rfid_tag == agv_cb.righty_tag_number)
				{
					agv_cb.drv_info.drv_direction_info = DIR_RIGHTY;
					req_check = AGV_REQ_MOVE;
				}
				break;
		}
	}
	else	// ckr_jhshin_220713[unexpected case]
	{
		Serial.println("Unkown Case!!!");
	}
	
	rfid_check_flag = FALSE;
	return req_check;
}


//*********************************************************************************************
// Function : stop_btn_callback
//
// Date : 
//
// Author : jhshin 
//
// Description : If called this function, then go to Stop state
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
static void stop_btn_callback(void)
{
	Serial.println("stop_btn_callback");
	lcd_display_status_set_value(LCD_STOP);
	
	if(agv_cb.motor_go_btn_flag == 1)
	{
		agv_cb.motor_go_btn_flag = 0;
		agv_cb.drv_info.move_start_by_rfid_tag = FALSE;
	}
}

//*********************************************************************************************
// Function : 
//
// Date : 
#if 0
	if(digitalRead(PIN_BUTTON) == LOW)
	{
		agv_cb.pin_btn_pressed = TRUE;
		if(agv_cb.photo_sen_status == PIN_DOWN)
		{
			Serial.println("PIN BTN PRESSED - UP!!");
			ckr_pin_up_move();
		}
		else
		{
			Serial.println("PIN BTN PRESSED - DN!!");
			ckr_pin_down_move();
		}
	}
	else
	{
		agv_cb.pin_btn_pressed = FALSE;
	}
	return agv_cb.pin_btn_pressed;
#endif /* DEBUG_MODE */

// Author : jhshin 
//
// Description : TOP == 0 then PIN is UP... BOTTOM ==0 then, PIN is Down
//
// Input Parameters
// none
//
// Output Parameters
// none
//
//*********************************************************************************************
static bool ckr_pin_btn_check(int rfid_tag)
{
	bool result = FALSE;	

	Serial.println("ckr_pin_btn_check");

	Serial.println(_RD_PHOTO_TOP);
	Serial.println(_RD_PHOTO_BOT);

	switch (rfid_tag)
	{
		case 0:
			if((_RD_PHOTO_TOP == 1) && (_RD_PHOTO_BOT == 1))
			{
				Serial.println("ckr_pin_flotting..down");
				ckr_pin_down_move();
				result = ckr_pin_check_stop(PIN_HIGH_HIGH);
				if(result == FALSE)
				{
					result = ckr_pin_check_stop(PIN_HIGH_HIGH);
					if(result == FALSE)
					{
						lcd_display_status_set_value(E100);
					}
				}
			}
			else if((_RD_PHOTO_TOP == 0) && (_RD_PHOTO_BOT == 1))	// PIN is UP status
			{
				Serial.println("ckr_pin_down_move");
				ckr_pin_down_move();

				result = ckr_pin_check_stop(PIN_LOW_HIGH);
				if(result == FALSE)
				{
					result = ckr_pin_check_stop(PIN_LOW_HIGH);
					if(result == FALSE)
					{
						lcd_display_status_set_value(E100);
					}
				}
			}
			else	// PIN is DOWN status
			{
				Serial.println("ckr_pin_up_move");
				ckr_pin_up_move();
				
				result = ckr_pin_check_stop(PIN_HIGH_LOW);
				if(result == FALSE)
				{
					result = ckr_pin_check_stop(PIN_HIGH_LOW);
					if(result == FALSE)
					{
						lcd_display_status_set_value(E100);
					}
				}
			}
			break;
		case RFID_TAG_PINDN_GO:
			Serial.println("ckr_pin_down_move");
			ckr_pin_down_move();
			result = ckr_pin_check_stop(PIN_LOW_HIGH);
			if(result == FALSE)
			{
				result = ckr_pin_check_stop(PIN_LOW_HIGH);
				if(result == FALSE)
				{
					lcd_display_status_set_value(E100);
				}
			}
			break;
		case RFID_TAG_PINUP_GO:
			Serial.println("ELSE ckr_pin_down_move");
			ckr_pin_up_move();
			
			result = ckr_pin_check_stop(PIN_HIGH_LOW);
			if(result == FALSE)
			{
				result = ckr_pin_check_stop(PIN_HIGH_LOW);
				if(result == FALSE)
				{
					lcd_display_status_set_value(E100);
				}
			}			
			break;
		default:
			break;
	}

	return result;
}
//*********************************************************************************************
// Function : check_photo_sen_status
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
static int check_photo_sen_status(int rfid_input)
{
	int result = 0;
	if(rfid_input == RFID_TAG_PINUP_GO)
	{
		if(digitalRead(PHOTOSEN_BOTTOM) == HIGH)
		{
			agv_cb.photo_sen_status = PIN_DOWN;
			result = R_OK;
		}
		else
		{
			ckr_pin_down_move();
			delay(1000);
			if(digitalRead(PHOTOSEN_BOTTOM) == HIGH)
			{
				agv_cb.photo_sen_status = PIN_DOWN;
				result = R_OK;	
			}
			else
			{
				lcd_display_status_set_value(E100);
				result = R_NG;
			}
		}
	}
	else if(rfid_input == RFID_TAG_PINDN_GO)
	{
		if(digitalRead(PHOTOSEN_TOP) == HIGH)
		{
			agv_cb.photo_sen_status = PIN_UP;
			result = R_OK;
		}
		else
		{
			ckr_pin_up_move();
			delay(1000);
			if(digitalRead(PHOTOSEN_TOP) == HIGH)
			{
				agv_cb.photo_sen_status = PIN_UP;
				result = R_OK;
			}
			else
			{
				lcd_display_status_set_value(E100);
				result = R_NG;
			}
		}
	}
}
static int ckr_read_bottom_line(void)
{
	int line_read_val = 0;

	if(_READ_LINE_SEN1)		line_read_val |= 1;
	if(_READ_LINE_SEN2)		line_read_val |= (1 << 1);
	if(_READ_LINE_SEN3)		line_read_val |= (1 << 2);
	if(_READ_LINE_SEN4)		line_read_val |= (1 << 3);
	if(_READ_LINE_SEN5)		line_read_val |= (1 << 4);
	if(_READ_LINE_SEN6)		line_read_val |= (1 << 5);
	
	if(_READ_LINE_SEN7)		line_read_val |= (1 << 6);
	if(_READ_LINE_SEN8)		line_read_val |= (1 << 7);
	if(_READ_LINE_SEN9)		line_read_val |= (1 << 8);
	if(_READ_LINE_SEN10)		line_read_val |= (1 << 9);
	if(_READ_LINE_SEN11)		line_read_val |= (1 << 10);
	if(_READ_LINE_SEN12)		line_read_val |= (1 << 11);

#ifdef DEBUG_MODE
	Serial.print("RD SEN 1: ");
	Serial.println(_READ_LINE_SEN1);

	Serial.print("RD SEN 12: ");
	Serial.println(_READ_LINE_SEN12);
#endif /* DEBUG_MODE */

	return line_read_val;
}
