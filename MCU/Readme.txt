Main.c	
	int receive_int();				מקבל intger מה-PC
	void servo_deg(int deg)			מודד מרחק בזווית הנבחרת
	void servo_scan(int left,int right) 		מבצע סריקה בין זווית שמאלית לימנית
	Script_mode()				כותב את הסקריפט לזיכרון פלאש ומריץ את הסקריפט
menuFunction.h
	int str2int(volatile char *str)			ממיר מחרוזת לintger
	int int2str(char *str,int num)			ממיר integer למחרוזת
	int receive_int()				מקבל intger מה-PC
	void receive_string(int *data)		מקבל מחרוזת מה-PC
	void send_ack(int data)			שולח ביט אישור לPC
	void send_ss_data(int deg,int distance) 	שולח את המרחק והזווית מהחיישן לPC
	void write_seg (char* flash_ptr,int offset)	כותב char לזיכרון הפלאש
	char read_char(char address)		קורא char מזיכרון הפלאש
	int read_mem(int offset)			קורא מספר char רצופים מזיכרון הפלאש
	void blink_rgb(int delay,int times)		מדליק את נורת ה-RGB בצבעים שונים בקצב של הדיילי
	void rlc_leds(int delay,int times)		מדליק את הלדים ומבצע הזזה שמאלה בקצב של הדיילי
	void rrc_leds(int delay,int times)		מדליק את הלדים ומבצע הזזה ימניה בקצה של הדיילי
	void servo_deg(int deg)			מודד מרחק בזווית הנבחרת
	void servo_scan(int left,int right)		מבצע סריקה בין זווית שמאלית לימנית
bspGPIO.c	
	void InitGPIO(void)			מקנפג את ההפורטים של הבקר בתחילת הריצה
halTimers.c
	void StopTimers()				עוצר את כלל השעונים
	void Timer0_A_delay_ms(int ten_mili_sec)	מפעיל דיילי עם שעון TA0
	void Timer1_A_delay_ms(int ten_mili_sec)	מפעיל דיילי עם שעון TA1
	void SS_Echo_config()			מודד את המרחק מהחיישן ומחזיר בס"מ
	void PWM_Servo_config(int deg)		מזיז את מנוע ע"פ הזווית הרצויה
	int SS_Trig_config()				יוצר פולס אתחול לתחילת פעולת החיישן
	__interrupt void Timer0_A0(void)		פסיקה של TA0.0 לצורך השהייה
	__interrupt void Timer1_A0(void)		פסיקה של TA0.0 לצורך השהייה
	__interrupt void Timer1_A1(void)		פסיקה לצורך input capture של שעון TA1.1
halUart.c
	void uart_config()				אתחול הרגיסטרים של מודול ה-UART
	__interrupt void USCI0RX_ISR(void)		פסיקת הקבלה RX של מודול ה-UART
	__interrupt void USCI0TX_ISR(void)		פסיקת השליחה TX של מודול ה-UART
