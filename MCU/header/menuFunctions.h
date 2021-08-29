#ifndef _menuFunctions_H_
#define _menuFunctions_H_

#define RxBuffer UCA0RXBUF
#define TxBuffer UCA0TXBUF

#define FLASH_INFO_SEG_B_START     (char*)0xE080
#define FLASH_INFO_SEG_B_END       (char*)0xE0BF

#define FLASH_INFO_SEG_C_START     (char*)0xE040
#define FLASH_INFO_SEG_C_END       (char*)0xE07F

#define FLASH_INFO_SEG_D_START     (char*)0xE000
#define FLASH_INFO_SEG_D_END       (char*)0xE03F
//----------------------------------------------------------------------------------------------------
// Structs
//----------------------------------------------------------------------------------------------------
typedef struct Scripts{
	int written[3];
	char *pscript[3];
	int size[3];
	int lines[3];
	int num_script;
}Scripts;

//----------------------------------------------------------------------------------------------------
// Global Variable
//----------------------------------------------------------------------------------------------------
Scripts s = {{0}, {FLASH_INFO_SEG_B_START, FLASH_INFO_SEG_C_START, FLASH_INFO_SEG_D_START}, {0}, {0} , 1};
int delay = 50;
int write_to_flash = 0;
int offset = 0;

//----------------------------------------------------------------------------------------------------
// Functions Declaration
//----------------------------------------------------------------------------------------------------
int str2int(volatile char *str);
int int2str(char *str, int num);
int receive_int();

void receive_string(int *data);
void send_ack(int data);
void send_ss_data(int deg, int distance);

void write_seg (char* flash_ptr, int offset);
char read_char(char address);
int read_mem(int offset);
void blink_rgb(int delay, int times);
void rlc_leds(int delay, int times);
void rrc_leds(int delay, int times);
void servo_deg(int deg);
void servo_scan(int left, int right);

//----------------------------------------------------------------------------------------------------
// External Functions and Variable Declaration
//----------------------------------------------------------------------------------------------------
extern int state;
extern volatile char p_tx[10];
extern volatile char p_rx[10];
extern int index;

extern void StopTimers();
extern void Timer0_A_delay_ms(int mili_sec);
extern void Timer1_A_delay_ms(int mili_sec);
extern void PWM_Servo_config(int deg);
extern int SS_Trig_config();
extern void SS_Echo_config();

void script_mode(){
    int script_size_counter;
    int opcode, x, p, l, r, num_byte;
    int script_lines_counter;
//----------------------------------------------------------------------------------------------------
// Stage 1 - Get auxiliary variables to write script
//----------------------------------------------------------------------------------------------------
    // Get number of script
	__bis_SR_register(LPM0_bits + GIE);
	s.num_script = receive_int();

	if(!s.written[s.num_script - 1]){
		// Get script size
		receive_string(&s.size[s.num_script - 1]);
		 // Get number of script's lines
		receive_string(&s.lines[s.num_script - 1]);

//----------------------------------------------------------------------------------------------------
// Stage 2 -  Writing script to flash
//----------------------------------------------------------------------------------------------------
		write_to_flash = 1;
		script_size_counter = s.size[s.num_script - 1];
		while(script_size_counter--){
			__bis_SR_register(LPM0_bits + GIE);
			write_seg(s.pscript[s.num_script - 1], offset++);   // Write value to flash
			send_ack(1);
		}
		s.written[s.num_script - 1] = 1;                        // Mark script already written
		write_to_flash = 0;
	}

//----------------------------------------------------------------------------------------------------
// Stage 3 - Executing Script
//----------------------------------------------------------------------------------------------------
	offset = 0;
	opcode = 0;
	script_lines_counter = s.lines[s.num_script - 1];
	while(script_lines_counter > 0){
		// Get Opcode
		opcode = read_mem(2);
		num_byte = 0;
		switch(opcode){
			case 1:
				while(read_mem(2) != 0x00)num_byte += 1;
				offset -= num_byte * 2;
				x = read_mem(num_byte * 2);
				blink_rgb(delay, x);
				break;
			case 2:
				while(read_mem(2) != 0x00)num_byte += 1;
				offset -= num_byte * 2;
				x = read_mem(num_byte * 2);
				rlc_leds(delay, x);
				break;
			case 3:
				while(read_mem(2) != 0x00)num_byte += 1;
				offset -= num_byte * 2;
				x = read_mem(num_byte * 2);
				rrc_leds(delay, x);
				break;
			case 4:
				while(read_mem(2) != 0x00)num_byte += 1;
				offset -= num_byte * 2;
				delay = read_mem(num_byte * 2);
				break;
			case 5:
				RGB_CLR;
				Leds_CLR;
				break;
			case 6:
				p = read_mem(2);
				servo_deg(p);
				// Show the degree and distance (dynamically) onto PC screen
				break;
			case 7:
				l = read_mem(2);
				r = read_mem(2);
				servo_scan(l, r);
				// Show the degree and distance (dynamically) onto PC screen
				break;
			case 8:
				state = 0;
				break;
			default:
				//opcode = 8;
				break;
		}
		if(script_lines_counter)offset++;      // If its not the last script line advance the '\n' char in the script
		script_lines_counter--;
	}
	offset = 0;

//----------------------------------------------------------------------------------------------------
// Stage 4 - Send Acknowledge after finishing executing the script
//----------------------------------------------------------------------------------------------------
	IE2 &= ~UCA0RXIE;                          // Disable USCI_A0 RX interrupt
	send_ack(500);
}

