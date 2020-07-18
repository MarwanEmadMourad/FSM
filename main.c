#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
#define GoN 0
#define WaitN 1
#define GoE 2
#define WaitE 3


void PORTE_PORTB_init(void){

    *((volatile uint32_t*)0x400FE608) |= 0x00000012 ;  // initializing clock to port B & E
    *((volatile uint32_t*)0x40005400) |= 0x0000003F ;  // initializing 6 leds (PB0-PB5) as o/p
    *((volatile uint32_t*)0x40024400) &= ~0x03   ;     // initializing PE0 and PE1 as i/p
    *((volatile uint32_t*)0x4000551C) |= 0x0000003F ;  // activating digital enable on PB0-PB5
    *((volatile uint32_t*)0x4002451C) |= 0x03 ;        // activating digital enable on PE0,PE1
    GPIO_PORTE_AMSEL_R &= ~0x03;                       // 3) disable analog function on PE1-0
    GPIO_PORTE_PCTL_R &= ~0x000000FF;                  // 4) enable regular GPIO
    GPIO_PORTE_AFSEL_R &= ~0x03;                       // 6) regular function on PE1-0
    GPIO_PORTB_AMSEL_R &= ~0x3F;                       // 3) disable analog function on PB5-0
    GPIO_PORTB_PCTL_R &= ~0x00FFFFFF;                  // 4) enable regular GPIO
    GPIO_PORTB_AFSEL_R &= ~0x3F;                       // 6) regular function on PB5-0
}

void delay(float N){

    // N is the time value in seconds

    uint32_t time = 1600000 ;        //number of bus cycles that is equivalent to 1sec obtained from Systick timer
    uint32_t x = N*time ;            //value of the total delay time multiplied by the bus cycles number
    while (x)                        //delay loop
    {x-- ;}
}

typedef struct State{

    //here we are defining a new data type (Stype) contains the following members

        unsigned long out ;      //State output
        unsigned long wait ;     //time to stay in each state
        unsigned long next[4];   //array of the next states

}Stype;


int main(void)
{

    PORTE_PORTB_init();   //initiallizing Port B & E

    Stype FSM[4] = {

      {0x21,3,{GoN,WaitN,GoN,WaitN}} ,

      {0x22,1,{GoE,GoE,GoE,GoE}} ,

      {0x0C,3,{GoE,GoE,WaitE,WaitE}} ,

      {0x14,1,{GoN,GoN,GoN,GoN}}

    } ; //initiallizing array of 4 states that we have (GoN,WaitN,GoE,WaitE)

    unsigned long Current_state,input ;

    Current_state = GoN ;             //Asumming that the initial state is Go North

    while (1){


        GPIO_PORTB_DATA_R = FSM[Current_state].out ;     //since the current state is GoN the output is GoN , input haven't been taken yet

        delay(FSM[Current_state].wait) ;                 //stay in the state for a given delay

        input = GPIO_PORTE_DATA_R & 0x03 ;               //take the input

        Current_state = FSM[Current_state].next[input] ; //update your current state based on the taken input
    }

}
