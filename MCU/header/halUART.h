#ifndef _HALUART_H
#define _HALUART_H

#define RxBuffer UCA0RXBUF
#define TxBuffer UCA0TXBUF


//----------------------------------------------------------------------------------------------------
// Functions Declaration
//----------------------------------------------------------------------------------------------------
void uart_config();

//----------------------------------------------------------------------------------------------------
// External Functions and Variable Declaration
//----------------------------------------------------------------------------------------------------
extern volatile char p_tx[10], p_rx[10];
extern unsigned int index;
extern int write_to_flash;

extern int str2int(char volatile *str);

//----------------------------------------------------------------------------------------------------
// Interrupt Service Routines
//----------------------------------------------------------------------------------------------------
__interrupt void USCI0RX_ISR(void);
__interrupt void USCI0TX_ISR(void);
#endif
