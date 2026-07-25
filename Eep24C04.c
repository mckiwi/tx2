#include "REG_MPC82G516.H"
#include "userdefine.h"

#define SET_IIC_SCL  IIC_SCL = 1
#define CLR_IIC_SCL  IIC_SCL = 0
#define SET_IIC_SDA  IIC_SDA = 1
#define CLR_IIC_SDA  IIC_SDA = 0

#define   IIC_SDA_OUT IIC_SDA = 0	  
#define   IIC_SDA_IN  IIC_SDA = 1

#define EEPROM_24c04_Write  0xa0
#define EEPROM_24c04_Read   0xa1

//:Dennis, Migrated
#define CT7302_Write        0x20
#define CT7302_Read         0x21
	  
void TDF24c04WriteOneByte(U8 IIC_Address, U8 IIC_Data);
U8 TDF24c04ReadOneByte(U8 IIC_Address);

void iicSTART(void);
void iicSTOP(void);
void I2CSS_Wait(U16 tt);
void iicAck(U8 ans);
U8 iicCheckAck(void);
void iicWriteByte(U8 I2cData);
U8 iicReadByte(void);
void SendStr(U8 IIC_Address, U8 IIC_Data);
void ReadStr(U8 IIC_Address, U8 IIC_Data);

//:Dennis, Migrated
void CT_7302_WriteOneByte(U8 IIC_Address, U8 IIC_Data);
void CT_7302_WriteTwoByte(U8 IIC_Address, U8 IIC_Data1, U8 IIC_Data2);
U8 CT_7302_ReadOneByte(U8 IIC_Address);


//---------------------------------------------------------------------------------
//Start 
//---------------------------------------------------------------------------------
void iicSTART(void)
{
	SET_IIC_SDA;
	SET_IIC_SCL;
	I2CSS_Wait(2);	//~DA,240813,added!	
	CLR_IIC_SDA;
	I2CSS_Wait(2);	//~DA,240813,added!	
	CLR_IIC_SCL;	
}//End of iicSTART(void)

//---------------------------------------------------------------------------------
//Stop
//---------------------------------------------------------------------------------
void iicSTOP(void)
{
	CLR_IIC_SCL;
	CLR_IIC_SDA;
	I2CSS_Wait(2);	//~DA,240813,added!		
	SET_IIC_SCL;
	I2CSS_Wait(2);	//~DA,240813,added!	
	SET_IIC_SDA;
}//End of iicSTOP(void)

//---------------------------------------------------------------------------------
//Wait 
//---------------------------------------------------------------------------------
void I2CSS_Wait(U16 tt)
{
	while(tt > 0)
	{
		tt--;
	}
}//End of I2CSS_Wait(U16 tt)

//---------------------------------------------------------------------------------
//Check ACK
//---------------------------------------------------------------------------------
U8 iicCheckAck(void)
{
	U8 i = 0;
	SET_IIC_SDA; 
	I2CSS_Wait(10);
	SET_IIC_SCL;
	I2CSS_Wait(10);        
	while((IIC_SDA) && (i < 250)) i++;
//	while(IIC_SDA) errtime--;
	CLR_IIC_SCL;
	I2CSS_Wait(2);
	return 1;
}//End of U8 iicCheckAck(void)

//---------------------------------------------------------------------------------
//No ACK
//---------------------------------------------------------------------------------

void NoAck() 
{
	SET_IIC_SDA;
	SET_IIC_SCL;
	//*** I2CSS_Wait(2);	//~DA,240813,added!
	CLR_IIC_SCL;
	//*** I2CSS_Wait(2);	//~DA,240813,added!	
}//End of NoAck()

//---------------------------------------------------------------------------------
//Write Byte
//---------------------------------------------------------------------------------
void iicWriteByte(U8 I2cData)
{
	U8 Bits;
	
	for(Bits = 0; Bits < 8; Bits++)
	{
		if(I2cData & 0x80)
		{
			SET_IIC_SDA;
		}
		else
		{
			CLR_IIC_SDA;
		}
		SET_IIC_SCL;
		I2CSS_Wait(5);
		//*** I2CSS_Wait(2);	//~DA,240813	
		CLR_IIC_SCL;
		//*** I2CSS_Wait(2);	//~DA,240813,added!		
		I2cData <<= 1;
	}
}//End of iicWriteByte(U8 I2cData)

