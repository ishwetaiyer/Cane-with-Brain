#include <p18f4550.h>
#include "vector_relocate.h"

# define rs PORTEbits.RE0	//register select signal to RE0
# define rw PORTEbits.RE1	//read/write signal to RE1
# define en PORTEbits.RE2	//enable signal to RE2

//Function Prototypes
void init_LCD();
void LCD_command(unsigned char ); //Function to pass command to the LCD
void LCD_data(unsigned char ); //Function to write character to the LCD
void LCD_write_string(unsigned char *string);
void msdelay(unsigned int time);
void TXbyte(unsigned char data);
void TXstring(unsigned char *string);
unsigned char rx_data(void);

void main(void) {
    unsigned int timer_my, i, val[10], dist, b, flag;
    unsigned char str[]="hi";
	unsigned char string1[] = "GPGGA";
    unsigned char cmd_data[5], time_data[15], latitude[15], longitude[15];
    unsigned char rxdata[10];	//for reception
    //unsigned char s1[]={};
    //unsigned char s2[]={};
    //unsigned char s3[]={};
    //unsigned char s4[]={};
    TRISD = 0x00;
    TRISE = 0x00;
    TRISCbits.TRISC7 = 1;		// RECEPTION
	TRISCbits.TRISC6 = 0;		// TRANSMISSION
    init_LCD();
    ADCON1 = 0x0F;
    INTCON2bits.RBPU = 0;
    SPBRGH = 0X04;			//TO SET THE BAUD RATE AS 9600
	SPBRG = 0XE1;				//TO SET THE BAUD RATE AS 9600
	TXSTA = 0X24;				//CONFIGURATION OF TXSTA
	RCSTA = 0X90;				//CONFIGURATION OF RCSTA
	BAUDCON = 0X08;			//CONFIGURATION OF BAUDCON
    // TRISB2 = 1; // Push Button switch
    // TRISB3 = 1; // Power Button
	for(i = 0; str[i] != '\0'; i++)
         LCD_data(str[i]);
     if(rx_data() == '$')				//Start of GPS Sentence
	 {
		for (i = 0; i < 5; i++)
		{	
		  flag = 0;
		  cmd_data[i] = rx_data();	//Receive GPS sentence type
		  if(cmd_data[i] != string1[i])		//Verify GPS sentence with GPGGA
		  {
		    flag = 1;
		    break;
		  }
		}
		if(flag == 0)
		{
		   for(i = 0; i < 12; i++)				
			   time_data[i] = rx_data(); //Receive the Time 
		   time_data[12] = '\0';
		   for(i = 0; i < 12; i++)			
			   latitude[i]=rx_data(); //Receive the Latitude
		   latitude[12] = '\0';
		   for(i = 0; i < 12; i++)
			  	longitude[i] = rx_data(); //Receive the Longitude
		   longitude[12] = '\0';
		}
        LCD_command(0X80);
        for(i = 0; latitude[i] != '\0'; i++)
             LCD_data(latitude[i]);
        LCD_command(0XC0);
        for(i = 0; longitude[i] != '\0'; i++)
             LCD_data(longitude[i]);
        TXstring("\n\rAT\r");
        msdelay(1000);
        TXstring("\n\rAT+CMGF=1\r");
        msdelay(1000);
         TXstring("\n\rAT+CMGS=\"+917757938300\"\r");
        msdelay(1000);
        /*TXstring(latitude);
        msdelay(1000);
        TXstring(longitude);
        msdelay(1000);
        TXstring("\n\rHELP\r");
        LCD_command(0x80);
        msdelay(1000);*/
        TXstring("\n\rHELP\r");
        LCD_write_string("HELP");
        TXbyte(26); // ASCII value of ctrl+Z          
    }
       // From here for normal operation
       // When help button not pressed
     
 } 
void init_LCD()
{
    LCD_command(0x38);            // initialization of 16X2 LCD in 8bit mode 
   	LCD_command(0x01);           // clear LCD
   	LCD_command(0x0C);          // cursor off
   	LCD_command(0x80);         // go to first line and 0th position	
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
void LCD_write_string(unsigned char *string)
{
  while(*string)			//Check for End of String
  LCD_data(*string++); 	//sending data on LCD byte by byte
}
void TXstring(unsigned char *string)
{
	unsigned char i = 0;
	for(i = 0; i < string[i] != '\0'; i++)
	{
		TXbyte(string[i]);
	}
}	
void TXbyte(unsigned char data)
{
	while(TXSTAbits.TRMT == 0);
	TXREG = data;
}
unsigned char rx_data(void)
{
    while(PIR1bits.RCIF == 0);
	return(RCREG);
}
void msdelay(unsigned int time)
{
	unsigned int i,j;
	for(i = 0;i < time; i++)
	{
		for(j = 0; j < 710; j++);
	}
} 	

