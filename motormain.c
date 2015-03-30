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
    char str[9];
    double voltage;
    
    initLCD();
    initPWM();
    initADC();
    //Enable ON the H-bridge
    TRISBbits.TRISB11 = 0;
    LATBbits.LATB11 = 1;


    //init a button?
    TRISBbits.TRISB5 = 1;
    IFS1bits.CNIF = 0;
    IEC1bits.CNIE = 1;
    CNEN2bits.CN27IE = 1;

    while(1){
        if(done){
            moveCursorLCD(0,0);
            voltage = (3.3/1023)*adcVal;
            sprintf(str, "%1.5fV", voltage);
            printStringLCD(str);
            done = 0;
        }
        switch(curState){
            case backward:
                //Change direction here
                RPOR0bits.RP1R = 0;
                RPOR1bits.RP3R = 0;
                RPOR1bits.RP2R = 18; //Pin 6 is maped to OC1 control left wheel
                RPOR0bits.RP0R = 19; //Pin 4 is mapped to OC2 Control Right wheel
                nextState = idle2;
                curState = waitswitch;
                break;
            case idle1:
                //Do nothing State
                OC1RS = 0;
                OC2RS = 0;
                nextState = backward;
                break;
            case forward :
                //Change direct here
                RPOR1bits.RP2R = 0;
                RPOR0bits.RP0R = 0;
                RPOR0bits.RP1R = 19;
                RPOR1bits.RP3R = 18;
                nextState = idle1;
                curState = waitswitch;
                break;
            case idle2:
                //Do nothing State
                OC1RS = 0;
                OC2RS = 0;
                nextState = forward;
                break;
            case waitswitch:
                if(adcVal > 511){
                    //Turned all the way CW
                    //Keep right wheel  on while
                    //lowering the speed of left wheel
                    OC2RS = abs((1023-adcVal)*2);
                    OC1RS = 1023;

                }
                else if(adcVal < 500){
                    //Turned all the way CCW
                    //Keep left wheel on while
                    //Lowering the speed of right wheel
                    OC2RS = 1023;
                    OC1RS = abs(adcVal*2);
                 }
                else{
                    OC1RS = 1023;
                    OC2RS = 1023;
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
    adcVal = ADC1BUF0;
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
    if(PORTBbits.RB5 == 0){
      
    }
    else if(PORTBbits.RB5 == 1){
        
        curState = nextState;
    }

}