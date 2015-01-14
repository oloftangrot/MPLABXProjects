/* 
 * File:   timer0_isr_trial1.c
 * Author: olof
 *
 * Created on January 12, 2014, 8:29 PM
 */

#include <stdio.h>
#include <stdlib.h>

// HARDWARE:
//   - PIC16F84A with 4Mhz Crystal Oscillator
//   - LED --> RB0 pin
#include <pic16f887.h>
#include <xc.h>

/* Configuration fuses */
//__CONFIG(FOSC_XT & WDTE_OFF & PWRTE_OFF & CP_OFF);

#pragma config FOSC = HS        // Oscillator Selection
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)


//------------------------------------------------------------------------------
/* Macros */

#define LED1  RB0
#define LED2  RB1

#define mToggleLED1()  (LED1^=1)
#define mToggleLED2()  (LED2^=1)

#define TMR0_PRESET   133          //Precalculated; TMR0 interrupt freq = 1KHz

//------------------------------------------------------------------------------
/* Global Variables */

volatile unsigned int skptmr1;     // A skip timer variable
volatile unsigned int skptmr2;     // another skip timer variable

//------------------------------------------------------------------------------
/* Function Prototype */

void TMR0Init(void);

//------------------------------------------------------------------------------

void main()
{
  TRISB &= ~0x03;                  //LED1 and LED2
  LED1 = 0;
  LED2 = 0;

  TMR0Init();
                                   //Initial time delays
  skptmr1 = 1000;                  //1000-ms delay
  skptmr2 = 250;                   //250-ms delay

  while(1)
  {
    if(skptmr1==0)                 //If 1000ms has passed,
    {
      skptmr1 = 1000;              // restart the delay and
      mToggleLED1();               // toggle the LED1.
    }

    if(skptmr2==0)                 //If 250ms has passed,
    {
      skptmr2 = 250;               // restart the delay and
      mToggleLED2();               // toggle the LED2.
    }
  }
}

//------------------------------------------------------------------------------

/* Interrupt Service Routine */
void interrupt isr(void)
{
  if(T0IE && T0IF)
  {
    TMR0 = TMR0_PRESET;       //Load TMR0 with a preset value so that interrupt
                              // frequency is 1000x per second
    if(skptmr1)               //If skip timer variable is not yet 0,
      --skptmr1;              // decrement it
    if(skptmr2)               //Same.
      --skptmr2;
    T0IF = 0;                 //Clear TMR0 interrupt flag.
  }
  return;
}

//------------------------------------------------------------------------------

/* Initialize TMR0 in timer mode. Interrupt frequency = 1000 Hz */
void TMR0Init(void)
{
  T0CS = 0;               //TMR0 in timer mode.
  PSA = 0;                //Prescaler is assigned to TMR0.
  PS2 = 0;                //Prescaler = 8
  PS1 = 1;
  PS0 = 0;
  TMR0 = TMR0_PRESET;

  T0IF = 0;               //Clear TMR0 interrupt flag bit.
  T0IE = 1;               //Enable TMR0 as an interrupt source.
  GIE = 1;                //Enable interrupt operation.
  return;
}
