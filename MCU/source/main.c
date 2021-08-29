#include <msp430.h>
#include  "bspGPIO.h"
#include  "menuFunctions.h"    

//----------------------------------------------------------------------------------------------------
// Global Variable
//----------------------------------------------------------------------------------------------------
int state = 0;
volatile char p_rx[10];
volatile char p_tx[10];
int index;
int degree;
int start;
//----------------------------------------------------------------------------------------------------
// Functions Declaration
//----------------------------------------------------------------------------------------------------
void script_mode();

//----------------------------------------------------------------------------------------------------
// External Functions and Variable Declaration
//----------------------------------------------------------------------------------------------------
extern int receive_int();
extern void script_mode();
extern void servo_deg(int deg);
extern void servo_scan(int left, int right);

void main(void){
    InitGPIO();
    while(1){
		// FSM:
        switch(state){
            case 0:
                __bis_SR_register(LPM0_bits + GIE);
				state = receive_int();
                break;
            case 1:
                //Radar Detector System
                __bis_SR_register(LPM0_bits + GIE);
                start = receive_int();
                servo_scan(0,180);
                break;
            case 2:
				//Telemeter
				// Get degree from pc
                __bis_SR_register(LPM0_bits + GIE);
                receive_string(&degree);
                servo_deg(degree);
                _no_operation();
                break;
            case 3:
				//Script Mode
                script_mode();
                break;
            case 4:
                break;
            default:
                state = 0;
                break;
        }
    }
}
