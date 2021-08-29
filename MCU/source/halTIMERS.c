#include <msp430.h>
#include  "halTIMERS.h"    

#define period_pwm_servo 26214 		//25[ms]
#define duty_pwm_servo 300 //550    //0.6[ms]
#define period_ss_trig 60120 		//60[ms] 
#define duty_ss_trig 40		  		//20[us]
#define step 10//11 			    //(2.5-0.6[ms]/180)

void StopTimers(){
    TA0CTL &= ~ 0x18;
    TA1CTL &= ~ 0x18;
}

void Timer0_A_delay_ms(int ten_mili_sec){
  TA0CCTL0 = CCIE;                             // CCR0 interrupt enabled
  TA0CCR0 = ten_mili_sec * 1310;
  TA0CTL = TASSEL_2 + ID_3 + MC_1 + TACLR;   // SMCLK/8 = 131072[Hz], upmode
  __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ interrupt
}

void Timer1_A_delay_ms(int ten_mili_sec){
  TA1CCTL0 = CCIE;                             // CCR0 interrupt enabled
  TA1CCR0 = ten_mili_sec * 1310;
  TA1CTL = TASSEL_2 + ID_3 + MC_1 + TACLR;   // SMCLK/8 = 131072[Hz], upmode
  __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ interrupt
}


void PWM_Servo_config(int deg){// PWM Servo - Timer1_A2
	// Timer info: f = 40[Hz], T = 25[ms]
	TA1CCTL2 = OUTMOD_7;                      		// Reset/set
	TA1CCR2 = duty_pwm_servo + step * deg;          // PWM Duty Cycle
    TA1CCR0 = period_pwm_servo;                     // Clock period of TBCCR0
    TA1CTL = TASSEL_2 + TACLR + MC_1;        		  		// SMCLK, up mode;
}

void SS_Echo_config(){// SS-Echo - Timer1_A1
    // Configure the TA1CCR1 to do input capture
    // TA1CCR1 Capture mode; CCI1A; Both Rising and Falling Edge; interrupt enable
    TA1CCTL1 = CAP + CM_3 + CCIE + SCS + CCIS_0;
    TA1CTL = TASSEL_2 + MC_2;        // SMCLK, Cont Mode; start timer
}


int SS_Trig_config(){// SS-Trig - Timer0_A1
	// Timer info: f = 16.7[Hz], T = 60[ms], duty = 20[us]
    SS_Echo_config();
	TA0CCTL1 = OUTMOD_7;                     	   				    // Reset/set
	TA0CCR1 = duty_ss_trig;                  						// PWM Duty Cycle
    TA0CCR0 = period_ss_trig;                     				    // Clock period of TBCCR0
    TA0CTL = TASSEL_2 + TACLR;        		  						// SMCLK, up mode;
	First_Time = 0x01;
	TA0CTL |= MC_1;
	__bis_SR_register(LPM0_bits + GIE);   							// Enter LPM0
	StopTimers();
	return (int)(((double)(FEdge - REdge) / 1048576) * 17000);    //range[cm] for 25 deg celsios
}

//------------------------------------------------------------------
//--------------- Timer A0 Interrupt Service Rotine ----------------
//------------------------------------------------------------------
// Delay Interput
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0(void){
	TA0CCTL0 &= ~CCIE;                               // CCR0 interrupt enabled
	TA0CTL = TACLR;
    TA0CTL = MC_0 + TACLR;
    __bic_SR_register_on_exit(LPM0_bits + GIE);  	 // Exit LPM0 on return to main
}

// not in use
#pragma vector = TIMER0_A1_VECTOR
__interrupt void TIMER0_A1(void){
	
}

//------------------------------------------------------------------
//--------------- Timer A1 Interrupt Service Rotine ----------------
//------------------------------------------------------------------
// Delay Interput
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer1_A0(void){
    TA1CCTL0 &= ~CCIE;                               // CCR0 interrupt enabled
	TA1CTL = TACLR;
    TA1CTL = MC_0 + TACLR;
    __bic_SR_register_on_exit(LPM0_bits + GIE);  // Exit LPM0 on return to main
}

// SS - Echo - TA1CCR1 to do input capture
#pragma vector=TIMER1_A1_VECTOR
__interrupt void Timer1_A1(void){
	// Rising Edge was captured
	if(First_Time){
		REdge = TA1CCR1;
		First_Time = 0x0;
	}
	// Falling Edge was captured
	else{
		FEdge = TA1CCR1;
		__bic_SR_register_on_exit(LPM0_bits + GIE);  // Exit LPM0 on return to main
	}
	TA1CCTL1 &= ~CCIFG;
}

// not in use
#pragma vector=TIMER1_A2_VECTOR
__interrupt void Timer1_A2(void){
  ;
}



