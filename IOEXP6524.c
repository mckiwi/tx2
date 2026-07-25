//////////////////////////////////////////////////////////////
//;
//; IOEXP6524.c
//;  For new IO Expander NXP
//;  TDF,202205 added, Dennis
//;
//////////////////////////////////////////////////////////////

#include "REG_MPC82G516.H"
#include "userdefine.h"

#define SET_IO_SCL  ioSCL = 1
#define CLR_IO_SCL  ioSCL = 0
#define SET_IO_SDA  ioSDA = 1
#define CLR_IO_SDA  ioSDA = 0

#define IO_SDA_OUT  ioSDA = 0	  
#define IO_SDA_IN   ioSDA = 1

//
void 	ioWriteOneByte(U8 expIOAddr, U8 IICSUB_Address, U8 IIC_Data);	//~DA,240807
U8 		ioReadOneByte(U8 expIOAddr, U8 IICSUB_Address);	//~DA,240807

void 	ioSTART(void);
void 	ioSTOP(void);
void 	ioCSS_Wait(U16 tt);
void 	ioAck(U8 ans);
U8 		ioCheckAck(void);

void 	ioWriteByte(U8 I2cData);
U8 		ioReadByte(void);
void 	ioioSendStr(U8 IIC_Address, U8 IIC_Data);
void 	ioioReadStr(U8 IIC_Address, U8 IIC_Data);

//:D,A
void 	ioWrite3Ports(U8 *I2cDataPorts);	//~DA,240809
void 	ioWrite2Ports(U8 *I2cDataPorts, U8 index);	//~DA,240809
void 	ioWrite1Ports(U8 *I2cDataPorts, U8 index);	//~DA,240809	
void 	ioWriteThreePorts(U8 expIOAddr, U8 IICSUB_Address, U8 *I2cDataPorts);	//~DA,240809
void 	ioWriteTwoPorts(U8 expIOAddr, U8 IICSUB_Address, U8 *I2cDataPorts, U8 index);	//~DA,240809
void 	ioWriteOnePorts(U8 expIOAddr, U8 IICSUB_Address, U8 *I2cDataPorts, U8 index);	//~DA,240809
void 	initIOPorts(U8 expICNum);	//~DA,240807



//~DA,240807
volatile U8 IOPORTS_DATBK[2][3]={0};

//---------------------------------------------------------------------------------
//Start 
//---------------------------------------------------------------------------------
void ioSTART(void)
{
	SET_IO_SDA;
	SET_IO_SCL;
	CLR_IO_SDA;
	CLR_IO_SCL;
}//End of ioSTART(void)

//---------------------------------------------------------------------------------
//Stop
//---------------------------------------------------------------------------------
void ioSTOP(void)
{
	CLR_IO_SCL;
	CLR_IO_SDA;
	SET_IO_SCL;
	SET_IO_SDA;
}//End of ioSTOP(void)

//---------------------------------------------------------------------------------
//Wait 
//---------------------------------------------------------------------------------
void ioCSS_Wait(U16 tt)
{
	while(tt > 0)
	{
		tt--;
	}
}//End of ioCSS_Wait(U16 tt)

//---------------------------------------------------------------------------------
//Check ACK
//---------------------------------------------------------------------------------
U8 ioCheckAck(void)
{
	//:D	U8 i;
	SET_IO_SDA; 
	//ioCSS_Wait(10);
	ioCSS_Wait(4);
	SET_IO_SCL;
	//ioCSS_Wait(10);  
	ioCSS_Wait(2); 
	//***:D	i = 0;	
	//***:D	while((ioSDA) && (i < 250)) i++;
	if(ioSDA) IOEXP_ERR = 1;					//i > 249) IOEXP_ERR = 1;
	else IOEXP_ERR = 0;
	IOEXP_ERR = !ioSDA;				//:Dennis,M
	CLR_IO_SCL;
	ioCSS_Wait(2);
	return 1;
}//End of U8 ioCheckAck(void)


/***
//~D,A
//---------------------------------------------------------------------------------
//No ACK
//---------------------------------------------------------------------------------
void NoAckIO() 
{
	SET_IO_SDA;
	SET_IO_SCL;
	CLR_IO_SCL;
}//End of NoAck()
***/


//---------------------------------------------------------------------------------
//Write Byte
//---------------------------------------------------------------------------------
void ioWriteByte(U8 I2cData)
{
	U8 Bits;
	
	for(Bits = 0; Bits < 8; Bits++)
	{
		if(I2cData & 0x80)
		{
			SET_IO_SDA;
		}
		else
		{
			CLR_IO_SDA;
		}
		SET_IO_SCL;
		ioCSS_Wait(2);
		CLR_IO_SCL;
		ioCSS_Wait(2);		
		I2cData <<= 1;
	}
}//End of ioWriteByte(U8 I2cData)



