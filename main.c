#include <msp430.h> 
///#include <stdint.h>
#define uint8_t unsigned char
#define uint16_t unsigned int

/*
 * main.c
 */


#define SSOUT P1OUT
#define SSX BIT6
#define SSY BIT7
void writeMCP492x(uint16_t data,uint8_t ss);


int main(void) {
	uint8_t i = 3;
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



    while(1) {
	for(i=0;i<255;i++)  {
		writeMCP492x(2048 + i*4,SSX);
		writeMCP492x(2048 - i*4,SSY);
		}
	for(i=255;i>0;i--)  {
		writeMCP492x(2048 + i*4,SSX);
		writeMCP492x(2048 - i*4,SSY);
		}
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
