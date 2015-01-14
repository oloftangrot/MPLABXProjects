/* 
 * File:   timer0_trial1.c
 * Author: olof
 *
 * Created on January 12, 2014, 9:11 PM
 */

#include <stdio.h>
#include <stdlib.h>

#include <xc.h>
#include <pic16f887.h>
/*
 * 
 */
int Count = 0;

int main(int argc, char** argv) {
    TMR0 = 0; // TMRO initiation
    T0CS = 0;  // Choosing to work with internal CLK
    T0SE = 0; // Reacting on Low2High edge
    PSA = 0;
    PS0 = 1; PS1 = 1; PS2 = 1; // Prescaler value divides in 256
    while ( 1 ) {
        while( !T0IF ); // Loop until the timer overflows.
        T0IF = 0;
        Count++;
        if ( Count == 15 ) {
            Count = 0;
        }
    }
    return (EXIT_SUCCESS);
}

