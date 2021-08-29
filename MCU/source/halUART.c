#include <msp430.h>
#include  "halUART.h"    

void uart_config(){
	UCA0CTL1 |= UCSSEL_2;                    // CLK = SMCLK
	UCA0BR0 = 104;                           // 
	UCA0BR1 = 0x00;                          //
	UCA0MCTL = UCBRS0;                   	 //	
    UCA0CTL1 &= ~UCSWRST;                    // **Initialize USCI state machine**
    IE2 |= UCA0RXIE;                         // Enable USCI_A0 RX interrupt
}

//------------------------------------------------------------------
//-------- USCI A0/B0 Transmit Interrupt Service Rotine ------------
//------------------------------------------------------------------
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{ 	
    TxBuffer = p_tx[index++];
    while (!(IFG2&UCA0TXIFG));
    if(p_tx[index - 1] == '\r'){
        index = 0;
        IE2 &= ~UCA0TXIE;        			         // Disable USCI_A0 TX interrupt
        IE2 |= UCA0RXIE;                 			 // Enable USCI_A0 RX interrupt
        __bic_SR_register_on_exit(LPM0_bits + GIE);  // Exit LPM0 on return to main
	}
}

//------------------------------------------------------------------
//-------- USCI A0/B0 Receive Interrupt Service Routine ------------
//------------------------------------------------------------------
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
	while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
    if(write_to_flash)
        p_rx[0] = RxBuffer;
    else
        p_rx[index++] = RxBuffer;

	__bic_SR_register_on_exit(LPM0_bits + GIE);  // Exit LPM0 on return to main
}