//~DA,240809,
//:Dennis, ioWritePorts
////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------
//Write all three Ports
//---------------------------------------------------------------------------------
void ioWrite3Ports(U8 *I2cDataPorts)
{
	U8 Bits;
	U8 index,cc;
	
	index = 0; 	//3Ports: enforce to 0, no matter what index is come in.
	cc = 0;
	for(Bits = 0; Bits < 24; Bits++)
	{
		if(*(I2cDataPorts + index) & 0x80)
		{
			SET_IO_SDA;
		}
		else
		{
			CLR_IO_SDA;
		}
		SET_IO_SCL;
		ioCSS_Wait(5);
		CLR_IO_SCL;
		*(I2cDataPorts + index) <<= 1;
		cc++;
		if (cc >= 8) {
			ioCheckAck();			//:D,Assert ACK			
			cc = 0;
			index ++;		 	
	  }
	}
}//End of ioWrite3Ports(U8 *I2cDataPorts)



//~D,A
//---------------------------------------------------------------------------------
//Write all three Ports
//---------------------------------------------------------------------------------
void ioWrite2Ports(U8 *I2cDataPorts, U8 index)
{
	U8 Bits;
	//U8 index,cc;
	U8 cc;
	
	//*** index = 1;			//skip P0,Start from P1
	if (index > 1)
		return;					//error
	//
	cc = 0;
	for(Bits = 0; Bits < 16; Bits++)
	{
		if(*(I2cDataPorts + index) & 0x80)
		{
			SET_IO_SDA;
		}
		else
		{
			CLR_IO_SDA;
		}
		SET_IO_SCL;
		ioCSS_Wait(5);
		CLR_IO_SCL;
		*(I2cDataPorts + index) <<= 1;
		cc++;
		if (cc >= 8) {
			ioCheckAck();			//:D,Assert ACK
			cc = 0;
			index ++;		 	
	  }
	}
}//End of ioWrite2Ports(U8 *I2cDataPorts)


//~D,A
//---------------------------------------------------------------------------------
//Write all three Ports
//---------------------------------------------------------------------------------
void ioWrite1Ports(U8 *I2cDataPorts, U8 index)
{
	U8 Bits;
	//U8 index,cc;
	U8 cc;
	
	//
	//index = 1;			//skip P0,Start from P1
	if (index > 2)
	  return;				//error
	//  
	cc = 0;
	for(Bits = 0; Bits < 8; Bits++)
	{
		if(*(I2cDataPorts + index) & 0x80)
		{
			SET_IO_SDA;
		}
		else
		{
			CLR_IO_SDA;
		}
		SET_IO_SCL;
		ioCSS_Wait(5);
		CLR_IO_SCL;
		*(I2cDataPorts + index) <<= 1;
		cc++;
		if (cc >= 8) {
			ioCheckAck();			//:D,Assert ACK
			cc = 0;
			index ++;		 	
	  }
	}
}//End of ioWrite2Ports(U8 *I2cDataPorts)


//---------------------------------------------------------------------------------
//Write three Ports
//---------------------------------------------------------------------------------
void ioWriteThreePorts(U8 expIOAddr, U8 IICSUB_Address, U8 *I2cDataPorts)
{
	ioSTART();
	ioWriteByte(expIOAddr);		//~DA,240807
	if(ioCheckAck()) ioWriteByte(IICSUB_Address);			//subADDR, cmd
	if(ioCheckAck()) ioWrite3Ports(I2cDataPorts);		//datas 3 ports, index=0
	if(ioCheckAck()) ioSTOP();
//*** */	delay10us(500);				//5ms(500)
}//End of ioWriteThreePorts(U8 IIC_Address, U8 cmd, U8 *I2cDataPorts)


//~D,A
//---------------------------------------------------------------------------------
//Write two Ports
//---------------------------------------------------------------------------------
void ioWriteTwoPorts(U8 expIOAddr, U8 IICSUB_Address, U8 *I2cDataPorts, U8 index)
{
	
	ioSTART();
	ioWriteByte(expIOAddr);		//~DA,240807
	if(ioCheckAck()) ioWriteByte(IICSUB_Address);		//subADDR, cmd
	if(ioCheckAck()) ioWrite2Ports(I2cDataPorts, index);		//datas, 2 ports
	if(ioCheckAck()) ioSTOP();
//*** */	delay10us(500);				//5ms(500)
}//End of ioWriteTwoPorts(U8 IIC_Address, U8 cmd, U8 *I2cDataPorts)


