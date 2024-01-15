/* 
 * File:   ext_eeprom.h
 * Author: puran
 *
 * Created on 14 December, 2023, 1:02 PM
 */

#ifndef EXT_EEPROM_H
#define	EXT_EEPROM_H
#define EEPROM_SLAVE_WRITE             0b10100000 // 0xD0
#define EEPROM_SLAVE_READ              0b10100001 // 0xD1

unsigned char read_ext_eeprom(unsigned char addr);
void write_ext_eeprom(unsigned char addr, unsigned char data);
void write_str(char *str,unsigned char addr);
#endif	/* EXT_EEPROM_H */

