/*
 * Author: PURANDHAR MURARISETTY
 * PROJECT : CAR BLACK BOX
 */


#include <xc.h>
#include "main.h"

#pragma config WDTE = OFF //Watchdog timer disabled

static void init_config(void) {
    //Write your initialization code here
    init_i2c(100000);
    init_ds1307();
    init_digital_keypad();
    init_clcd();
    init_adc();
    init_timer2();
    init_uart(9600);
    PEIE = 1;
    GIE = 1;
}
extern int log_screen;
void main(void) {
    init_config(); //Calling initializing function
    unsigned char control_flag = 0x01;
    char event[] = "ON";
    unsigned char key, speed = 0, gr = 0,reset_flag;
    char *gear[] = {"GN","GR","G1","G2","G3","G4","G5"};
    unsigned char menu_pos,key_l,flag = 0;
    int wait = 0;
    unsigned char flag_l;
    //log_event(event,speed);
    write_str("1111",PASS_ADDR);
    while (1) {
        //Write application code here
        
        speed = read_adc()/10;
        if(speed > 99)
        {
            speed = 99;
            log_event(event,speed);
        }
        key = read_digital_keypad(STATE);
        if(key == SW1)
        {
            //__delay_ms(500);
            strcpy(event,"C ");
            log_event(event,speed);
        }
        else if(key == SW2 && gr < 7)
        {
            strcpy(event,gear[gr]);
            gr++;
            log_event(event,speed);
        }
        else if(key == SW3 && gr > 0)
        {
            gr--;
            strcpy(event,gear[gr]);
            log_event(event,speed);
        }
        else if((key == SW4 || key == SW5) && control_flag == DASH_BRD_SCN_FLAG)
        {
            control_flag = LOGIN_SCN_FLAG;
            clear_screen();
            clcd_print("ENTER PASSWORD",LINE1(1));
            clcd_write(LINE2(5), INST_MODE);
            clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
            __delay_us(100);
            reset_flag = RESET_PASS;
            TMR2ON = 1;
        }
        else if(key == SW4 && (control_flag == LOGIN_MENU_FLAG || control_flag == VIEW_LOG_FLAG ||control_flag == RESET_TIME_FLAG))
        {
            do
            {
                key_l = read_digital_keypad(LEVEL);
                
                if(wait++ >= 30000)
                {
                    break;
                }
            }while(key_l == SW4);
            
            if(wait >= 30000)
            {
                key = LONG_PRESS4;
                wait = 0;
            }
            else if(wait > 3000 && wait < 30000 && key_l == ALL_RELEASED)
            {
                key = SHORT_PRESS4;
                wait = 0;
            }
        }
        else if(key == SW5 && (control_flag == LOGIN_MENU_FLAG ||control_flag == VIEW_LOG_FLAG))
        {
            do
            {
                key_l = read_digital_keypad(LEVEL);
                
                if(wait++ >= 30000)
                {
                    break;
                }
            }while(key_l == SW5);
            
            if(wait >= 30000)
            {
                key = LONG_PRESS5;
                wait = 0;
            }
            else if(wait > 3000 && wait < 30000 && key_l == ALL_RELEASED)
            {
                key = SHORT_PRESS5;
                wait = 0;
            }
        }
        if(key == LONG_PRESS4 && control_flag == LOGIN_MENU_FLAG)
        {
            switch (menu_pos)
            {
                case 0:
                    control_flag = VIEW_LOG_FLAG;
                    key = ALL_RELEASED;
                    break;
                case 1:
                    control_flag = CLEAR_LOG_FLAG;
                    clear_screen();
                    //clcd_print("sucess",LINE1(3));
                    break;
                case 2:
                    control_flag = DOWNLOAD_LOG_FLAG;
                    clear_screen();
                    //clcd_print("sucess",LINE1(3));
                    break;
                case 3:
                    control_flag = RESET_TIME_FLAG;
                    clear_screen();
                    key = ALL_RELEASED;
                   // clcd_print("sucess",LINE1(3));
                    break;
                case 4:
                    control_flag = CHANGE_PASS_FLAG;
                    clear_screen();
                    clcd_print("ENTER PASSWORD",LINE1(1));
                    clcd_write(LINE2(5), INST_MODE);
                    clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
                    __delay_us(100);
                    reset_flag = RESET_PASS;
                    break;
            }
        }

        switch(control_flag)
        {
            case DASH_BRD_SCN_FLAG:
                
                display_dashboard(event,speed);               
                if(flag == 0)
                {
                    log_event(event,speed);
                    flag = 1;
                }
                break;
            case LOGIN_SCN_FLAG:
                switch(login(key,reset_flag))
                {
                    case RETURN_BACK:
                        clear_screen();
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                        control_flag = DASH_BRD_SCN_FLAG;
                        TMR2ON = 0;
                        break;
                    case LOGIN_SUCCESS:
                        clear_screen();
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                        control_flag = LOGIN_MENU_FLAG;
                        reset_flag = RESET_PASS;
                        continue;
                }
                break;
            case LOGIN_MENU_FLAG:
                menu_pos = login_menu(reset_flag,key);                
                if(key == LONG_PRESS5 || log_screen == 0)
                {
                    clear_screen();
                    control_flag = DASH_BRD_SCN_FLAG;
                }
                break;
            case VIEW_LOG_FLAG :
                view_log(key);
                if(key == LONG_PRESS4)
                {
                    clear_screen();
                    control_flag = LOGIN_MENU_FLAG;
                }
                else if(key == LONG_PRESS5)
                {
                    clear_screen();
                    clcd_print("Logout",LINE1(3));
                    __delay_ms(2000);
                    control_flag = DASH_BRD_SCN_FLAG;
                }
                break;
            case DOWNLOAD_LOG_FLAG:
                download_log();
                __delay_ms(1000);
                clear_screen();
                control_flag = DASH_BRD_SCN_FLAG;
                break;
            case CHANGE_PASS_FLAG:  
                flag_l = change_pass(key,reset_flag);
                if(flag_l == C_FAILED || flag_l == C_SUCESS)
                {
                    clear_screen();
                    control_flag = DASH_BRD_SCN_FLAG;
                }
                break;
            case RESET_TIME_FLAG:
                change_time(key);
                if(key == LONG_PRESS4)
                {
                    clear_screen();
                    control_flag = DASH_BRD_SCN_FLAG;
                }
                break;
            case CLEAR_LOG_FLAG:
                clear_log();
                __delay_ms(1000);
                control_flag = DASH_BRD_SCN_FLAG;
                break;
        }
        reset_flag = RESET_NOTHING;
    }
}
