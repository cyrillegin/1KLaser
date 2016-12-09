#include <msp430.h> 

#define uint8_t unsigned char
#define uint16_t unsigned int

#define LASER BIT0
#define SSOUT P1OUT
#define SSX BIT6
#define SSY BIT7

#define length 234

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

     //left side
     uint8_t logo[] = {66, 158, 47, 177, 37, 176, 27, 177, 16, 186, 8, 195, 8, 212, 27, 195, 47, 220, 27, 236, 40, 238, 56, 233, 66, 220, 68, 212, 68, 203, 86, 186, 75, 175, 66, 158, //lower left wrench:36
         	66, 98, 47, 79, 37, 80, 27, 79, 16, 70, 8, 61, 8, 44, 27, 61, 47, 36, 27, 20, 40, 18, 56, 23, 66, 36, 68, 44, 68, 53, 86, 70, 75, 81, 66, 98, //upper left wrench:36
     		118, 125, 110, 136, 102, 138, 86, 120, 95, 105, 97, 108, 102, 114, 110, 118, 118, 125, //left eye:18
			190, 158, 209, 177, 219, 176, 229, 177, 240, 186, 248, 195, 248, 212, 229, 195, 209, 220, 229, 236, 216, 238, 200, 233, 190, 220, 188, 212, 188, 203, 170, 186, 181, 175, 190, 158,//lower right wrench:36
			190, 98, 209, 79, 219, 80, 229, 79, 240, 70, 248, 61, 248, 44, 229, 61, 209, 36, 229, 20, 216, 18, 200, 23, 190, 36, 188, 44, 188, 53, 170, 70, 181, 81, 190, 98, //upper right wrench:36
			138, 125, 146, 136, 154, 138, 170, 120, 161, 105, 159, 108, 154, 114, 146, 118, 138, 125,//right eye: 18
			80, 159, 127, 180, 175, 159, 187, 120, 175, 84, 162, 70, 155, 55, 146, 55, 140, 70, 137, 70, 131, 55, 121, 55, 117, 70, 115, 70, 108, 55, 98, 55, 93, 74, 80, 84, 67, 120, 80, 159,//face: 40
			127, 103, 130, 91, 133, 83, 127, 91, 121, 83, 125, 91, 127, 103//nose:14
     };

     uint8_t myIndex = 0;
     uint16_t counter;
     while(1){
    	 	 	if(myIndex < length-3){
    	 	 		if(myIndex == 34 || myIndex == 70 || myIndex == 88 || myIndex == 124 || myIndex ==160 || myIndex == 178 || myIndex == 218 || myIndex == 232){
    	 	 			for(counter = 1200; counter > 0; counter--){
    	 	 				counter = counter- 1;
    	 	 			}
    	 	 			P1OUT &= ~LASER;
    	 	 		}
    	 	 	    	drawLine(logo[myIndex], logo[myIndex+1], logo[myIndex+2], logo[myIndex+3]);
    	 	 	    	if(myIndex == 34 || myIndex == 70 || myIndex == 88 || myIndex == 124 || myIndex ==160 || myIndex == 178 || myIndex == 218 || myIndex == 232){
    	 	 	    		for(counter = 800; counter > 0; counter--){
    	 	 	    			counter = counter- 1;
    	 	 	    		}
    	 	 	    	   P1OUT |= LASER;
    	 	 	    	}
    	 	 	    	myIndex = myIndex +2;
    	 	 	} else {
    	 	 		for(counter = 1200; counter > 0; counter--){
    	 	 			counter = counter- 1;
    	 	 		}
    	 	 		P1OUT &= ~LASER;
    	 	 	    	drawLine(logo[myIndex], logo[myIndex+1], logo[0], logo[1]);
    	 	 	    	for(counter = 800; counter > 0; counter--){
    	 	 	    		counter = counter- 1;
    	 	 	    	}
    	 	 	    P1OUT |= LASER;
    	 	 	    myIndex = 0;
    	 	 	}
    	 	  }

}

void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){

	uint16_t dx = x1 > x2 ? x1 - x2 : x2 - x1;
	uint16_t dy = y1 > y2 ? y1 - y2 : y2 - y1;

	uint16_t steps = dx > dy ? dx : dy;

	uint16_t finalSteps = 1;
	while(finalSteps < steps){
		finalSteps = finalSteps << 1;
	}
	finalSteps = finalSteps >> 1;

	uint16_t Xincrement = (dx*128) / finalSteps;
	uint16_t Yincrement = (dy*128) / finalSteps;

	uint16_t x = x1*128;
	uint16_t y = y1*128;

	for(steps = 0; steps < finalSteps; steps++){
		x = x1 < x2 ? x+Xincrement : x - Xincrement;
		y = y1 < y2 ? y+Yincrement : y - Yincrement;

	    	writeMCP492x(x/8, SSX);
	    	writeMCP492x(y/8, SSY);
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
