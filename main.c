#include <msp430.h> 

#define uint8_t unsigned char
#define uint16_t unsigned int

#define LASER BIT0
#define SSOUT P1OUT
#define SSX BIT6
#define SSY BIT7

#define wrenchLength 36
#define faceLength 54
#define eyeLength 18

//lower left wrench
    uint8_t wrench[] = {66, 158, 47, 177, 37, 176, 27, 177, 16, 186, 8, 195, 8, 212, 27, 195, 47, 220, 27, 236, 40, 238, 56, 233, 66, 220, 68, 212, 68, 203, 86, 186, 75, 175, 66, 158};

     //face
     const uint8_t face[] = {80, 159, 127, 180, 175, 159, 187, 120, 175, 84, 162, 70, 155, 55, 146, 55, 140, 70, 137, 70, 131, 55, 121, 55, 117, 70, 115, 70, 108, 55, 98, 55, 93, 74, 80, 84, 67, 120, 80, 159, 127, 103, 130, 91, 133, 83, 127, 91, 121, 83, 125, 91, 127, 103};

     //Left eye
     uint8_t leftEye[] = {118, 125, 110, 136, 102, 138, 86, 120, 95, 105, 97, 108, 102, 114, 110, 118, 118, 125};

void writeMCP492x(uint16_t data,uint8_t ss);
void drawLine(uint8_t, uint8_t, uint8_t, uint8_t);

void stall(uint16_t a){
	uint16_t counter = a == 0 ? 8000 : 8000;
	for(counter = 0; counter > 0; counter--){
		if(counter < 1){
			counter = 0;
		}
	}
}

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




     uint8_t myIndex = 0;
     uint8_t logoPart = 0;
     uint8_t mirrorCount = 4;
     uint8_t direction = 0;
     uint8_t i;
     uint8_t oldX;
     uint8_t oldY;
     while(1){
    	 	 if(logoPart == 0){
    	 	 	 //Draws the wrenches
    	 	 		 if(mirrorCount > 0){
					 if(myIndex < wrenchLength-3){
						drawLine(wrench[myIndex], wrench[myIndex+1], wrench[myIndex+2], wrench[myIndex+3]);
						myIndex = myIndex +2;
					  } else {
						  if(mirrorCount > 1){
							  oldX = wrench[myIndex];
							  oldY = wrench[myIndex+1];
							  for(i=0; i < wrenchLength; i++){
								if(i%2 == 0){
									if(direction == 0){
									  	wrench[i] = 256 - wrench[i];
									} else {
										wrench[i+1] = 256 - wrench[i+1];
									}
								 }
							  }
							  direction = direction == 1 ? 0 : 1;
							  stall(0);
							  P1OUT &= ~LASER;
							  drawLine(oldX, oldY, wrench[0], wrench[1]);
							  stall(1);
							  P1OUT |= LASER;
						  }
						  myIndex = 0;
						  mirrorCount = mirrorCount - 1;
					 }
    	 	 		 } else {
    	 	 			stall(0);
    	 	 			P1OUT &= ~LASER;
    	 	 			drawLine(wrench[myIndex], wrench[myIndex+1], leftEye[0], leftEye[1]);
    	 	 			stall(1);
    	 	 			P1OUT |= LASER;
    	 	 			mirrorCount = 2;
    	 	 			logoPart = 1;
    	 	 		 }
    	 	 	 }
			//draws the eyes
    	 	 	if(logoPart == 1){
    	 	 		 if(myIndex < eyeLength-3){
    	 	 			drawLine(leftEye[myIndex], leftEye[myIndex+1], leftEye[myIndex+2], leftEye[myIndex+3]);
    	 	 			myIndex = myIndex +2;
    	 	 		} else if(mirrorCount > 1) {

    	 	 		    oldX = leftEye[myIndex];
    	 	 		  	oldY = leftEye[myIndex+1];
    	 	 		    	for(i=0; i < eyeLength; i++){
    	 	 		    		if(i%2 == 0){
    	 	 		    			leftEye[i] = 256 - leftEye[i];
    	 	 		    		}
    	 	 		    	}
    	 	 		    	stall(0);
    	 	 		    	P1OUT &= ~LASER;
    	 	 		    drawLine(oldX, oldY, leftEye[0], leftEye[1]);
    	 	 		    stall(1);
    	 	 		    P1OUT |= LASER;
    	 	 		    myIndex = 0;
    	 	 		    mirrorCount = mirrorCount - 1;
    	 	 		} else {
    	 	 			stall(0);
    	 	 			P1OUT &= ~LASER;
    	 	 			drawLine(leftEye[myIndex], leftEye[myIndex+1], face[0], face[1]);
    	 	 			stall(1);
    	 	 			P1OUT |= LASER;
    	 	 			myIndex = 0;
    	 	 			mirrorCount = 4;
    	 	 			logoPart = 2;
    	 	 		}
    	 	 	}
    	 	 	//draws the face and nose
    	 	 	 if(logoPart == 2){
    	 	 		if(myIndex < faceLength-3){
    	 	 			if(myIndex == 38){
    	 	 				stall(1);
    	 	 				P1OUT &= ~LASER;
    	 	 			}
    	 	 		    	drawLine(face[myIndex], face[myIndex+1], face[myIndex+2], face[myIndex+3]);
    	 	 		    	if(myIndex == 38){
    	 	 		    	   stall(1);
    	 	 		    	   P1OUT |= LASER;
    	 	 		    	}
    	 	 		    	myIndex = myIndex +2;
    	 	 		} else {
    	 	 			stall(0);
    	 	 			P1OUT &= ~LASER;
    	 	 		    	drawLine(face[myIndex], face[myIndex+1], wrench[0], wrench[1]);
    	 	 		    stall(1);
    	 	 		    P1OUT |= LASER;
    	 	 			logoPart = 0;
    	 	 		    myIndex = 0;
    	 	 		    mirrorCount = 4;
    	 	 		}
    	 	 	 }
    	 	 }
}



void drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2){

	uint8_t dx = x1 > x2 ? x1 - x2 : x2 - x1;
	uint8_t dy = y1 > y2 ? y1 - y2 : y2 - y1;

	uint8_t steps = dx > dy ? dx : dy;

	uint8_t finalSteps = 1;
	while(finalSteps < steps){
		finalSteps = finalSteps << 1;
	}
	finalSteps = finalSteps >> 1;

	uint16_t Xincrement = (dx*128) / finalSteps;
	uint16_t Yincrement = (dy*128) / finalSteps;

	uint16_t x = x1*128;
	uint16_t y = y1*128;
	uint16_t i;

	for(i = 0; i < finalSteps; i++){
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
