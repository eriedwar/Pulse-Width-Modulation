#include "msp.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "lcdLib_432.h"

int direction = 1; // 1 for forward, -1 for backward
int speed = 0; // 0 for stopped, 1 for slow speed, 2 for fast speed
#define TIMER_A_CCTLMC__UP   0x0010  // Timer A mode
#define TIMER_A_CLR         0x0004  // Timer A clear
#define PWM_PERIOD 60000  // PWM period in clock cycles
#define PWM_DUTY_CYCLE_MIN 2000  // Minimum duty cycle for servo
#define PWM_DUTY_CYCLE_MAX 7500  // Maximum duty cycle for servo
#define PWM_DUTY_CYCLE_STEP 50   // Duty cycle step size for servo
#define DELAY 2500000


void soundBuzzer(void);

void main(void)
 {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    //    // Configure GPIO
    P5->DIR &= ~(BIT0 | BIT1 | BIT2 | BIT4); // input pins
    P5->REN |= BIT0 | BIT1 | BIT2 | BIT4; // enable pull-up/down resistor
    P5->OUT &= ~(BIT0 | BIT1 | BIT2 | BIT4); // select pull-up resistor

    // Configure interrupts for input pins
    P5->IE |= (BIT0 | BIT1 | BIT2 | BIT4); // enable interrupts
    P5->IES &= ~(BIT0 | BIT1 | BIT2 | BIT4); // select falling edge
    P5->IFG &= ~(BIT0 | BIT1 | BIT2 | BIT4); // clear interrupt flag

    // Enable Port 5 interrupt on the NVIC
    NVIC->ISER[1] = 1 << ((PORT5_IRQn) & 31);

    WDT_A->CTL = WDT_A_CTL_PW |             // Stop WDT
                WDT_A_CTL_HOLD;

    // Configure P2.4 for Timer_A0 PWM output
    P6->DIR |= BIT5;
    P2->DIR |= BIT4;
    P2->SEL0 |= BIT4;
    P2->SEL1 &= ~BIT4;

    // Configure Timer_A0 for PWM mode
    TIMER_A0->CCR[0] = PWM_PERIOD;            // PWM period
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;// Reset/set output mode
    TIMER_A0->CCR[1] = PWM_DUTY_CYCLE_MIN - 1;
    TIMER_A0->CCR[1] = PWM_DUTY_CYCLE_MIN - 1;    // PWM initial duty cycle
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK |     // SMCLK clock source
                    TIMER_A_CTL_MC__UP |         // Up mode
                    TIMER_A_CTL_CLR;             // Clear timer counter

    TIMER_A0->CCR[1] = 0;
    lcdInit();
    lcdClear();

}
//funtion definition to sound buzzer
void soundBuzzer(){
    int note = 523;//play note C5
    int duration = 1000;//set to play at 1000ms or 1 sec
    const unsigned int period = 477;//set period of the tone to be played
    int cycles = (note * duration) / 1000;//sets note to play for every one second
    int i = 0;//set variable i for (for) loop
    for(; i < cycles; i++){
        P6->OUT ^= BIT5;//set to toggle on and off sound from piezo buzzer
        __delay_cycles(period);
    }
    P6->OUT &= ~BIT5;//turn off buzzer by setting the output pin to low state
}
//
/* Port5 ISR */
void PORT5_IRQHandler(void)
{
    if(P5 -> IFG & BIT0){
        printf("BIT0\n");
        soundBuzzer();
        lcdClear();
        lcdSetText("One Cycle Only", 0, 0); // display input 1 on the LCD
        TIMER_A0->CCR[1] = PWM_DUTY_CYCLE_MIN;
        __delay_cycles(DELAY);
        TIMER_A0->CCR[1] = PWM_DUTY_CYCLE_MAX;
        __delay_cycles(DELAY);
        TIMER_A0->CCR[1] = PWM_DUTY_CYCLE_MIN;
        lcdSetText("Done", 0, 1); // display input 1 on the LCD
//        P5->IFG &= ~BIT0; // clear interrupt flag
    }
    if(P5 -> IFG & BIT1){
        printf("BIT1\n");
        lcdClear();
        lcdSetText("Slow Cycle", 0, 0); // display input 1 on the LCD
        while(1){
            soundBuzzer();
            TIMER_A0->CCR[1] = PWM_DUTY_CYCLE_MIN;
            __delay_cycles(DELAY);
            TIMER_A0->CCR[1] = PWM_DUTY_CYCLE_MAX;
            __delay_cycles(DELAY);
            TIMER_A0->CCR[1] = PWM_DUTY_CYCLE_MIN;
//            P5->IFG &= ~BIT1; // clear interrupt flag
            if(P5->IFG & BIT4)
                break;
        }
    }
    if(P5 -> IFG & BIT2){
        printf("BIT2\n");
        lcdClear();
        lcdSetText("Fast Cycle", 0, 0); // display input 1 on the LCD
        while(1){
            soundBuzzer();
            TIMER_A0->CCR[1] = PWM_DUTY_CYCLE_MIN;
            __delay_cycles(DELAY/2);
            TIMER_A0->CCR[1] = PWM_DUTY_CYCLE_MAX;
            __delay_cycles(DELAY/2);
            TIMER_A0->CCR[1] = PWM_DUTY_CYCLE_MIN;
//            P5->IFG &= ~BIT2; // clear interrupt flag
            if(P5->IFG & BIT4)
                break;
            }
    }
    if(P5 -> IFG & BIT4){
        printf("BIT4\n");
        speed = 0;
        lcdSetText("CANCELED", 0, 1); // display input 1 on the LCD
    }
    P5->IFG &= ~(BIT0 | BIT1 | BIT2 | BIT4);
}
