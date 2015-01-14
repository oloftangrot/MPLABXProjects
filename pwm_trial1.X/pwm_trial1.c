// ***************************************************************************
//  File Name    : pwm_trial1.c
//  Version      : 1.0
//  Description  : Pulse Width Modulation (PWM)
//                 Single Output, Steering Mode
//  Author(s)    : TaO
//  Target(s)    : 16F887
//  Compiler     : MPLAB X XC8
//  IDE          : Microchip MPLAB X
//  Programmer   : PICKit3
//  Last Updated : 26 Jan 2014
// ***************************************************************************
#include <xc.h>


// Using Internal Clock of 8 MHz

//#define FOSC 8000000L

//#define XTAL_FREQ 12000000L

// Delay Function
//#define	_delay_us(x) { unsigned char us; \
//		       us = (x)/(12000000/FOSC)|1; \
		       while(--us != 0) continue; }

//void _delay_ms(unsigned int ms)
//{
//  unsigned char i;
//
//  if (ms == 0) return;
//  do {
//    i = 4;
//    do {
//      __delay_us(164);
//    } while(--i);
//  } while(--ms);
//}

void main(void)
{
  unsigned int ipwm;
  unsigned char state;

  OSCCON = 0b01000000;         // Select 1 MHz internal clock

  TRISC = 0x00;        // Set All on PORTC as Output
  PORTC = 0x00;        // Turn Off all PORTC
  TRISD = 0x00;        // Set All on PORTD as Output
  PORTD = 0x00;        // Turn Off all PORTC

  ANSEL = 0x00;        // Set PORT AN0 to AN7 digital I/O
  ANSELH = 0x00;       // Set PORT AN8 to AN11 as Digital I/O

  /* Init PWM for Single Output */
  CCP1CON = 0b00001100;  // Single PWM mode; P1A, P1C active-high; P1B, P1D active-high
  CCPR1L = 0;            // Start with zero Duty Cycle

  T2CON = 0b00000110;    // Postscale: 1:1, Timer2=On, Prescale = 1:16
  PR2 = 0xb0;            // Frequency: xxx kHz
  TMR2 = 0;              // Start with zero Counter

  PSTRCON = 0b00001111;  // Enable Pulse Steering on P1C (RD6)
  state = 0;             // Start with state 1

  for(;;) {
//      PORTC=0xff;
//      __delay_ms(10);
//      PORTC=0x0;
//      __delay_ms(10);
      const unsigned char th = 150;
    ipwm = 0;
    while (ipwm < th ) {
      CCPR1L = ipwm;
      ipwm++;
      __delay_ms( 5 );     // Delay 5 millisecond
    }

    ipwm = th;
    while (ipwm > 0) {
      CCPR1L = ipwm;
      ipwm--;
      __delay_ms( 5 );     // Delay 5 millisecond
    }

//     __delay_ms(100);    // Delay 100 millisecond
//
//    if (state == 0) {
//      state=1;
//      PSTRCON=0b00001000;  // Enable Pulse Steering on P1D (RC2)
//    } else if (state == 1) {
//      state=2;
//      PSTRCON=0b00001100;  // Enable Pulse Steering on P1C and P1D (RC3 and RC2)
//    } else {
//      state=0;
//      PSTRCON=0b00000100;  // Enable Pulse Steering on P1C (RC3)
//    }
  }
}

/* EOF: pwm_trial1.c */
