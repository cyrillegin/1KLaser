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

     //double hex
//     uint16_t myPoly[] = {192, 128, 160, 183, 96, 183, 64, 128, 95, 72, 160, 72, 192, 128, 160, 128, 144, 155, 112, 155, 96, 128, 111, 100, 144, 100, 160, 128};
//     uint16_t polyLength = 28;
//     uint16_t offIndices[] = {12, 26};
//     uint16_t offLength = 2;

     //penrose triangle
     uint16_t myPoly[] = {230, 220, 245, 185, 150, 5, 65, 155, 150, 155, 130, 110, 150, 75, 230, 220, 25, 220, 5, 185, 175, 185, 150, 155, 130, 110, 110, 155, 5, 185, 110, 5, 150, 5};
     uint16_t polyLength = 34;
     uint16_t offIndices[] = {22, 26, 32};
     uint16_t offLength = 3;


     uint16_t offIter = 0;
     uint16_t myIndex = 0;

     while(1){
    	 	 int off = 0;
    	 	 for(offIter = 0; offIter < offLength; offIter++){
			 if(myIndex == offIndices[offIter]){
				 int j;
				 for(j = 0; j < 400; j++)
					 continue;
				P1OUT &= ~LASER;
				off = 1;
			 }
    	 	 }
    	 	 if(myIndex < polyLength-3){
    	 		 drawLine(myPoly[myIndex], myPoly[myIndex+1], myPoly[myIndex+2], myPoly[myIndex+3]);
    	 		myIndex = myIndex +2;
    	 	 } else {
    	 		drawLine(myPoly[myIndex], myPoly[myIndex+1], myPoly[0], myPoly[1]);
    	 		myIndex = 0;
    	 	 }
    	 	 if(off == 1){
    	 		int j;
    	 		for(j = 0; j < 250; j++)
    	 			continue;
    	 		 P1OUT |= LASER;
    	 	 }
     }
}

void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){

	uint16_t dx = x1 > x2 ? x1 - x2 : x2 - x1;
	uint16_t dy = y1 > y2 ? y1 - y2 : y2 - y1;

	uint16_t steps = dx > dy ? dx/6 : dy/6;//use 4 or 8 to reduce size by ~15 bytes.

	uint16_t Xincrement = (dx*100) / steps;
	uint16_t Yincrement = (dy*100) / steps;

	int x = x1*100;
	int y = y1*100;
	int i;

	for(i = 0; i < steps; i++){
		x = x1 < x2 ? x+Xincrement : x - Xincrement;
		y = y1 < y2 ? y+Yincrement : y - Yincrement;

	    	writeMCP492x((int)((x/100)*16), SSX);
	    	writeMCP492x((int)((y/100)*16), SSY);
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
