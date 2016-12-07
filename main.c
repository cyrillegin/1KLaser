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


     //lower left wrench
     uint16_t wrench[] = {265, 635, 190, 710, 150, 705, 110, 710, 65, 745, 35, 780, 35, 850, 110, 780, 190, 880, 110, 945, 160, 955, 225, 935, 265, 880, 275, 850, 275, 815, 345, 745, 300, 700, 265, 635};
     volatile uint16_t wrenchLength = 36;

     //Skull outer
     uint16_t face[] = {320, 390, 510, 305, 700, 390, 750, 545, 700, 690, 650, 745, 620, 805, 585, 805, 560, 745, 550, 745, 525, 805, 485, 805, 470, 745, 460, 745, 435, 805, 395, 805, 375, 730, 320, 690, 270, 545, 320, 390};
     uint16_t faceLength = 40;

     //Left eye
       uint16_t leftEye[] = {475, 525, 440, 480, 410, 475, 345, 545, 380, 605, 390, 595, 410, 570, 440, 555, 475, 525};
       uint16_t eyeLength = 18;

     //Nose
       uint16_t nose[] = {510, 615, 520, 660, 535, 695, 510, 660, 485, 695, 500, 660, 510, 615};
       uint16_t noseLength = 14;

     mirrorY(face, faceLength, 1024);
     mirrorY(leftEye, eyeLength, 1024);
     mirrorY(nose, noseLength, 1024);

     uint16_t downsizeIter = 0;
     for(downsizeIter = 0; downsizeIter < wrenchLength; downsizeIter++){
    	 	 wrench[downsizeIter] = wrench[downsizeIter]/4;
     }
     for(downsizeIter = 0; downsizeIter < eyeLength; downsizeIter++){
      	 leftEye[downsizeIter] = leftEye[downsizeIter]/4;
     }
     for(downsizeIter = 0; downsizeIter < noseLength; downsizeIter++){
       	nose[downsizeIter] = nose[downsizeIter]/4;
     }
     for(downsizeIter = 0; downsizeIter < faceLength; downsizeIter++){
       	 face[downsizeIter] = face[downsizeIter]/4;
     }

     uint16_t myIndex = 0;
     uint16_t logoPart = 0;
     uint16_t mirrorCount = 4;
     uint16_t laserDelay;
     uint16_t direction = 0;
     while(1){
    	 	 switch(logoPart){
    	 	 	 //Draws the wrenches
    	 	 	 case 0:
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
								  mirrorX(wrench, wrenchLength, 256);
								  direction = 1;
							  } else {
								  mirrorY(wrench, wrenchLength, 256);
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
				 break;
			//draws the eyes
    	 	 	 case 1:
    	 	 		 if(myIndex < eyeLength-3){
    	 	 			drawLine(leftEye[myIndex], leftEye[myIndex+1], leftEye[myIndex+2], leftEye[myIndex+3]);
    	 	 			myIndex = myIndex +2;
    	 	 		} else if(mirrorCount > 1) {
    	 	 		    for(laserDelay = 0; laserDelay < 400; laserDelay++)
    	 	 		       continue;
    	 	 		    P1OUT &= ~LASER;
    	 	 		    uint16_t oldX = leftEye[myIndex];
    	 	 		  	uint16_t oldY = leftEye[myIndex+1];
    	 	 		    	mirrorX(leftEye, eyeLength, 256);
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
    	 	 		break;
    	 	 	//draws the nose
    	 	 	 case 2:
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
    	 	 		 break;
    	 	 	//draws the face
    	 	 	 case 3:
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
    	 	 		break;
    	 	 }
     }
}

void mirrorX(uint16_t *arr, uint16_t length, uint16_t maxX){
	uint16_t i;
	for(i=0; i < length; i++){
		if(i%2 == 0){
			arr[i] = maxX - arr[i];
		}
	}
}

void mirrorY(uint16_t *arr, uint16_t length, uint16_t maxY){
	uint16_t i;
	for(i=0; i < length; i++){
		if(i%2 == 1){
			arr[i] = maxY - arr[i];
		}
	}
}

void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){

	uint16_t dx = x1 > x2 ? x1 - x2 : x2 - x1;
	uint16_t dy = y1 > y2 ? y1 - y2 : y2 - y1;

	uint16_t steps = dx > dy ? dx/2 : dy/2;//(penrose only) use 6 for best result. use 4 or 8 to reduce size by ~15 bytes. 4 blinks and 8 seems too fast though..

	uint16_t finalSteps = 1;
	while(finalSteps < steps){
		finalSteps = finalSteps << 1;
	}
	finalSteps = finalSteps >> 1;

	uint16_t Xincrement = (dx*128) / finalSteps;
	uint16_t Yincrement = (dy*128) / finalSteps;

	int x = x1*128;
	int y = y1*128;
	int i;

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
