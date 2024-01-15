/*
 * File:   isr.c
 * Author: puran
 *
 * Created on 6 January, 2024, 6:16 PM
 */


#include <xc.h>
#include "main.h"

extern int min, return_time,log_screen;
void __interrupt() isr(void)
{
    static unsigned long int count = 0;
    
    if (TMR2IF == 1)
    {
        if (++count == 75000) //1250 for 1 sec
        {
            --min;
            count = 0;
        }
        if(count % 1250 == 0)
        {
            if(return_time > 0)
            {
                return_time--;
            }
            if(log_screen > 0)
            {
                log_screen--;
            }
        }
        
        TMR2IF = 0;
    }
}
