/*
 * File:   car_black_box_def.c
 * Author: puran
 *
 * Created on 14 December, 2023, 2:00 PM
 */


#include <xc.h>
#include "main.h"


char time[7];
unsigned char clock_reg[3];
char log[11];
int log_no = -1;
int min,return_time,log_screen;
char *menu[] = {"View log","Clear log","Download log","Set time","Change passwd"};

static void get_time(void)
{
    clock_reg[0] = read_ds1307(HOUR_ADDR); // HH -> BCD 
    clock_reg[1] = read_ds1307(MIN_ADDR); // MM -> BCD 
    clock_reg[2] = read_ds1307(SEC_ADDR); // SS -> BCD 
}

void display_time(void)
{
    get_time();
   
    time[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
    time[1] = (clock_reg[0] & 0x0F) + '0';
    // MM 
    //time[2] = ':';
    time[2] = ((clock_reg[1] >> 4) & 0x07) + '0';
    time[3] = (clock_reg[1] & 0x0F) + '0';
    // SS
    //time[5] = ':';
    time[4] = ((clock_reg[2] >> 4) & 0x07) + '0';
    time[5] = (clock_reg[2] & 0x0F) + '0';
    time[6] = '\0';
    
    clcd_putch(time[0],LINE2(1));
    clcd_putch(time[1],LINE2(2));
    clcd_putch(':',LINE2(3));
    clcd_putch(time[2],LINE2(4));
    clcd_putch(time[3],LINE2(5));
    clcd_putch(':',LINE2(6));
    clcd_putch(time[4],LINE2(7));
    clcd_putch(time[5],LINE2(8));
    //clcd_print(time,LINE2(1));
}

void display_dashboard(char *event, unsigned char speed)
{
    clcd_print("TIME     E  SP",LINE1(2));
    clcd_print(event,LINE2(10));
    display_time();
    clcd_putch(speed/10 + '0',LINE2(14));
    clcd_putch(speed%10 + '0',LINE2(15));
}

void log_car_event(void)
{
    char addr;
    log_no++;
    if(log_no == 10)
    {
        log_no = 0;
    }
    addr = log_no * 11 + LOG_ADDR;
    write_str(log,addr);
    
}

void log_event(char *event, unsigned char speed)
{
    strcpy(log,time);
    strcat(log,event);
    log[8] = speed/10 + '0';
    log[9] = speed%10 + '0';
    log[10] = '\0';
    log_car_event();
}

void clear_screen(void)
{
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    __delay_us(500);
}


unsigned char login(unsigned char key,unsigned char reset)
{
    static char user_pass[5];
    static int i;
    static unsigned char attempt_left;
    
    if(reset == RESET_PASS)
    {
        key = ALL_RELEASED;
        attempt_left = 3;
        i = 0;
        return_time = 3;
    }
    
    if(return_time == 0)
    {
        return RETURN_BACK;
    }
    //sw4 = 1  sw5 = 0
    
    if(key == SW4 && i < 4)
    {
        clcd_putch('*',LINE2(5+i));
        //user_pass[i] = '1';
        write_ext_eeprom(i+5,'1');
        i++;
        return_time = 3;
    }
    else if(key == SW5 && i < 4)
    {
        clcd_putch('*',LINE2(5+i));
        //user_pass[i] = '0';
        write_ext_eeprom(i+5,'0');
        i++;
        return_time = 3;
    }
    if(i == 4)
    {
        char pass[5];
        for(char j = 0; j < 4; j++)
        {
            pass[j] = read_ext_eeprom(j);
            user_pass[j] = read_ext_eeprom(j+5);
        }
        pass[4] = '\0';
        user_pass[4] = '\0';
        
        
        if(strcmp(pass,user_pass) == 0)
        {
            clear_screen();
            clcd_print("LOGIN SUCCESS",LINE1(2));
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
            __delay_us(100);
            __delay_ms(2000);
            return LOGIN_SUCCESS;
            //menu
        }
        else
        {
            attempt_left--;
            if(attempt_left)
            {
                clear_screen();
                clcd_print("WORNG PASSWORD",LINE1(1));
                clcd_putch(attempt_left+'0',LINE2(0));
                clcd_print("attempt's left", LINE2(2));
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                __delay_ms(2000);
            }
            else
            {
                //int min;
                min = 15;/// sec nee to dor 15 min 900 sec
                clear_screen();
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                clcd_print("YOU ARE BLOCKED",LINE1(1));
                clcd_print("min left",LINE2(5));
                while(min)
                {
                    clcd_putch(min/10+'0',LINE2(2));
                    clcd_putch(min%10+'0',LINE2(3));
                }
                attempt_left = 3;
            }
            clear_screen();
            return_time = 3;
            clcd_print("ENTER PASSWORD",LINE1(1));
            clcd_write(LINE2(5), INST_MODE);
            clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
            __delay_us(100);
            i = 0;
        }
    }
}

unsigned char login_menu(unsigned char reset,unsigned char key)
{
    static unsigned char menu_pos;
    if(reset == RESET_PASS)
    {
        menu_pos = 0;
        log_screen = 5;
    }
    if(key == SHORT_PRESS4 && menu_pos > 0)
    {
        clear_screen();
        menu_pos--;
        log_screen = 5;
    }
    else if(key == SHORT_PRESS5 && menu_pos <= 3)
    {
        clear_screen();
        menu_pos++;
        log_screen = 5;
    }
    if(menu_pos < 3)
    {
        clcd_putch('*',LINE1(0));  
        clcd_print(menu[menu_pos],LINE1(2));
        clcd_print(menu[menu_pos+1],LINE2(2));
    }
    else if(menu_pos == 3)
    {
        clcd_putch('*',LINE1(0));
        clcd_print(menu[menu_pos],LINE1(2));
        clcd_print(menu[menu_pos+1],LINE2(2));
    }
    else
    {
        clcd_putch(' ',LINE1(0));
        clcd_putch('*',LINE2(0));
        clcd_print(menu[menu_pos-1],LINE1(2));
        clcd_print(menu[menu_pos],LINE2(2));
    }

    return menu_pos;
}


void view_log(unsigned char key)
{    
    if(log_no == -1)
    {
        clear_screen();
        clcd_print("NO LOGS RECORDED",LINE1(0));
        __delay_ms(500);
    }
    else
    {
    //clear_screen();
    clcd_print("#  TIME     E SP",LINE1(0));
    unsigned char addr = LOG_ADDR;
    static int num = 0;
    char data[15];
    if(key == SHORT_PRESS4 && (num < log_no || num == 9))
    {
        if(num < 9)
        {
            num++;
        }
        else
        {
            num = 0;
        }
    }
    else if(key == SHORT_PRESS5 && num > 0)
    {
        --num;
    }
    addr += num * 11;
    clcd_putch(num+1+'0',LINE2(0));
    for(char i = 0; i < 15; i++)
    {
        if(i == 2 || i == 5)
        {
            data[i] = ':';
            continue;
        }
        if(i == 8 || i == 11)
        {
            data[i] = ' ';
            continue;
        }
        
        data[i] = read_ext_eeprom(addr);
         
        addr++;
    }
    data[14] = '\0';
    clcd_print(data,LINE2(2));
   // __delay_ms(2000);
    }
    //while(1);
}

void download_log(void)
{
    clear_screen();
    clcd_print("Downloading log",LINE1(1));
    char data[15],addr = LOG_ADDR,i;
    int j = log_no,num = 1;
    
    while(j-- >= 0)
    {
        putchar(num + '0');
        putchar(' ');
        for (i = 0; i < 15; i++) 
        {
            if (i == 2 || i == 5) 
            {
                data[i] = ':';
                continue;
            }
            if (i == 8 || i == 11) 
            {
                data[i] = ' ';
                continue;
            }
            data[i] = read_ext_eeprom(addr);
            addr++;
        }
        data[14] = '\0';
        puts(data);
        puts("\n");
        ++num;
    }
    clear_screen();
    clcd_print("Download ",LINE1(2));
    clcd_print("completed",LINE2(2));
}

void clear_log(void)
{
    log_no = -1;
    clear_screen();
    clcd_print("log cleared",LINE1(2));
}

unsigned char change_pass(unsigned char key,unsigned char reset)
{
    static char t_pass[5],r_pass[5];
    static int i,j;
  
    if(reset == RESET_PASS)
    {
        key = ALL_RELEASED;
        i = 0;
        j = 0;
    }
    
    //sw4 = 1  sw5 = 0
    
    if(key == SW4 && i < 4)
    {
        clcd_putch('*',LINE2(5+i));
        t_pass[i] = '1';
        i++;
    }
    else if(key == SW5 && i < 4)
    {
        clcd_putch('*',LINE2(5+i));
        t_pass[i] = '0';
        i++;
    }
    if(i == 4)
    {
        i++;
        j = 0;
        clear_screen();
        clcd_print("REENTER PASSWORD",LINE1(0));
        clcd_write(LINE2(5), INST_MODE);
        clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
        __delay_us(100);
        key = ALL_RELEASED;
    }
    if(key == SW4 && j < 4)
    {
        clcd_putch('*',LINE2(5+j));
        r_pass[j] = '1';
        j++;
    }
    else if(key == SW5 && j < 4)
    {
        clcd_putch('*',LINE2(5+j));
        r_pass[j] = '0';
        j++;
    }
    if(j == 4)
    {
        t_pass[4] = '\0';
        r_pass[4] = '\0';
        if(strcmp(t_pass,r_pass) == 0)
        {
            clear_screen();           
            write_str(t_pass,PASS_ADDR);
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
            __delay_us(100);
            clcd_print("password changed", LINE1(0));
            clcd_print("successful",LINE2(1));
            __delay_ms(3000);
            return C_SUCESS;
        }
        else
        {
            clear_screen();
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
            __delay_us(100);
            clcd_print("passwords not",LINE1(0));
            clcd_print("match",LINE2(2));
            __delay_ms(2000);
            return C_FAILED;
        }
    }
    return C_CONT;
}

void change_time(unsigned char key)
{
    static unsigned char blink = 6;
    clcd_print("Time (HH:MM:SS)",LINE1(1));
    clcd_print("  ",LINE2(blink+1));
    
    if(key == SW5)
    {
        (blink == 6) ? blink = 0 : (blink += 3);
    }
    else if(key == SHORT_PRESS4)
    {
        if(blink == 0)
        {
            if(time[0] == '2' && time[1] == '3')
            {
                time[0] = '0';
                time[1] = '0';
            }
            else if(time[0] < '3')
            {
                if(time[1] == '9')
                {
                    time[1] = '0';
                    time[0]++;
                }
                else
                {
                    time[1]++;
                }
            }
        }
        else if(blink == 3)
        {
            if(time[3] == '9')
            {
                time[3] = '0';
                (time[2] == '5') ? (time[2] = '0') : (time[2]++);
            }
            else
            {
                time[3]++;
            }
        }
        else if(blink == 6)
        {
            if(time[5] == '9')
            {
                time[5] = '0';
                (time[4] == '5') ? (time[4] = '0') : (time[4]++);
            }
            else
            {
                time[5]++;
            }
        }
        
    }

        clcd_putch(time[0],LINE2(1));
        clcd_putch(time[1],LINE2(2));
        clcd_putch(':',LINE2(3));
        clcd_putch(time[2],LINE2(4));
        clcd_putch(time[3],LINE2(5));
        clcd_putch(':',LINE2(6));
        clcd_putch(time[4],LINE2(7));
        clcd_putch(time[5],LINE2(8));
        
        char hh[3] = {time[0],time[1],'\0'};
        char mm[3] = {time[2],time[3],'\0'};
        char ss[3] = {time[4],time[5],'\0'};
        
        write_ds1307(HOUR_ADDR,decimalToBCD(atoi(hh)));
        write_ds1307(MIN_ADDR,decimalToBCD(atoi(mm)));
        write_ds1307(SEC_ADDR,decimalToBCD(atoi(ss)));
}

uint8_t decimalToBCD(uint8_t val)
{
    return ((val / 10) << 4) + (val % 10);
}
