/*
 * pulse.c
 *
 * @author Evan Blough
 */
#include "lcd.h"
#include "String.h"
#include "Timer.h"
#include "stdlib.h"
#include <stdbool.h>
#include "C:/ti/TivaWare_C_Series-2.1.2.111/driverlib/interrupt.h"

volatile  int rising_time = 0, falling_time, previous_time = 0;
volatile int update_flag = 0;

void TIMER3B_Handler(void)
{

    if (rising_time != previous_time) // checks if rising time is not equal to previous time
    {
        rising_time = TIMER3_TBR_R; //sets rising time to register
        previous_time = rising_time;
    }
    else
    {
        falling_time = TIMER3_TBR_R; //sets falling time to register
        previous_time = falling_time;
    }

    //lcd_printf("Handler Executed Reg Value %d", TIMER3_TBR_R);

    /*if(update_flag == 0){
     rising_time = TIMER3_TBR_R;
     update_flag = 1;
     }

     else if(update_flag == 1){
     falling_time = TIMER3_TBR_R;
     update_flag = 2;
     }*/

    TIMER3_ICR_R |= 0x400; // clear ICR flag

}

void init(void)
{
    SYSCTL_RCGCGPIO_R |= 2;
    GPIO_PORTB_AFSEL_R |= 8;
    GPIO_PORTB_DEN_R |= 8;
    GPIO_PORTB_DIR_R &= ~0x08;
    GPIO_PORTB_AMSEL_R |= 8;
    GPIO_PORTB_ADCCTL_R = 0;
    GPIO_PORTB_PCTL_R |= 0x7000;
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R3;

    TIMER3_CTL_R &= ~0x100;
    TIMER3_CFG_R |= 0x00000004; // 16 bit timer Config
    TIMER3_TBMR_R |= 0x7; //Capture mode, Edge-time mode, Count up
    TIMER3_CTL_R |= 0xD00;
    TIMER3_TBPR_R = 0xFF; //slides
    TIMER3_TBILR_R = 0xFFFF; //slides
    TIMER3_ICR_R |= 0x400; // CBECINT = bit 10, 0b100_0000_0000
    TIMER3_IMR_R |= 0x400; // CBEIM = bit 10, 0b100_0000_0000 Enable Capture Interrupt
    TIMER3_CTL_R |= 0x100;
    //TIMER3_MIS_R
    // NVIC_PRI9_R |= 0x20;
    NVIC_EN1_R |= 0x10; // ENB I

    IntRegister(INT_TIMER3B, TIMER3B_Handler); // ENB I
    IntMasterEnable(); // ENB I
}

void send_pulse(void)
{
    GPIO_PORTB_AFSEL_R &= ~0x08;    // Alternate Function Disable
    GPIO_PORTB_DIR_R |= 0x08;    // set PB3 as output
    GPIO_PORTB_DATA_R |= 0x08;    // set PB3 to high
    timer_waitMicros(5);
    // waits 5 MiicroSeconds
    GPIO_PORTB_DATA_R &= 0xF7; // set PB3 to low
    GPIO_PORTB_DIR_R &= 0xF7; // set PB3 as input
    GPIO_PORTB_AFSEL_R |= 0x08; // Alternate Function Enable
}

int main(void)
{
    int pulseDiff = 0;
    int distance = 0;
    int time = 0;
    ;
    unsigned overflow = 0; // increments everytime a overflow is caught
    init();
    lcd_init(); //init the lcd
    distance = 5;
    while (1)
    {
        send_pulse(); // sends that pulse
        timer_waitMillis(250); //waits for Jevay to see this

        pulseDiff = falling_time - rising_time;
        if (pulseDiff < 0)
        {
            pulseDiff = pulseDiff + 4294967296; // Difference will be negative add size of timer full timer to get real time distance
            overflow++;
        }
        time = pulseDiff / 16000; // time in ms
        //distance = time * 34 / 2; //distance in cm
        distance = pulseDiff * 0.010625 / 10; // distance in mm


        lcd_printf("difference %d \nDistance: %d cm\nOverFlows: %d\nTime: %d", pulseDiff, distance, overflow, time);

    }
}
