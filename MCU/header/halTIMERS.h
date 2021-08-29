#ifndef _HALTIMERS_H
#define _HALTIMERS_H

//----------------------------------------------------------------------------------------------------
// Global Variable
//----------------------------------------------------------------------------------------------------
volatile unsigned char First_Time;
volatile unsigned int REdge, FEdge;

//----------------------------------------------------------------------------------------------------
// Functions Declaration
//----------------------------------------------------------------------------------------------------
void StopTimers();
void Timer0_A_delay_ms(int ten_mili_sec);
void Timer1_A_delay_ms(int ten_mili_sec);
void SS_Echo_config();
void PWM_Servo_config(int deg);
int SS_Trig_config();
void Timer1_A1_config();

//----------------------------------------------------------------------------------------------------
// External Functions and Variable Declaration
//----------------------------------------------------------------------------------------------------
extern volatile char p_tx[10];
extern volatile char p_rx[10];
extern unsigned int index;

extern void int2str(char *str, unsigned int num);

//----------------------------------------------------------------------------------------------------
// Interrupt Service Routines
//----------------------------------------------------------------------------------------------------
__interrupt void Timer0_A0(void);
__interrupt void Timer0_A1(void);
__interrupt void Timer1_A0(void);
__interrupt void Timer1_A1(void);// not in use
__interrupt void Timer1_A2(void);// not in use
#endif
