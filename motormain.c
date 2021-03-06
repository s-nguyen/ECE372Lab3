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
#include <math.h>
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
typedef enum stateTypeEnum{
    forward, idle1, backward, idle2, waitswitch
} stateType;

volatile stateType curState = idle2;
volatile stateType nextState;

volatile int done = 0;
volatile int adcVal = 0;

int main(void)
{
    char str[9]; //Used to print to LCD
    double voltage;
    
    initLCD();
    initPWM();
    initADC();
    initSW();
    //Enable ON the H-bridge
    ENABLEPIN = OUTPUT;
    ENABLE = 1;

    while(1){
        if(done){
            moveCursorLCD(0,0);
            voltage = (3.3/1023)*adcVal; //Scale 3.3 Volts with ADC
            sprintf(str, "%1.5fV", voltage); //print voltage read from ADC to a string
            printStringLCD(str);
            done = 0;
        }
        switch(curState){
            case backward:
                //Change direction here
                PIN5 = 0; //0 for NULL not used
                PIN6 = 0;
                PIN6 = 18; //Pin 6 is maped to OC1 control left wheel
                PIN4 = 19; //Pin 4 is mapped to OC2 Control Right wheel
                nextState = idle2;
                curState = waitswitch;
                break;
            case idle1:
                //Do nothing State
                LEFTWHEEL = 0;
                RIGHTWHEEL = 0;
                nextState = backward;
                break;
            case forward :
                //Change direct here
                PIN6 = 0;
                PIN4 = 0;
                PIN5 = 19;
                PIN7 = 18;
                nextState = idle1;
                curState = waitswitch;
                break;
            case idle2:
                //Do nothing State
                LEFTWHEEL = 0;
                RIGHTWHEEL = 0;
                nextState = forward;
                break;
            case waitswitch:
                if(adcVal > 511){
                    //Turned all the way CW
                    //Keep right wheel  on while
                    //lowering the speed of left wheel
                    RIGHTWHEEL = abs((1023-adcVal)*2);
                    LEFTWHEEL = 1023;

                }
                else if(adcVal < 500){
                    //Turned all the way CCW
                    //Keep left wheel on while
                    //Lowering the speed of right wheel
                    RIGHTWHEEL = 1023;
                    LEFTWHEEL = abs(adcVal*2);
                 }
                else{
                    LEFTWHEEL = 1023;   //Middle should have both on max
                    RIGHTWHEEL = 1023;
                }
                break;
            default:
                curState = forward;
                break;
        }
    }
    return 0;
}
// ******************************************************************************************* //
void _ISR _ADC1Interrupt(void){

    IFS0bits.AD1IF = 0;
    adcVal = ADC1BUF0; //Read from one buffer from ADC
    /*
    int i = 0;
    unsigned int *adcPtr;
    adcVal = 0;
    adcPtr = (unsigned int *) (&ADC1BUF0);
    for(i = 0; i < 16; i++){
        adcVal = adcVal + *adcPtr/16;
        adcPtr++;
    }
     */
    done = 1;
}
// ******************************************************************************************* //
void _ISR _CNInterrupt(void){
    IFS1bits.CNIF = 0;
    if(PORTBbits.RB5 == 1){
        curState = nextState;
    }

}