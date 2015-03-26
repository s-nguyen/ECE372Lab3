// ******************************************************************************************* //
//
// File: oScope.c
// Date: 10-1-2014
// Authors: Garrett Vanhoy
// Description: This shows the use of a PWM signal as well as initializing the ADC
// ******************************************************************************************* //
#include "p24fj64ga002.h"
#include <stdio.h>
#include <stdlib.h>
#include "adc.h"
#include "pwm.h"
#include "timer.h"
#include "lcd.h"

_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & BKBUG_ON & COE_ON & ICS_PGx1 &
    FWDTEN_OFF & WINDIS_OFF & FWPSA_PR128 & WDTPS_PS32768 )
_CONFIG2( IESO_OFF & SOSCSEL_SOSC & WUTSEL_LEG & FNOSC_PRIPLL & FCKSM_CSDCMD & OSCIOFNC_OFF &
    IOL1WAY_OFF & I2C1SEL_PRI & POSCMOD_XT )
// ******************************************************************************************* //
// ******************************************************************************************* //

volatile int val = 0;
volatile int done = 0;
volatile int adcVal = 0;

int main(void)
{
    char str[9];
    
    initLCD();
    initPWM();
    initADC();
    TRISBbits.TRISB11 = 0;
    LATBbits.LATB11 = 1;
    
    while(1)
    {
        
        if(done){
            clearLCD();
            sprintf(str, "%d", adcVal);
            printStringLCD(str);
            OC1RS = adcVal;
            done = 0;
        }
         RPOR1bits.RP3R = 18;
        
       
    }
    return 0;
}
// ******************************************************************************************* //
void _ISR _ADC1Interrupt(void){
    IFS0bits.AD1IF = 0;
    int i = 0;
    unsigned int *adcPtr;
    adcVal = 0;
    adcPtr = (unsigned int *) (&ADC1BUF0);
    for(i = 0; i < 16; i++){
        adcVal = adcVal + *adcPtr/16;
        adcPtr++;
    }
    done = 1;
}
// ******************************************************************************************* //