//---------------------------------------------------------------------------------
//READ Byte
//---------------------------------------------------------------------------------
U8 iicReadByte(void)
{
	U8  I2cData = 0, Bits;
	for(Bits = 0; Bits < 8; Bits++)
	{
		SET_IIC_SCL;
		I2cData = I2cData << 1;
		I2cData = I2cData | ((unsigned char)(IIC_SDA));
		//*** I2CSS_Wait(2);	//~DA,240813,added!
		CLR_IIC_SCL;
		//*** I2CSS_Wait(2);	//~DA,240813,added!		
	}
	return I2cData;
}//End of U8 iicReadByte(void)

//---------------------------------------------------------------------------------
//Write Data Function
//---------------------------------------------------------------------------------
void TDF24c04WriteOneByte(U8 IIC_Address, U8 IIC_Data)
{
	iicSTART();
	iicWriteByte(EEPROM_24c04_Write);
	if(iicCheckAck()) iicWriteByte(IIC_Address);
	if(iicCheckAck()) iicWriteByte(IIC_Data);
	if(iicCheckAck()) iicSTOP();
	T2delay1ms(8);//Write Cycle Time Must > 5ms
}//End of TDF24c04WriteOneByte(U8 Device_Address, U8 IIC_Address, U8 IIC_Data)

//---------------------------------------------------------------------------------
//READ DATA FUNCTION
//---------------------------------------------------------------------------------
U8 TDF24c04ReadOneByte(U8 IIC_Address)
{
	U8 x_Read_iic_Data;
	iicSTART();
	iicWriteByte(EEPROM_24c04_Write);
	if(iicCheckAck()) iicWriteByte(IIC_Address);
	if(iicCheckAck()) iicSTART();
	iicWriteByte(EEPROM_24c04_Read);
	if(iicCheckAck()) x_Read_iic_Data = iicReadByte();
	NoAck();
	iicSTOP();
	return x_Read_iic_Data;
//	T2delay1ms(8);//Write Cycle Time > 5ms
}//End of TDF24c04ReadOneByte



//:Dennis, Migrated ----------------------------------------------------------------
void CT_7302_WriteOneByte(U8 IIC_Index_Code, U8 IIC_Data)
{
	iicSTART();
	iicWriteByte(CT7302_Write);
	if(iicCheckAck()) iicWriteByte(IIC_Index_Code);
	if(iicCheckAck()) iicWriteByte(IIC_Data);
	if(iicCheckAck()) iicSTOP();
	T2delay1ms(8);//Write Cycle Time Must > 5ms
}//End of TDF24c04WriteOneByte(U8 Device_Address, U8 IIC_Address, U8 IIC_Data)

/*
void CT_7302_WriteTwoByte(U8 IIC_Index_Code, U8 IIC_Data1, U8 IIC_Data2)
{
	iicSTART();
	iicWriteByte(CT7301C_Write);
	if(iicCheckAck()) iicWriteByte(IIC_Index_Code);
	if(iicCheckAck()) iicWriteByte(IIC_Data1);
	if(iicCheckAck()) iicWriteByte(IIC_Data2);
	if(iicCheckAck()) iicSTOP();
//	T2delay1ms(8);//Write Cycle Time Must > 5ms
}//End of TDF24c04WriteOneByte(U8 Device_Address, U8 IIC_Address, U8 IIC_Data)
*/

/*
U8 CT_7302_ReadOneByte(U8 IIC_Address)
{
	U8 x_Read_iic_Data;
	iicSTART();
	iicWriteByte(CT7301C_Write);
	if(iicCheckAck()) iicWriteByte(IIC_Address);
	if(iicCheckAck()) iicSTART();
	iicWriteByte(CT7301C_Read);
	if(iicCheckAck()) x_Read_iic_Data = iicReadByte();
	NoAck();
	iicSTOP();
	return x_Read_iic_Data;
//	T2delay1ms(8);//Write Cycle Time > 5ms
}//End of TDF24c04ReadOneByte
*/
//:------------------------------------------------------------------------------------



//__EOF

