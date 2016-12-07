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

void mirrorX(uint16_t *arr, uint16_t length){
	uint16_t i;
	for(i=0; i < length; i++){
		if(i%2 == 0){
			arr[i] = 256 - arr[i];
		}
	}
}

void mirrorY(uint16_t *arr, uint16_t length){
	uint16_t i;
	for(i=0; i < length; i++){
		if(i%2 == 1){
			arr[i] = 256 - arr[i];
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


     //lower left wrench
     uint16_t wrench[] = {66, 158, 47, 177, 37, 176, 27, 177, 16, 186, 8, 195, 8, 212, 27, 195, 47, 220, 27, 236, 40, 238, 56, 233, 66, 220, 68, 212, 68, 203, 86, 186, 75, 175, 66, 158};
     uint16_t wrenchLength = 36;

     //face
     uint16_t face[] = {80, 159, 127, 180, 175, 159, 187, 120, 175, 84, 162, 70, 155, 55, 146, 55, 140, 70, 137, 70, 131, 55, 121, 55, 117, 70, 115, 70, 108, 55, 98, 55, 93, 74, 80, 84, 67, 120, 80, 159};
     uint16_t faceLength = 40;

     //Left eye
     uint16_t leftEye[] = {118, 125, 110, 136, 102, 138, 86, 120, 95, 105, 97, 108, 102, 114, 110, 118, 118, 125};
       uint16_t eyeLength = 18;

     //Nose
     uint16_t nose[] = {127, 103, 130, 91, 133, 83, 127, 91, 121, 83, 125, 91, 127, 103};
     uint16_t noseLength = 14;

     uint16_t myIndex = 0;
     uint16_t logoPart = 0;
     uint16_t mirrorCount = 4;
     uint16_t laserDelay;
     uint16_t direction = 0;
     while(1){
    	 	 if(logoPart == 0){
    	 	 	 //Draws the wrenches
    	 	 		 if(mirrorCount > 0){
					 if(myIndex < wrenchLength-3){
						drawLine(wrench[myIndex], wrench[myIndex+1], wrench[myIndex+2], wrench[myIndex+3]);
						myIndex = myIndex +2;
					  } else {
						  for(laserDelay = 0; laserDelay < 400; laserDelay++)
							  continue;
						  P1OUT &= ~LASER;
						  if(mirrorCount > 1){
							  uint16_t oldX = wrench[myIndex];
							  uint16_t oldY = wrench[myIndex+1];
							  if(direction == 0){
								  mirrorX(wrench, wrenchLength);
								  direction = 1;
							  } else {
								  mirrorY(wrench, wrenchLength);
								  direction = 0;
							  }
							  drawLine(oldX, oldY, wrench[0], wrench[1]);
						  }
						  for(laserDelay = 0; laserDelay < 250; laserDelay++)
							  continue;
						  P1OUT |= LASER;
						  myIndex = 0;
						  mirrorCount = mirrorCount - 1;
					 }
    	 	 		 } else {
    	 	 			for(laserDelay = 0; laserDelay < 400; laserDelay++)
    	 	 				continue;
    	 	 			P1OUT &= ~LASER;
    	 	 			mirrorCount = 2;
    	 	 			drawLine(wrench[myIndex], wrench[myIndex+1], leftEye[0], leftEye[1]);
    	 	 			logoPart = 1;
    	 	 			for(laserDelay = 0; laserDelay < 250; laserDelay++)
    	 	 				continue;
    	 	 			P1OUT |= LASER;
    	 	 		 }
    	 	 }
			//draws the eyes
    	 	 	if(logoPart == 1){
    	 	 		 if(myIndex < eyeLength-3){
    	 	 			drawLine(leftEye[myIndex], leftEye[myIndex+1], leftEye[myIndex+2], leftEye[myIndex+3]);
    	 	 			myIndex = myIndex +2;
    	 	 		} else if(mirrorCount > 1) {
    	 	 		    for(laserDelay = 0; laserDelay < 400; laserDelay++)
    	 	 		       continue;
    	 	 		    P1OUT &= ~LASER;
    	 	 		    uint16_t oldX = leftEye[myIndex];
    	 	 		  	uint16_t oldY = leftEye[myIndex+1];
    	 	 		    	mirrorX(leftEye, eyeLength);
    	 	 		    drawLine(oldX, oldY, leftEye[0], leftEye[1]);
    	 	 		    myIndex = 0;
    	 	 		    for(laserDelay = 0; laserDelay < 250; laserDelay++)
    	 	 		    		continue;
    	 	 		    P1OUT |= LASER;
    	 	 		    mirrorCount = mirrorCount - 1;
    	 	 		} else {
    	 	 			for(laserDelay = 0; laserDelay < 400; laserDelay++)
    	 	 			    	continue;
    	 	 			P1OUT &= ~LASER;
    	 	 			 mirrorCount = 4;
    	 	 			 drawLine(leftEye[myIndex], leftEye[myIndex+1], nose[0], nose[1]);
    	 	 			 myIndex = 0;
    	 	 			 logoPart = 2;
    	 	 			for(laserDelay = 0; laserDelay < 250; laserDelay++)
    	 	 				continue;
    	 	 			P1OUT |= LASER;
    	 	 		}
    	 	 	}
    	 	 	//draws the nose
    	 	 	 if(logoPart == 2){
    	 	 		 if(myIndex < noseLength-3){
    	 	 		    	 drawLine(nose[myIndex], nose[myIndex+1], nose[myIndex+2], nose[myIndex+3]);
    	 	 		    	 myIndex = myIndex +2;
    	 	 		 } else {
    	 	 			for(laserDelay = 0; laserDelay < 400; laserDelay++)
    	 	 			    	 continue;
    	 	 			P1OUT &= ~LASER;
    	 	 		    	drawLine(nose[myIndex], nose[myIndex+1], face[0], face[1]);
    	 	 		    	logoPart = 3;
    	 	 		    	myIndex = 0;
    	 	 		    	for(laserDelay = 0; laserDelay < 250; laserDelay++)
    	 	 		    	   continue;
    	 	 		    P1OUT |= LASER;
    	 	 		 }
    	 	 	 }
    	 	 	//draws the face
    	 	 	 if(logoPart == 3){
    	 	 		if(myIndex < faceLength-3){
    	 	 		    	drawLine(face[myIndex], face[myIndex+1], face[myIndex+2], face[myIndex+3]);
    	 	 		    	myIndex = myIndex +2;
    	 	 		} else {
    	 	 			for(laserDelay = 0; laserDelay < 400; laserDelay++)
    	 	 			   continue;
    	 	 			P1OUT &= ~LASER;
    	 	 		    	drawLine(face[myIndex], face[myIndex+1], wrench[0], wrench[1]);
    	 	 		    	logoPart = 0;
    	 	 		    	myIndex = 0;
    	 	 		    	mirrorCount = 4;
    	 	 		    	for(laserDelay = 0; laserDelay < 250; laserDelay++)
    	 	 		    	    continue;
    	 	 		    P1OUT |= LASER;
    	 	 		}
    	 	 	 }
    	 	 }

}



void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){

	uint16_t dx = x1 > x2 ? x1 - x2 : x2 - x1;
	uint16_t dy = y1 > y2 ? y1 - y2 : y2 - y1;

	uint16_t steps = dx > dy ? dx/2 : dy/2;

	uint16_t finalSteps = 1;
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

	    	writeMCP492x(x/8, SSX);//used to be: (x/128)*16
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
