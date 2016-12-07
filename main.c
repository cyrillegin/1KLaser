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
//     uint16_t myPoly[] = {230, 220, 245, 185, 150, 5, 65, 155, 150, 155, 130, 110, 150, 75, 230, 220, 25, 220, 5, 185, 175, 185, 150, 155, 130, 110, 110, 155, 5, 185, 110, 5, 150, 5};
//     uint16_t polyLength = 34;
//     uint16_t offIndices[] = {22, 26, 32};
//     uint16_t offLength = 3;

     //hack a day all:
//       uint16_t myPoly[] = {265, 635, 190, 710, 150, 705, 110, 710, 65, 745, 35, 780, 35, 850, 110, 780, 190, 880, 110, 945, 160, 955, 225, 935, 265, 880, 275, 850, 275, 815, 345, 745, 300, 700, 265, 635,
//    		   385, 310, 275, 205, 275, 170, 245, 105, 190, 70, 150, 65, 110, 70, 190, 140, 110, 240, 35, 170, 35, 205, 65, 280, 110, 310, 150, 315, 180, 310, 295, 405, 325, 350, 385, 310,
//		   755, 635, 840, 710, 875, 705, 910, 710, 960, 745, 990, 815, 990, 850, 910, 780, 830, 880, 910, 945, 875, 955, 830, 945, 775, 910, 755, 880, 745, 815, 675, 745, 720, 700, 755, 635,
//		   645, 315, 755, 210, 760, 135, 790, 100, 825, 80, 865, 70, 915, 75, 835, 140, 915, 240, 995, 170, 985, 250, 955, 290, 915, 305, 875, 310, 845, 315, 735, 410, 695, 350, 645, 315,
//		   320, 390, 510, 305, 700, 390, 750, 545, 700, 690, 650, 745, 620, 805, 585, 805, 560, 745, 550, 745, 525, 805, 485, 805, 470, 745, 460, 745, 435, 805, 395, 805, 375, 730, 320, 690, 270, 545, 320, 390,
//		   475, 525, 440, 480, 410, 475, 345, 545, 380, 605, 390, 595, 410, 570, 440, 555, 475, 525,
//		   550, 525, 580, 480, 615, 475, 680, 545, 645, 605, 635, 595, 615, 570, 580, 555, 550, 525,
//		   510, 615, 520, 660, 535, 695, 510, 660, 485, 695, 500, 660, 510, 615
//       };
//       uint16_t polyLength = 234;
//
//       uint16_t offIndices[] = {34, 70, 106, 142, 182, 200, 218, 232};
//       uint16_t offLength = 8;

     //lower left wrench
     uint16_t lowerLeftWrenchPoly[] = {265, 635, 190, 710, 150, 705, 110, 710, 65, 745, 35, 780, 35, 850, 110, 780, 190, 880, 110, 945, 160, 955, 225, 935, 265, 880, 275, 850, 275, 815, 345, 745, 300, 700, 265, 635};
     volatile uint16_t wrenchLength = 36;

     //upper left wrench
//     uint16_t myPoly[] = {385, 310, 275, 205, 275, 170, 245, 105, 190, 70, 150, 65, 110, 70, 190, 140, 110, 240, 35, 170, 35, 205, 65, 280, 110, 310, 150, 315, 180, 310, 295, 405, 325, 350, 385, 310};
//     uint16_t polyLength = 36;

     //lower right wrench
//     uint16_t myPoly[] = {755, 635, 840, 710, 875, 705, 910, 710, 960, 745, 990, 815, 990, 850, 910, 780, 830, 880, 910, 945, 875, 955, 830, 945, 775, 910, 755, 880, 745, 815, 675, 745, 720, 700, 755, 635};
//     uint16_t polyLength = 36;

     //upper right wrench
//     uint16_t myPoly[] = {645, 315, 755, 210, 760, 135, 790, 100, 825, 80, 865, 70, 915, 75, 835, 140, 915, 240, 995, 170, 985, 250, 955, 290, 915, 305, 875, 310, 845, 315, 735, 410, 695, 350, 645, 315};
//     uint16_t polyLength = 36;

     //Skull outer
//     uint16_t myPoly[] = {320, 390, 510, 305, 700, 390, 750, 545, 700, 690, 650, 745, 620, 805, 585, 805, 560, 745, 550, 745, 525, 805, 485, 805, 470, 745, 460, 745, 435, 805, 395, 805, 375, 730, 320, 690, 270, 545, 320, 390};
//     uint16_t polyLength = 40;

     //Left eye
//       uint16_t leftEye[] = {475, 525, 440, 480, 410, 475, 345, 545, 380, 605, 390, 595, 410, 570, 440, 555, 475, 525};
//       uint16_t leftEyeLength = 18;

     //Right eye
//       uint16_t rightEye[] = {550, 525, 580, 480, 615, 475, 680, 545, 645, 605, 635, 595, 615, 570, 580, 555, 550, 525};
//       uint16_t rightEyeLength = 18;

     //Nose
//       uint16_t nose[] = {510, 615, 520, 660, 535, 695, 510, 660, 485, 695, 500, 660, 510, 615};
//       uint16_t noseLength = 14;

     uint16_t offIndices[] = {1};
     uint16_t offLength = 1;


     uint16_t myIndex = 0;

     uint16_t downsizeIter = 0;
     for(downsizeIter = 0; downsizeIter < wrenchLength; downsizeIter++){
    	 	 lowerLeftWrenchPoly[downsizeIter] = lowerLeftWrenchPoly[downsizeIter]/4;
     }

     int direction = 0;
     while(1){
    	 	 if(myIndex < wrenchLength-3){
    	     	drawLine(lowerLeftWrenchPoly[myIndex], lowerLeftWrenchPoly[myIndex+1], lowerLeftWrenchPoly[myIndex+2], lowerLeftWrenchPoly[myIndex+3]);
    	     	myIndex = myIndex +2;
    	      } else {
    	    	  	int oldX = lowerLeftWrenchPoly[myIndex];
    	    	  	int oldY = lowerLeftWrenchPoly[myIndex+1];
    	    	  	if(direction == 0){
    	    	  		mirrorX(lowerLeftWrenchPoly, wrenchLength, 256);
    	    	  		direction = 1;
    	    	  	} else {
    	    	  		mirrorY(lowerLeftWrenchPoly, wrenchLength, 256);
    	    	  		direction = 0;
    	    	  	}
    	     	drawLine(oldX, oldY, lowerLeftWrenchPoly[0], lowerLeftWrenchPoly[1]);
    	     	myIndex = 0;
    	      }
     }








//     uint16_t offIter;

//     while(1){
//    	 	 int off = 0;
//    	 	 for(offIter = 0; offIter < offLength; offIter++){
//			 if(myIndex == offIndices[offIter]){
//				 int j;
//				 for(j = 0; j < 400; j++)
//					 continue;
//				P1OUT &= ~LASER;
//				off = 1;
//			 }
//    	 	 }
//    	 	 if(myIndex < polyLength-3){
//    	 		drawLine(myPoly[myIndex], myPoly[myIndex+1], myPoly[myIndex+2], myPoly[myIndex+3]);
//    	 		myIndex = myIndex +2;
//    	 	 } else {
//    	 		drawLine(myPoly[myIndex], myPoly[myIndex+1], myPoly[0], myPoly[1]);
//    	 		myIndex = 0;
//    	 	 }
//    	 	 if(off == 1){
//    	 		int j;
//    	 		for(j = 0; j < 250; j++)
//    	 			continue;
//    	 		 P1OUT |= LASER;
//    	 	 }
//     }
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
