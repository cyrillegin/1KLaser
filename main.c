#include <msp430.h> 
///#include <stdint.h>
#define uint8_t unsigned char
#define uint16_t unsigned int

/*
 * main.c
 */

#define LASER BIT0
#define SSOUT P1OUT
#define SSX BIT6
#define SSY BIT7
void writeMCP492x(uint16_t data,uint8_t ss);
void drawLine(uint8_t, uint8_t, uint8_t, uint8_t);

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
     PM5CTL0 &= ~LOCKLPM5;//sets to 12 mh

     CSCTL1 |= DCORSEL_4;//?

     P1DIR |= BIT0 + SSX + SSY;//p1 output
     P1OUT |= BIT0;//set bit 0

     ///////////////
     ///SPI Int//////
     ////////////////
     P5SEL0 |= BIT1 + BIT2;
     UCB0CTLW0 |= UCSWRST;                     // **Put state machine in reset**
     UCB0CTLW0 |= UCMST+UCSYNC+UCCKPL+UCMSB;   // 3-pin, 8-bit SPI master MSB
     UCB0CTLW0 |= UCSSEL_2;                    // SMCLK
     UCB0BR0 |= 0x01;						  // CLK / 1
     UCB0BR1 = 0;
     UCB0CTL1 &= ~UCSWRST;


     while(1){
    	 drawLine(192, 128, 160, 183);
    	 drawLine(160, 183, 96, 183);
    	 drawLine(96, 183, 64, 128);
    	 drawLine(64, 128, 95, 72);
    	 drawLine(95, 72, 160, 72);
    	 drawLine(160, 72, 192, 128);
     }
}
void drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2){

	int dx = (int)x2 - (int)x1;
	int dy = (int)y2 - (int)y1;
	int steps;
	    if (abs(dx) > abs(dy)){
	        steps = abs(dx/4);
	    }else{
	        steps = abs(dy/4);
	    }

	    float Xincrement = (float) dx / (float) steps;
	    float Yincrement = (float) dy / (float) steps;

	    float x = (float) x1;
	    float y = (float) y1;
	    int i;
	    for(i = 0; i < steps; i++){
	        x = x + Xincrement;
	        y = y + Yincrement;
			writeMCP492x( ((int)(x*16)),SSX);
	        writeMCP492x( ((int)(y*16)),SSY);
	    }
	    i++;
}

int abs(int val){
	return (val<0 ? (-val) : val);
}



void writeMCP492x(uint16_t data,uint8_t ss) {
  // Take the top 4 bits of config and the top 4 valid bits (data is actually a 12 bit number)
  //and OR them together
  uint8_t top_msg = (0x30 & 0xF0) | (0x0F & (data >> 8));

  // Take the bottom octet of data
  uint8_t lower_msg = (data & 0x00FF);

  // Select our DAC, Active LOW
  SSOUT &= ~ss;

  // Send first 8 bits
  UCB0TXBUF = top_msg;
  while (UCBUSY & UCB0STAT);

  // Send second 8 bits
  UCB0TXBUF = lower_msg;
  while (UCBUSY & UCB0STAT);
  //Deselect DAC
  SSOUT |= ss;
}