//~DA,240809,Dennis
//---------------------------------------------------------------------------------
//Write One Ports
//---------------------------------------------------------------------------------
void ioWriteOnePorts(U8 expIOAddr, U8 IICSUB_Address, U8 *I2cDataPorts, U8 index)
{
	
	ioSTART();
	ioWriteByte(expIOAddr);		//~DA,240807
	if(ioCheckAck()) ioWriteByte(IICSUB_Address);		//subADDR, cmd
	if(ioCheckAck()) ioWrite1Ports(I2cDataPorts, index);		//datas, 2 ports
	if(ioCheckAck()) ioSTOP();
//*** */	delay10us(500);				//5ms(500)
}//End of ioWriteOnePorts(U8 IIC_Address, U8 cmd, U8 *I2cDataPorts)


//~DA,240807
//---------------------------------------------------------------------------------
// init IOPorts - :Dennis
//   expICNum: IOEXP_FUNC, IOEXP_SRC
//---------------------------------------------------------------------------------
void initIOPorts(U8 expICNum)
{
	U8 expIOAddr;
	U8 IOPORTS_DAT[2][3];		//~DA,240813	

	if (expICNum > 1)
	  return;				//NA
	if (expICNum == 0)
	  expIOAddr = IOEXP1_ADDR;
	else
	  expIOAddr = IOEXP2_ADDR;

	bXIOLock = High;		//lock


	#if 0 //~DA,240918, moved to below
		//[54: Interrupt mask]: 1=INT is masked, 0=active
		IOPORTS_DAT[expICNum][0] = 0xFF;
		IOPORTS_DAT[expICNum][1] = 0xFF;
		IOPORTS_DAT[expICNum][2] = 0xFF;	
		ioWriteThreePorts(expIOAddr, 0x54, &IOPORTS_DAT[expICNum][0]);	
		
		//[68: Interrupt clear]: 1=INT is clear
		IOPORTS_DAT[expICNum][0] = 0xFF;
		IOPORTS_DAT[expICNum][1] = 0xFF;
		IOPORTS_DAT[expICNum][2] = 0xFF;	
		ioWriteThreePorts(expIOAddr, 0x68, &IOPORTS_DAT[expICNum][0]);
	#endif

	//[5C:Output Port Configure via Port]: 0=Push pull, 1=Open drain
	ioWriteOneByte(expIOAddr, 0x5C, 0x00);			//Port0,1,2 = all Push pull 	

	#if 0
	//~DA,240918		
		//[70:Output Port Configure via PINs]: 0=Push pull, 1=Open drain
		IOPORTS_DAT[expICNum][0] = 0x00;	//*** 0x1F;				//0x00; -- 0x0C set 0 -->0x80 set to reverse to Opendrain
		IOPORTS_DAT[expICNum][1] = 0x00;
		IOPORTS_DAT[expICNum][2] = 0x00;	
		ioWriteThreePorts(expIOAddr, 0x70, &IOPORTS_DAT[expICNum][0]);
	#endif	

	//[0C:Configuration]: p1,p2=(0=all output), p0=(00001111=0x0F)
	IOPORTS_DAT[expICNum][0] = 0x00;	//*** 0x0F;
	IOPORTS_DAT[expICNum][1] = 0x00;
	IOPORTS_DAT[expICNum][2] = 0x00;	
	ioWriteThreePorts(expIOAddr, 0x0C, &IOPORTS_DAT[expICNum][0]);

	/***
		//[4C:Configuration]: set 1 to enable internal pull up/down
		IOPORTS_DAT[expICNum][0] = 0x00;	//0xFF;
		IOPORTS_DAT[expICNum][1] = 0x00;	//0xFF;
		IOPORTS_DAT[expICNum][2] = 0x00;	//0xFF;	
		ioWriteThreePorts(expIOAddr, 0x4C, &IOPORTS_DAT[expICNum][0]);

		//[50:Configuration]: set pull up=1/down=0, win above 0x4C
		IOPORTS_DAT[expICNum][0] = 0xFF;
		IOPORTS_DAT[expICNum][1] = 0xFF;
		IOPORTS_DAT[expICNum][2] = 0xFF;	
		ioWriteThreePorts(expIOAddr, 0x50, &IOPORTS_DAT[expICNum][0]);	
	***/

	//:	
	if (toInit)
	{	
		//~DA,240812,
		//Init IO ports
		switch(expICNum)
		{
			case 0: 
				//Funcs
				//***240912				delay10us(200);				//2ms
				IOPORTS_DAT[0][0] = 0x0B;	//STB_CTL=OFF(0x08),MUTE_CTL=OFF(0x00),SPB_CTL=OFF(0x02), SPA_CTL=OFF(0x01): 0x0B
				IOPORTS_DAT[0][1] = 0xDE;  //HICUT_CTL=OFF(0x80),LOWCUT_CTL_OFF(0x40),LOUD_CTL=OFF(0x00),STECHO_CTL=OFF(0x10),ECHO_CTL=OFF)(0x08),REV_CTL=OFF(0x04),ANTI_CTL=OFF(0x02): 0xDE 
				IOPORTS_DAT[0][2] = 0x00;  //SPA_LED(0x00),SPB_LED(0x00),STECHO_LED(0x00),ECHO_LED(0x00),REV_LED(0x00),HICUT_LED(0x00),LOWCUT_LED(0x00),ANTI_LED(0x00): 0x00
				ioWriteThreePorts(expIOAddr, 0x04, &IOPORTS_DAT[expICNum][0]);
				break;
			case 1:
				//Sources
				//***240912				delay10us(200);				//2ms
				IOPORTS_DAT[1][0] = 0x00;	//BT_SPDIF_CTL(0x00),DISP_CTL(0x00): 0x00
				IOPORTS_DAT[1][1] = 0x20;	//STB_LED(0x20),CH_SEL(0x00),PROT_RST(0x00): 0x20  
				IOPORTS_DAT[1][2] = 0x00;	//KTV_LED(0x00),DVD_LED(0x00),AUX_LED(0x00),BT_LED(0x00),COAX_LED(0x00),OPT1_LED(0x00),OPT2_LED(0x00),LOUD_LED(0x00): 0x00
				ioWriteThreePorts(expIOAddr, 0x04, &IOPORTS_DAT[expICNum][0]);
				break;
			default:
				break;
		}
	}


#if 1 //~DA,240918, moved here
	//[54: Interrupt mask]: 1=INT is masked, 0=active
	IOPORTS_DAT[expICNum][0] = 0xFF;
	IOPORTS_DAT[expICNum][1] = 0xFF;
	IOPORTS_DAT[expICNum][2] = 0xFF;	
	ioWriteThreePorts(expIOAddr, 0x54, &IOPORTS_DAT[expICNum][0]);	
	
	//[68: Interrupt clear]: 1=INT is clear
	IOPORTS_DAT[expICNum][0] = 0xFF;
	IOPORTS_DAT[expICNum][1] = 0xFF;
	IOPORTS_DAT[expICNum][2] = 0xFF;	
	ioWriteThreePorts(expIOAddr, 0x68, &IOPORTS_DAT[expICNum][0]);
#endif

	bXIOLock = Low;			//released
	//
	//***240912		delay10us(500);				//5ms
}



