#include "REG_MPC82G516.H"
#include "userdefine.h"

unsigned char	RS232_First_Code, RS232_RW_Code, RS232_Addr_Code, RS232_Data_Code, RS232_Last_Code;

//--- for UART Command ----------------
unsigned char UART_RX_DATA = 0;
unsigned char UART_STATUS = 0; //0:Standby, 1:Command input, 2:Command ready.
unsigned char UART_INDEX = 0;
unsigned char UART_RX_BUF[5] = {0, 0, 0, 0, 0};
void UART_Variable_Reset(void);
void UART_init(void);  //UART啟始程式
void UART_Send(U8 Send_Data);

//--------------------------------------

void UART_main (void)//開始處理UART指令 
{
	RS232_First_Code = UART_RX_BUF[0];
	RS232_RW_Code = UART_RX_BUF[1];
	RS232_Addr_Code = UART_RX_BUF[2];
	RS232_Data_Code = UART_RX_BUF[3];
	RS232_Last_Code = UART_RX_BUF[4];
}//end of UART_main (void)

void UART_Interrupt() interrupt 4 
{
	U8 SEND_INDEX;
	if(RI) // RX interrupt
	{				                 /* if reception occur */
//		RI = 0; 			         /* clear reception flag for next reception */
//	  	UART_RX_DATA = SBUF;   /* Read receive data */

		RI = Low;
	  	UART_RX_DATA = SBUF;
		if((UART_STATUS == 0) && (UART_INDEX == 0))
		{
			if(UART_RX_DATA == RS232_Leader_Code)
			{
				UART_RX_BUF[0] = UART_RX_DATA;
				UART_INDEX = 1;
				UART_STATUS = 1;
				SEND_INDEX = 0;
			}
			else{UART_Variable_Reset();}
		}
		else if(UART_INDEX == 1)
		{
			if((UART_RX_DATA == RS232_Write) || (UART_RX_DATA == RS232_Read)) 
			{
				UART_RX_BUF[1] = UART_RX_DATA;
				UART_INDEX = 2;
				SEND_INDEX = 1;
			}
			else{UART_Variable_Reset();}
		}
		else if(UART_INDEX == 2)
		{
			if((UART_RX_BUF[1] == RS232_Write) || (UART_RX_BUF[1] == RS232_Read)) 
			{
				UART_RX_BUF[2] = UART_RX_DATA;
				UART_INDEX = 3;
				SEND_INDEX = 2;
			}
			else{UART_Variable_Reset();}
		}
		else if(UART_INDEX == 3)
		{
			if((UART_RX_BUF[1] == RS232_Write) || (UART_RX_BUF[1] == RS232_Read)) 
			{
				UART_RX_BUF[3] = UART_RX_DATA;
				UART_INDEX = 4;
				SEND_INDEX = 3;
			}
			else{UART_Variable_Reset();}
		}
		else if(UART_INDEX == 4)
		{
			if(UART_RX_DATA == RS232_End_Code) 
			{
				UART_RX_BUF[4] = UART_RX_DATA;
				UART_STATUS = 2;
				SEND_INDEX = 4;
			}
			else{UART_Variable_Reset();}
		}
	}//end of RX interrupt
//UART_Send(UART_RX_BUF[SEND_INDEX]);
}//end of UART_Interrupt() interrupt 4

void UART_init(void)  //UART啟始程式
{
	AUXR2 |= 0x40 ;
	SCON = 0x50;//Set UART Mode 1
	PCON = 0x80;//Set SMOD = 1 (Double Baud rate)
	TMOD = TMOD | 0x20 ;//Set Timer1 Mode 2
	TH1 = TL1 = 256-(((2 * Xtal_Freq)/Baud_Rate)/32) ;

	TR1 = 1; 						    // Timer 1 run //
	ES = 1; 						    // Enable serial interrupt	//
	EA = 1;						      // Enable global interrupt //
}//end of UART_init(void)  //UART啟始程式

void UART_Variable_Reset(void)
{
	char i = 0;
	UART_STATUS = 0;
	UART_INDEX = 0;
	UART_RX_DATA = 0;
	for(i = 0; i < 5; i++)
	{
		UART_RX_BUF[i] = 0;
	}
	RS232_First_Code = 0x00;
	RS232_RW_Code = 0x00;
	RS232_Addr_Code = 0x00;
	RS232_Data_Code = 0x00;
	RS232_Last_Code = 0x00;
}//end of UART_Variable_Reset(void)

//#ifdef DEBUG_RS232
void UART_Send(U8 Send_Data)
{
	SBUF = Send_Data;  
	while(~TI);
	TI = 0;
}//end of UART_Send(U8 Send_Data)
//#endif