int receive_int(){
    index = 0;
    return *p_rx - '0';
}

void receive_string(int *data){
	while(1){
		__bis_SR_register(LPM0_bits + GIE); 
		if(p_rx[index - 1] == '\0'){
			*data = str2int(p_rx);
			index = 0;
			break;
		}
	}
}

void send_ack(int data){
    int len_data = int2str(p_tx,data);
    p_tx[len_data + 1] = '\r';
    IE2 &= ~UCA0RXIE;                         // Disable USCI_A0 RX interrupt
    IE2 |= UCA0TXIE;                                                // enable USCI_A0 TX interrupt
    __bis_SR_register(LPM0_bits + GIE);
}

void send_ss_data(int deg, int distance){
	int len_deg = int2str(p_tx,deg);
	int len_dist =int2str(p_tx + len_deg + 1,distance);
	p_tx[len_deg + 1 + len_dist] = '\r';
    IE2 &= ~UCA0RXIE;                         // Disable USCI_A0 RX interrupt
    IE2 |= UCA0TXIE;                          // enable USCI_A0 TX interrupt
    __bis_SR_register(LPM0_bits + GIE);
}

void write_seg (char* flash_ptr, int offset){
    FCTL3 = FWKEY;                            // Clear Lock bit
    FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation
    flash_ptr[offset] = *p_rx;                // Write value to flash
    while((FCTL3 & WAIT) != WAIT);            // Wait for write to complete
    FCTL1 = FWKEY;                            // Clear WRT bit
    FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}

char read_char(char addr){
    char *segment;
    segment = s.pscript[s.num_script - 1] + offset;
    return segment[addr];
}

int read_mem(int num_bytes){
    int data = 0, i;
    char ch;
    for(i = 0; i < num_bytes; i++){
        ch = read_char(i);
        if(ch == 0)
            return 0;
        else if(ch > '9')
            data += (ch - 0x37) << 4 * (num_bytes - 1 - i);
        else
            data += (ch - 0x30) << 4 * (num_bytes - 1 - i);
    }
    offset += num_bytes;
    return data;
}

void blink_rgb(int delay, int times){
	unsigned char RGB;
    while(times){
        if(RGB == 0x1C)
            P2OUT &= ~0x18;
        else{
            P2OUT += 0x04;
            RGB = P2OUT & 0x1C;
        }
        Timer1_A_delay_ms(delay);
        times--;
    }
}

void rlc_leds(int delay, int times){
	int i;
	int bits[8] = {BIT0, BIT3, BIT4, BIT5, BIT7, BIT0, BIT6, BIT7};
	while(times){
		for(i = 0; times && i < 8; i++, times--){
			if(i < 5){
				P1OUT |= bits[i];
				Timer1_A_delay_ms(delay);
				P1OUT &= ~bits[i];
			}
			else{
				P2OUT |= bits[i];
				Timer1_A_delay_ms(delay);
				P2OUT &= ~bits[i];
			}
		}
	}
}

void rrc_leds(int delay, int times){
	int i;
	int bits[8] = {BIT7, BIT6, BIT0, BIT7, BIT5, BIT4, BIT3, BIT0};
	while(times){
		for(i = 0; times && i < 8; i++, times--){
			if(i < 3){
				P2OUT |= bits[i];
				Timer1_A_delay_ms(delay);
				P2OUT &= ~bits[i];
			}
			else{
				P1OUT |= bits[i];
				Timer1_A_delay_ms(delay);
				P1OUT &= ~bits[i];
			}
		}
	}
}

void servo_deg(int deg){
	servo_scan(deg, deg);
}

void servo_scan(int left, int right){
    int distance, i;
    int deg_inc = 9;
	int deg = left;
	int servo_deg_flag = (left == right) && (state == 3);

	// Rotate to 0 degree
	PWM_Servo_config(deg);
	Timer0_A_delay_ms(50);
	StopTimers();
	p_rx[0] = state + '0';

	// Delay before scan
	for(i = 0; i < 5; i++)Timer0_A_delay_ms(50);
    StopTimers();

	while(state != 0 && !(state == 3 && deg >= right + deg_inc)){
	    __bis_SR_register(GIE);
	    state = receive_int();
	    __bic_SR_register(GIE);
	    distance = SS_Trig_config();
		send_ss_data(deg, distance);

		if(servo_deg_flag)break;// state == 3 servo_deg();

		if(state != 2){
		    // Increase degree
            if(state == 1)
                deg = (deg + deg_inc) % (right + deg_inc);
            else if(state == 3)
                deg = (deg + deg_inc) % (right + 2 * deg_inc);
		}
        PWM_Servo_config(deg);
		Timer0_A_delay_ms(50);
		StopTimers();
	}
    for(i = 0; i < 5; i++)Timer0_A_delay_ms(50);
}

int str2int(volatile char *str){
     int res = 0;
     unsigned int i;
     for (i = 0; str[i] != '\0'; ++i) {
         if (str[i]> '9' || str[i]<'0')
             return -1;
         res = res * 10 + str[i] - '0';
 }
 return res;
}

int int2str(char *str, int num){
    int strSize = 0,j;
    long temp = num, len = 0;
    while(len == 0 || temp){
        len++;
        temp /= 10;
    }
    for(j = len - 1; j >= 0; j--){
        str[j] = (num % 10) + '0';
        num /= 10;
    }
    strSize += len;
    str[strSize] = '\n';
	return strSize;
}

#endif