////////////////////////////////////////////////////////////////////////////////////////////////////////

/***
//~D,A
//---------------------------------------------------------------------------------
//READ Byte
//---------------------------------------------------------------------------------
U8 ioReadByte(void)
{
	U8  I2cData = 0, Bits;
	for(Bits = 0; Bits < 8; Bits++)
	{
		SET_IO_SCL;
		I2cData = I2cData << 1;
		I2cData = I2cData | ((unsigned char)(ioSDA));
		CLR_IO_SCL;
	}
	return I2cData;
}//End of U8 ioReadByte(void)
***/


//---------------------------------------------------------------------------------
//Write Data Function
//---------------------------------------------------------------------------------
void ioWriteOneByte(U8 expIOAddr, U8 IICSUB_Address, U8 IIC_Data)
{
	ioSTART();
	//*** ioWriteByte(ADDR_PCAL6524_Write);
	ioWriteByte(expIOAddr);
	if(ioCheckAck()) ioWriteByte(IICSUB_Address);
	if(ioCheckAck()) ioWriteByte(IIC_Data);
	if(ioCheckAck()) ioSTOP();
//*** */ 	delay10us(500);				//5ms	
}//End of  ioWriteOneByte(U8 IIC_Address, U8 cmd, U8 IIC_Data)

/***
//~D,A
//---------------------------------------------------------------------------------
//READ DATA FUNCTION
//---------------------------------------------------------------------------------
U8 ioReadOneByte(U8 expIOAddr, U8 IICSUB_Address)
{
	U8 x_Read_iic_Data;
	ioSTART();
	//*** ioWriteByte(ADDR_PCAL6524_Write);
	ioWriteByte(expIOAddr);
	if(ioCheckAck()) ioWriteByte(IICSUB_Address);	
	if(ioCheckAck()) ioSTART();
	//*** ioWriteByte(ADDR_PCAL6524_Read);
	ioWriteByte(expIOAddr+1);
	if(ioCheckAck()) x_Read_iic_Data = ioReadByte();
	NoAckIO();
	ioSTOP();
	// delay10us(500);				//5ms
	return x_Read_iic_Data;
}//End of ioReadOneByte
***/

