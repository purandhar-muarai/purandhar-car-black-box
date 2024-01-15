/* 
 * File:   main.h
 * Author: puran
 *
 * Created on 14 December, 2023, 1:52 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#include <string.h>
#include "ds1307.h"
#include "digital_keypad.h"
#include "clcd.h"
#include "adc.h"
#include "i2c.h"
#include "ext_eeprom.h"
#include "timers.h"
#include "uart.h"
#include <stdint.h>
#include <stdlib.h>

#define DASH_BRD_SCN_FLAG    0x01
#define LOGIN_SCN_FLAG       0x02
#define RESET_PASS           0x01
#define RESET_NOTHING        0x00
#define _XTAL_FREQ           20000000
#define RETURN_BACK          0x03
#define LOGIN_SUCCESS        0x01
#define LOGIN_MENU_FLAG      0x04

#define VIEW_LOG_FLAG        0x07
#define CLEAR_LOG_FLAG       0x08
#define DOWNLOAD_LOG_FLAG    0x09
#define RESET_TIME_FLAG      0x10
#define CHANGE_PASS_FLAG     0x11

#define C_SUCESS             0x01
#define C_FAILED             0x02
#define C_CONT               0x03

#define SHORT_PRESS4          0x08
#define LONG_PRESS4           0x09
#define SHORT_PRESS5          0x10
#define LONG_PRESS5           0x11

#define LOG_ADDR             0x12
#define PASS_ADDR            0x00


void display_dashboard(char *event, unsigned char speed);
unsigned char login(unsigned char key, unsigned char reset);
unsigned char login_menu(unsigned char reset,unsigned char key);
void log_event(char *event, unsigned char speed);
void clear_screen(void);
void view_log(unsigned char key);
void display_log(char *data);
unsigned char change_pass(unsigned char key,unsigned char reset);
void download_log(void);
void clear_log(void);
void change_time(unsigned char key);
uint8_t decimalToBCD(uint8_t val);

#endif	/* MAIN_H */

