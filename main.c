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
void drawLine(int, int, int, int);

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


//     while(1){
//    	 	 drawLine(192, 128, 160, 183);
//    	 	 drawLine(160, 183, 96, 183);
//    	 	 drawLine(96, 183, 64, 128);
//    	 	 drawLine(64, 128, 95, 72);
//    	 	 drawLine(95, 72, 160, 72);
//		 drawLine(160, 72, 192, 128);
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
//     }
     drawLine(192, 128, 160, 183);
}

void drawLine(int x0, int y0, int x1, int y1){
	int dx = 0;
	if(x0 > x1){
		dx = x0-x1;
	}else{
		dx = x1-x0;
	}

	int dy = 0;
	if(y0 > y1){
		dy = y0-y1;
	}else{
		dy = y1-y0;
	}

	int steps = dx > dy ? dx : dy;

	int Xincrement = (dx*100) / steps;
	int Yincrement = (dy*100) / steps;

	int x = x0;
	int y = y0;
	volatile int i;
	int arrX[100];
	int arrY[100];

	for(i=0;i < steps; i++){
	    x = x + (Xincrement/10);
	    y = y + (Yincrement/10);
	    arrX[i] = x;
	    arrY[i] = y;
//	    writeMCP492x( (((x/10)*16)),SSX);
//	    writeMCP492x( (((y/10)*16)),SSY);
	}
	i++;
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
