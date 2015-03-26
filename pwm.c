#include "p24FJ64GA002.h"
#include "pwm.h"

void initPWM(){

    T3CONbits.TCKPS = 1;
    TMR3 = 0;
    PR3 = 1023; //1 milisecond
    

    OC1CONbits.OCTSEL = 1; //Using Timer 3
    OC1CONbits.OCM = 6; //PWM mode on OCx,
  //  RPOR1bits.RP2R = 18; //Pin 6 is maped to OC1

    OC1R = 0;
    OC1RS = 1007;

    T3CONbits.TON = 1;
}