/*
 * File:   ext_eeprom.c
 * Author: puran
 *
 * Created on 14 December, 2023, 1:01 PM
 */


#include <xc.h>
#include "ext_eeprom.h"
#include "i2c.h"

unsigned char read_ext_eeprom(unsigned char addr)
{
    unsigned char data;
    
    i2c_start();
    i2c_write(EEPROM_SLAVE_WRITE);
    i2c_write(addr);
    i2c_rep_start();
    i2c_write(EEPROM_SLAVE_READ);
    data = i2c_read(0);
    i2c_stop();
    
    return data;
}

void write_ext_eeprom(unsigned char addr, unsigned char data) // SEc_ADDR, data
{
    i2c_start();
    i2c_write(EEPROM_SLAVE_WRITE);
    i2c_write(addr);
    i2c_write(data);
    i2c_stop();
}

void write_str(char *str,unsigned char addr)
{
    while(*str)
    {
        write_ext_eeprom(addr,*str);
        str++;
        addr++;
    }
}