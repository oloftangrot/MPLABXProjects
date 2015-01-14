// ***************************************************************************
//  File Name    : servo_trial1.c
//  Version      : 1.0
//  Description  : Servo Motor Controller
//                 Using TIMER0 for Generating Servo PWM
//  Author       : RWB
//  Target       : 16F887
//  Compiler     : XC8
//  IDE          : Microchip MPLAB  X
//  Programmer   : PICKit3
//  Last Updated : 2014-1-28
// ***************************************************************************
#include <xc.h>

/*   PIC Configuration Bit:
**   INTIO     - Using Internal RC No Clock
**   WDTDIS    - Wacthdog Timer Disable
**   PWRTEN    - Power Up Timer Enable
**   MCLREN    - Master Clear Enable
**   UNPROTECT - Code Un-Protect
**   UNPROTECT - Data EEPROM Read Un-Protect
**   BORDIS    - Borwn Out Detect Disable
**   IESODIS   - Internal External Switch Over Mode Disable
**   FCMDIS    - Monitor Clock Fail Safe Disable
*/
//__CONFIG(INTIO & WDTDIS & PWRTEN & MCLREN & UNPROTECT \
//  & UNPROTECT & BORDIS & IESODIS & FCMDIS);

// Using Internal Clock of 8 Mhz
//#define FOSC 8000000L

// Servo definition and variables
#define MAX_VALUE 200
#define TIMER_VALUE 117 //177             // Initial Value for 0.1ms Interrupt
#define CCW_ROTATION (MAX_VALUE - 24)  // 2,4 ms
#define CW_ROTATION  (MAX_VALUE - 6)  // 0,6 ms
#define STOP_ROTATION MAX_VALUE
#define THRESHOLD_VALUE 50

volatile unsigned char pulse_max = 0;
volatile unsigned char pulse_top = 0;
volatile unsigned char top_value = 0;

static void interrupt isr(void)
{
  if( T0IF ) {	            // TIMER0 Interrupt Flag
    TMR0 = TIMER_VALUE;  // Reload the counter as soon as possible.
    RD7 = 1;             // Show interrupt entrance
    pulse_max++;            // Pulse Max Increment
    pulse_top++;            // Pulse Top Increment

    if (pulse_max >= MAX_VALUE) {  /* MAX_VALUE=200 turn off the pulse */
      RD6 = 0;                // Turn off RC2
      pulse_max = 0;
      pulse_top = 0;
    }
    /* top_value = MAX_VALUE - n, n=10: 10 x 0.1ms = 1.0ms, n=20: 20 x 0.1ms = 2.0ms */
    /* 2ms -> CCW Rotation, 1ms -> CW Rotation */
    if ( pulse_top == top_value ) {
      RD6 = 1;                // Turn On RC2
    }
    RD7 = 0;                // Show interrupt exit
    T0IF = 0;		    // Clear TIMER0 interrupt flag
  }
}

void main(void)
{
  unsigned char ldr_left;
  unsigned char ldr_right;
  int ldr_diff;

  OSCCON = 0x70;         // Select 8 Mhz internal clock

  /* Initial Port Used */
  TRISD = 0x00;        // Set RC0 and RC1 as input others as Output
  ANSEL = 0x30;        // Set PORT AN4 and AN5 as analog input
  ANSELH = 0x00;       // Set PORT AN8 to AN11 as Digital I/O
  PORTD = 0x00;        // Turn Off all PORTD

  /* Init Servo Pulse */
  pulse_max=0;
  pulse_top=0;
  top_value = MAX_VALUE; // top_value = MAX_VALUE: Servo Motor Stop

  /* Initial ADC */
  ADCON1=0b00110000;   // Select the FRC for 8 Mhz

  /* Init TIMER0: Period: Fosc/4 x Prescaler x TMR0
     0.0005 ms x 2 * 100 = 0.1 ms */

  OPTION_REG = 0b00000000; // 1:2 Prescaller
  TMR0 = TIMER_VALUE;          // Interupt every 0.1 ms
  T0IE = 1;		       // Enable interrupt on TMR0 overflow
  GIE = 1;		       // Global interrupt enable

  for(;;) {
#if 0
    /* Read the ADC here */
    ADCON0=0b00010001;       // select left justify result. ADC port channel AN4
    GO_DONE=1;	             // initiate conversion on the channel 4

    while(GO_DONE) continue;  // Wait for ldr_left conversion done
    ldr_left=ADRESH;         // Read 8 bits MSB, Ignore 2 bits LSB in ADRESL

    ADCON0=0b00010101;       // select left justify result. ADC port channel AN5
    GO_DONE=1;	             // initiate conversion on the channel 5

    while(GO_DONE) continue;  // Wait for ldr_right conversion done
    ldr_right = ADRESH;        // Read 8 bits MSB, Ignore 2 bits LSB in ADRESL

    /* Get the different */
    ldr_diff = ldr_left - ldr_right;

    if ((ldr_diff >= -THRESHOLD_VALUE) && (ldr_diff <= THRESHOLD_VALUE)) {
      top_value = MAX_VALUE;     // Stop the Servo Motor
    } else {
      if (ldr_diff > THRESHOLD_VALUE) {
        top_value = CCW_ROTATION;  // Counterclockwise Rotation
      } else {
        top_value = CW_ROTATION;   // Clockwise Rotation
      }
    }
#else
    top_value = CCW_ROTATION;  // Counterclockwise Rotation
    __delay_ms( 2000 );
    top_value = CW_ROTATION;   // Clockwise Rotation
    __delay_ms( 2000 );

#endif
  }
}


