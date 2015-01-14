// ***************************************************************************
//  File Name    : pwm_trial2.c
//  Version      : 1.0
//  Description  : Pulse Width Modulation (PWM)
//                 H-Bridge DC Motor Controller
//  Author(s)    : TÂO
//  Target(s)    : 16F887
//  Compiler     : XC8
//  IDE          : Microchip MPLAB IDE X
//  Programmer   : PICKit3
//  Last Updated : 03 Jan 2009
// ***************************************************************************
#include <xc.h>
#define TIMER_VALUE 25 // Initial Value for 10 ms Interrupt
void init_tick_timer( void );
unsigned long get_tick( void );
volatile unsigned long tick; // Declare tick counter volatile since it is updated in the ISR.
//
// Specification:
// R1A = direction
// AN0 = analog, in speed
// RD7-5+RC2 (P1D,P1C,P1B,P1A) = bridge output
// R1C = led out (sorta pulse width modulated)
//
void delay_ms(unsigned int ms)
{
  unsigned char i;

  if (ms == 0) return;
  do {
    i = 4;
    do {
      __delay_us( 245 );
    } while(--i);
  } while(--ms);
}

void main(void)
{
  unsigned int ipwm;
  unsigned char direction;
  unsigned int pp = 32;

  OSCCON=0x70;         // Select 8 Mhz internal clock

  TRISC = 0x00;        // Set All on PORTC as Output
  TRISD = 0x00;        // Set all om PORTD as output.
  TRISA = 0x03;        // Input for RA0 and RA1
  ANSEL = 0x01;        // Set PORT AN0 to analog input AN1 to AN7 digital I/O
  ANSELH = 0x00;       // Set PORT AN8 to AN11 as Digital I/O
  PORTC = 0x00;        // Turn Off all PORTC

  /* Init PWM for Full Bridge Output */
  CCP1CON = 0b01001100;  // Full Bridge Forward; P1A, P1C active-high; P1B, P1D active-high
  CCPR1L = 0;            // Start with zero Duty Cycle

  T2CON = 0b00000101;    // Postscale: 1:1, Timer2=On, Prescale = 1:4
  PR2 = 0x65;            // Frequency: 4.90 KHz
  TMR2 = 0;              // Start with zero Counter

  /* Init ADC */
  ADCON0 = 0b00000000;   // Select Left justify result. ADC port channel 0
  ADCON1 = 0b00110000;   // Select the FRC for 8 MHz
  ADON = 1;	        // turn on the ADC conversion module
  P1M1 = 0;
  P1M0 = 1;

  direction = 0;         // Start with Forward Direction
  ipwm = 0;

  init_tick_timer();

  for(;;) {
    if ( RA1 == 0 ) {           // Change the Motor Direction when pressed
     __delay_ms( 1 );
     if ( RA1 == 0 ) {          // Read again for simple debounce
       if ( direction == 0 ) {
         direction = 1;         // Reverse direction
         P1M1 = 1;
         P1M0 = 1;
       } else {
         direction = 0;         // Forward direction
         P1M1 = 0;
         P1M0 = 1;
       }
     }
    }

    GO_DONE = 1;	              // initiate conversion on the channel 0
    while( GO_DONE ) continue;  // Wait conversion done

//    ipwm = ADRESH;           // Get the highest 8 bit MSB result, ignore the 2 bit LSB
    ipwm = pp;
    CCPR1L = ipwm;           // Set the Duty Cycle base on the ADC result

    /* Blink the LED attached on RC0 */
    RC0 = 1;                 // Turn On
    delay_ms(ipwm);
    RC0 = 0;                 // Turn Off
    delay_ms(ipwm);
  }
}

static void interrupt isr(void)
{
  if( T0IF ) {	            // TIMER0 Interrupt Flag
    TMR0 = TIMER_VALUE;  // Reload the counter as soon as possible.
    RD2 = 1;             // Show interrupt entrance
    tick++;
    RD2 = 0;                // Show interrupt exit
    T0IF = 0;		    // Clear TIMER0 interrupt flag
  }
}

void init_tick_timer( void ) {
  /* Init TIMER0: Period: Fosc/4 x Prescaler x TMR0
     0.0005 ms x 2 * 100 = 0.1 ms */

  OPTION_REG = 0b00000101; // 1:32 Prescaller
  TMR0 = TIMER_VALUE;          // Interupt every 0.1 ms
  T0IE = 1;		       // Enable interrupt on TMR0 overflow
  GIE = 1;		       // Global interrupt enable
}

unsigned long get_tick( void )
{
    unsigned long tmp;
    if ( GIE ) { // Disable interrupt while reading the tick counter.
        GIE = 0;
        tmp = tick;
        GIE = 1;
    }
    else {
        tmp = tick;
    }
    return tmp;
}

/* EOF: pwm_trial2.c */
