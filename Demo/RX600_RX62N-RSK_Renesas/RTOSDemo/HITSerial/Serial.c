#include "Serial.h"
#include "iodefine.h"
#include "FreeRTOS.h"
/****** init sci ********/
void sci6_init(void)
{
	 MSTP_SCI6 = 0;  
     // clear SCR     
     SCI6.SCR.BYTE = 0;

     /* set b6 bit for receiving serial data SCI6 */
     PORT3.ICR.BIT.B3 = 1;               //Enable RXD work as input
     PORT3.DDR.BIT.B2 = 1;               //set TXD working as output
     // set SMR and SCMR

     SCI6.SMR.BIT.CKS = 1;               // pclk / 4
     SCI6.SMR.BIT.STOP = 0;               // 1 stop bit
     SCI6.SMR.BIT.PE = 0;               // no parity
     SCI6.SMR.BIT.CHR = 0;               // 8 data bits
     SCI6.SMR.BIT.CM = 0;               // async mode

     SCI6.SCMR.BYTE = 0X00;               // disable smart card mode

     SCI6.SEMR.BIT.ABCS = 0;

     // set value in BRR
     SCI6.BRR = 38;               // set for 9600BPS with pclk/4
                                   // (12000000/64*(2^2(n-1))*9600)-1, n=1
     // wait bit interval ? ok to check status of previously set bit 
     nop(); //using OS delay to wait 
	 nop();
	 nop();
	 nop();

     // transfer starts
	 _IPR( _SCI6_RXI6 ) = configKERNEL_INTERRUPT_PRIORITY;	
	 _IPR( _SCI6_TXI6 ) = configKERNEL_INTERRUPT_PRIORITY;
	 _IPR( _SCI6_TEI6 ) = configKERNEL_INTERRUPT_PRIORITY;	
}


/****** allow sci re interrupt: default allow REI interrupt *******/
void sci6_allowSCIRXI(void)
{
	SCI6.SCR.BYTE &= 0XCF;                // close RE and TE for configure 
	SCI6.SCR.BIT.RE = 1;
	SCI6.SCR.BIT.RIE = 1;
	
	ICU.IER[0x1D].BIT.IEN7 = 1;          //allow RXI request to CPU for interruption	
}

/****** allow sci te interrupt: default allow TEI interrupt ******/
void sci6_allowSCITXI(void)
{
	SCI6.SCR.BYTE &= 0XCF;                // close RE and TE for configure 
	SCI6.SCR.BIT.TE = 1;
	SCI6.SCR.BIT.TIE = 1;
	SCI6.SCR.BIT.TEIE = 1;
	
	ICU.IER[0x1E].BIT.IEN0 = 1;          //allow TXI request to CPU for interruption
	ICU.IER[0x1E].BIT.IEN1 = 1;	         // allow TEI request to cpu fot interruption
}

/******** allow be-direction communication *******/
void sci6_allowSCIBECommunication()
{
	SCI6.SCR.BYTE &= 0XCF;                // close RE and TE for configure 
	SCI6.SCR.BYTE |= 0XF4;                // RE,RIE, TE ,TIE, TEIE
	
	ICU.IER[0x1D].BIT.IEN7 = 1;          //allow RXI request to CPU for interruption
	ICU.IER[0x1E].BIT.IEN0 = 1;          //allow TXI request to CPU for interruption
	ICU.IER[0x1E].BIT.IEN1 = 1;	         // allow TEI request to cpu fot interruption
}

/****** clear sci re status ********/
void sci6_clearREFault(void)
{
      if (SCI6.SSR.BIT.ORER == 1)    // memery out
      {
           SCI6.SSR.BIT.ORER = 0;
      }

      if (SCI6.SSR.BIT.PER == 1)     // parity error
      {
           SCI6.SSR.BIT.PER = 0;
      }

      if (SCI6.SSR.BIT.FER == 1)     // frame error
      {     
           SCI6.SSR.BIT.FER = 0;
      }      
}
