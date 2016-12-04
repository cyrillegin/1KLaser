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
void drawLine(uint16_t, uint16_t, uint16_t, uint16_t);

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
//    	 	 drawLine(160, 183, 96, 183);
//    	 	 drawLine(96, 183, 64, 128);
//    	 	drawLine(64, 128, 95, 72);
//    	 	drawLine(95, 72, 160, 72);
//    	 	drawLine(160, 72, 192, 128);
//
//		 P1OUT &= ~LASER;
//		 drawLine(192,128, 160, 128);
//		 P1OUT |= LASER;
//
//		 drawLine(160, 128, 144, 155);
//		 drawLine(144, 155, 112, 155);
//		 drawLine(112, 155, 96, 128);
//		 drawLine(96, 128, 111, 100);
//		 drawLine(111, 100, 144, 100);
//		 drawLine(144, 100, 160, 128);
//
//		 P1OUT &= ~LASER;
//		 drawLine(160,128, 192, 128);
//		 P1OUT |= LASER;
     }
}

void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){

	uint16_t dx1 = x1 > x2 ? x1 - x2 : x2 - x1;
	uint16_t dy1 = y1 > y2 ? y1 - y2 : y2 - y1;

	uint16_t steps1 = dx1 > dy1 ? dx1/4 : dy1/4;

	uint16_t Xincrement1 = (dx1*100) / steps1;
	uint16_t Yincrement1 = (dy1*100) / steps1;

	int x11 = x1*100;
	int y11 = y1*100;
	int i;

	for(i = 0; i < steps1; i++){

		x11 = x1 < x2 ? x11+Xincrement1 : x11 - Xincrement1;
		y11 = y1 < y2 ? y11+Yincrement1 : y11 - Yincrement1;

	    	writeMCP492x((int)((x11/100)*16), SSX);
	    	writeMCP492x((int)((y11/100)*16), SSY);
	}
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
