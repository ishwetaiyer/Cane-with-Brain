#include<p18F4550.h>
extern void _startup(void);	//startup function which is stored in external memory
#pragma code _RESET_INTERRUPT_VECTOR = 0X1000 //directive which is used to assign program place in the memory(reset)
void _reset(void)
{
	_asm
	goto _startup
	_endasm 
}
#pragma code
#pragma code _LOW_INTERRUPT_VECTOR = 0X1018//directive which is used to assign program place in the memory(low priority interrupt)
void low_ISR(void)
{
}
#pragma code
void ultrasonic_isr(void);
#pragma interrupt ultrasonic_isr
void ultrasonic_isr(void)
{	
	int a;
	T0CON = 0X00;	
	TMR0H = 0;
	TMR0L = 0;
	if(INTCONbits.RBIF == 1)                       //Makes sure that it is PORTB On-Change Interrupt
  	{
    	INTCONbits.RBIE = 0;                         //Disable On-Change Interrupt
    	if(PORTBbits.RB4 == 1)                      //If ECHO is HIGH
    	T0CONbits.TMR0ON = 1;                       			 //Start Timer
    	if(PORTBbits.RB4 == 0)                      //If ECHO is LOW
    	{
     		 T0CONbits.TMR0ON = 0;                     		 //Stop Timer
     		 a = (TMR0L|(TMR0H<<8))/58.82; //Calculate Distance
    	}
	}
}
#pragma code _HIGH_INTERRUPT_VECTOR = 0X1008//directive which is used to assign program place in the memory(high priority interrupt)
void high_ISR(void)
{
	_asm
	goto ultrasonic_isr
	_endasm
}
#pragma code
# define rs PORTEbits.RE0	//register select signal to RE0
# define rw PORTEbits.RE1	//read/write signal to RE1
# define en PORTEbits.RE2	//enable signal to RE2
//Function Prototypes
void LCD_command(unsigned char ); //Function to pass command to the LCD
void LCD_data(unsigned char ); //Function to write character to the LCD
void msdelay(unsigned int time);
void udelay(unsigned int time);
void main()
{		
	unsigned int i,a,val[10];
	unsigned char msg[] = "Distance :";
	unsigned char msg1[] = "Out of Range :";
	TRISBbits.TRISB0 = 0;	
	TRISBbits.TRISB4 = 1;	
	TRISD = 0x00;   //Making Port D as output port
	TRISE = 0x00;   //Making Port E as output port
	PORTBbits.RB1 = 1; 	//Vcc
	PORTBbits.RB2 = 0;	//Gnd
	INTCONbits.GIE = 1;		//to set the priority levels
	INTCONbits.RBIF = 0;	//Clear PORTB On-Change Interrupt Flag
	INTCONbits.RBIE = 1;	// //Enable PORTB On-Change Interrupt
	T0CON = 0X00;	
	ADCON1 = 0x0F;	//Making ports as digital
	LCD_command(0x38);            // initialization of 16X2 LCD in 8bit mode 
    LCD_command(0x01);           // clear LCD
    LCD_command(0x0C);          // cursor off
   	LCD_command(0x80);         // go to first line and 0th position	
   	for(i = 0;msg[i] != '\0'; i++)
	{
			LCD_data(msg[i]);		// send the data to be displayed
	}   
	while(1)
	{	
		TMR0H = 0;
		TMR0L = 0;
		PORTBbits.RB0 = 1;
		udelay(1);
		PORTBbits.RB0 = !(PORTBbits.RB0);
		a = a + 1;                  //Distance Calibration
		if(a >= 2 && a <= 400)          //Check whether the result is valid or not
   		{
	   		for(i = 0; i < 3; i++)
			{
				val[i] = a%0X0A;
				val[i] = val[i]+0X30;
				a = a/0X0A;
			}
			LCD_data(val[2]);
			LCD_data(val[1]);
			LCD_data(val[0]);
	    }
		else
		{
			for(i = 0; msg1[i] != '\0'; i++)
			{
				LCD_data(msg1[i]);		// send the data to be displayed
			} 
		}       		
	}	
}
void LCD_command(unsigned char cmd) //Function to pass command to the LCD
{
	PORTD = cmd; 				//Send data on LCD data bus
	rs = 0;  			//RS = 0 since command to LCD
	rw = 0; 		    	//RW = 0 since writing to LCD
	en = 1; 		    	//Generate High to low pulse on EN
	msdelay(50);
	en = 0;
  
}
//Function to display content on LCD
void LCD_data(unsigned char d)
{
	PORTD = d;			//Send data on LCD bus
	rs = 1;				//RS=1 since data
	rw = 0;				//RW=0
	en = 1; 		    	//Generate High to low pulse on EN
	msdelay(50);
	en = 0;
}
void msdelay(unsigned int time)
{
	unsigned int i,j;
	for(i = 0; i < time; i++)
	{
		for(j = 0; j < 710; j++);
	}
} 	
void udelay(unsigned int time)
{
	unsigned int i,j;
	for(i = 0; i < time; i++)
	{
		for(j = 0; j < 8; j++);
	}
} 	