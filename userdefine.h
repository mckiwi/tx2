#include "MPC82.H"   //暫存器及組態定義

typedef unsigned int   U16;
typedef unsigned char  U8;

//********** 18.432MHz  *******************************************************

#define Frequency0 5085//5485//8533//5120//5650//4960//9920//10K = 0.1ms
//#define Frequency1 1000//1536//992//1K = 1ms
#define Frequency2 1536//922//(18432000/(32*38400))//460//991//922//
#define T  (18432000/12/2)//(18432000/12/2)//基本頻率18.432MHz, T = 768000
#define Xtal_Freq  18432000
#define Baud_Rate  19200
#define t0 (65536-(T/Frequency0))
//#define t1 (65536-(T/Frequency1))

//*****************************************************************************


//********** 11.0592MHz  ******************************************************
/*
#define Frequency0 5120//5485//5120//5650//4960//9920//10K = 0.1ms(90)
#define Frequency1 992//1000//992//1K = 1ms
#define Frequency2 922//922//460//991//922//
#define T  (11059200/12/2)//(11059200/12/2)//基本頻率11.0592MHz, T = 460800
#define t0 (65536-(T/Frequency0))
//#define t1 (65536-(T/Frequency1))
#define Xtal_Freq  11059200
#define Baud_Rate  19200//最好設定為 19200 ,其錯誤率為 0%
//#define t2 (65536-Frequency2)
//#define  Baud_rate bps = 9600
*/
//*****************************************************************************


//Global
#define MUTE_RAMPUP 	1		//1=to Support: Mute Relase Volume Ramp up


//遙控器接收部份的宣告
#define IR_START(x)			    ((x)>130 && (x)<140)//135
#define BIT_1(x)				((x)>18)//  && (x)<50)//27 )//23   (x)>18
#define BIT_0(x)				((x)<19)//  && (x)>5)//12    (x)<19
#define IR_REPEAT(x)			((x)>108 && (x)<116)//112
#define Ticks_error(x)			((x)>180)

#define WAITING_STATE   0
#define WAIT_NEXT_INT   1
#define GET_CUSTOM      2
#define GET_CUSTOM_BAR  3
#define GET_DATA        4
#define GET_DATA_BAR    5

#define High  1
#define Low   0

//=================================================================================================
//  此版接腳宣告 For PN:TX1MCU01C 修改後的版本
//  PCB 改成 PN:TX1MCU01D 時,接腳要注意是否有變動,切記要修改宣告.
//=================================================================================================

//:Dennis, check to change for TX2?
//P0_0 ~ P0_3 Music Mic Encoder
sbit	EN1				=	P0^0;	//music encoder
sbit	EN2				=	P0^1; 
sbit	DP1A1			=	P0^2;	//2th digit, music
sbit	DP1A2			=	P0^3;	//1st digit, music
sbit	SRCLR_DP1		=	P0^4;	//clear shift
sbit	SRCLK_DP1		=	P0^5;	//clk
sbit	RCLK_DP1		=	P0^6;	//latch 
sbit    SER_DP1			=   P0^7;	//data

sbit	DP2A2			=	P1^0;	//1st digit, mic
sbit	SRCLR_DP2		=	P1^1;
sbit	SRCLK_DP2		=	P1^2;
sbit	RCLK_DP2		=	P1^3;
sbit	SER_DP2			=   P1^4;
sbit	EVIC_ST			=   P1^5;

sbit	EN3				=   P1^6;	//mic encoder
sbit	EN4				=   P1^7;

sbit	COL0			=	P2^0;	//(i) kbscan
sbit	COL1			=	P2^1;
sbit	COL2			=	P2^2;
sbit	COL3			=	P2^3;
sbit	ROW0			=	P2^4;	//(o) kbscan
sbit	ROW1			=	P2^5;
sbit	ROW2			=	P2^6;
sbit    ROW3			=   P2^7;

//sbit	RXD				=	P3^0;
//sbit	TXD				=	P3^1;
sbit	REMOTE			=	P3^2;
sbit	SDA_MCU			=	P3^3;	//i2c, ioEXP#1,2
sbit	SCL_MCU			=	P3^4;
sbit	RST_MCU			=	P3^5;
sbit	STB_SW			=	P3^6;
//sbit  NC				=   P3^7;

//sbit	NC				=	P4^0;//DP2A2			=	P4^0;
//sbit  MC				=   P4^1;
sbit    DP2A1			=   P4^2;	//2nd digit, mic
//sbit	NC				=	P4^3;
sbit    IIC_SCL			=   P4^4;	//i2c, eeprom	
sbit    IIC_SDA			=   P4^5;
sbit    KEY_Data		=   P4^6;	//#bkey dat
sbit    KEY_SCLK		=   P4^7;	//#bkey clk


//*** sbit PWM0 = 0x92;  //PWM0=P12
//*** sbit PWM1 = 0x93;  //PWM1=P13

//~D,A //////
//:Dennis, new IO expander: for PCAL6524_NXP
/////////////////////////////////////////////////
sbit ioRESET			= P3^5; 	//pin39,  io RESET (RST_MCU)
sbit ioSCL				= P3^4; 	//pin38,	io SCL	(SCL_MCU)
sbit ioSDA 				= P3^3; 	//pin37,  io SDA	(SDA_MCU)
/////////////////////////////////////////////////


/***
 * Orig,
//:Dennis, check to change for TX2?
//P0_0 ~ P0_3 Music Mic Encoder
sbit	STB_SW			=	P0^4;
sbit	KeyUP			=	P0^5;
sbit	KeyReset		=	P0^6;//FAN_CTL			=	P0^6;//KeyReset		=	P0^6;
sbit    KeyDN			=   P0^7;

sbit	DP2A1			=	P1^0;//KEY_SCLK		=	P1^0;//DI
sbit	IIC_SCL			=	P1^1;//SK
sbit	Dimmer			=	P1^2;
sbit	RGB_Dimmer		=	P1^3;
sbit	IIC_SDA			=   P1^4;//EROM_CS			=   P1^4;//CS
sbit	PCLK3			=   P1^5;
sbit	PCLK1			=   P1^6;
sbit	PCLK2			=   P1^7;

sbit	COL0			=	P2^0;
sbit	COL1			=	P2^1;
sbit	COL2			=	P2^2;
sbit	COL3			=	P2^3;
sbit	ROW0			=	P2^4;
sbit	ROW1			=	P2^5;
sbit	ROW2			=	P2^6;
sbit    ROW3			=   P2^7;

//sbit	RXD				=	P3^0;
//sbit  TXD				=   P3^1;
//sbit    Remote			=   P3^2;
//**sbit	Din0			=	P3^3;
//**sbit    Din1			=   P3^4;
//**sbit    Din2			=   P3^5;
//**sbit    OE				=   P3^6;//KEYST			=   P3^6;
sbit    SCLK			=   P3^7;//EVIC_ST			=   P3^7;

sbit	KEYST			=	P4^0;//DP2A2			=	P4^0;
sbit    DP1A2			=   P4^1;
sbit    DP1A1			=   P4^2;//KEY_Data		=   P4^2;//EROM_SI			=   P4^2;//DO
sbit	Key_Check		=	P4^3;
sbit    EVIC_ST			=   P4^4;//OE				=   P4^4;
sbit    DP2A2			=   P4^5;
sbit    KEY_Data		=   P4^6;//DP1A1			=   P4^6;
sbit    KEY_SCLK		=   P4^7;//DP2A1			=   P4^7;


sbit PWM0 = 0x92;  //PWM0=P12
sbit PWM1 = 0x93;  //PWM1=P13
***/
	

extern int  x1ms;
extern char INPUT_Function;

extern unsigned char	IR_STATE, RepeatFlag, NewKeyFlag, Ticks, BitCount, Remote_Flag, Remote_Key_Flag;
extern unsigned char	TempData, Custom_Code, Data_Code, Custom_Code_BAR, Data_Code_BAR;
extern unsigned char	x55ms_counter, x220ms_counter;

extern unsigned char	SP_Function, LOUD_Function, Mute_Function, WUTA_CTL, ANTI_CTL, REV_CTL;				
extern unsigned char	ANTI_Function, LOWCUT_Function, HICUT_Function, REV_Function, ECHO_Function, STECHO_Function, HICUT_CTL, ECHO_CTL; 
extern unsigned char	MUTE_CTL, Limit_Music, Limit_Mic, VR_Auto_Flag, STECHO_CTL, PWM_Dimmer;
extern unsigned char	Limit_Flag, Power_ON_Flag, ASONG_Function, Last_Music_Volume_Counter;
extern unsigned char	LOUD_CTL, CH_SEL, Music_Limit, Mic_Limit, Preset_Music, Preset_Mic, AUTO_CTL, WUTA_Function;
extern unsigned char	Power_BlueLED_Flash_Flag, Power_RedLED_Flash_Flag, Power_GreenLED_Flash_Flag;
extern unsigned char	SPA_LED, SPB_LED, LOUD_LED, CH_LED, STB_LED, SPA_CTL, SPB_CTL, DISP_CTL, Volume_Action_Flag, RGB_PWM_Dimmer;
extern unsigned char	ANTI_LED, LOWCUT_LED, HICUT_LED, REV_LED, ECHO_LED, STECHO_LED, WUTA_LED, AUTO_LED, MUTE_LED;
extern unsigned char	RS232_Custom_Code, RS232_Custom_Code_BAR, RS232_Data_Code, RS232_Data_Code_BAR, LOWCUT_CTL, STB_CTL;
extern unsigned char	Character_units, Character_tens, Music_tens, Music_units, Mic_tens, Mic_units;
extern unsigned char	Music_Volume_Counter, Mic_Volume_Counter, FAN_CTL;
extern unsigned char	Music_UP_Flag, Mic_UP_Flag, Music_DN_Flag, Mic_DN_Flag, Turbo_Volume_Counter, Character;
extern unsigned char	BT_SPDIF_CTL;				//:Dennis
extern unsigned char	SPDIF_SOURCE;				//:Dennis

//~DA,240807
extern unsigned char 	IOEXP_ERR;
extern volatile U8 IOPORTS_DATBK[2][3];
extern bit isBoot;
extern volatile bit bXIOLock;	//~DA,240813
extern bit toInit;				//~DA,240918

//
extern data U16 gw_TimerBase;
extern unsigned char UART_STATUS; //0:Standby, 1:Command input, 2:Command ready.
extern U8 data gc_IrPowerOnOff;
extern U8  gc_MainRotateOldKey;			//~DA
extern U8  gc_MicRotateOldKey;			//~DA

extern void ScanKeySwitch(void);

extern void TDF24c04WriteOneByte(U8 IIC_Address, U8 IIC_Data);
extern U8 TDF24c04ReadOneByte(U8 IIC_Address);

extern void NJWDelay(U8 kk);
extern void main(void);
extern void ECHO_Action(void);
extern void WUTA_Action(void);
extern void ASONG_Action(void);
extern void Loud_Action(void);
extern void Mute_Action(void);
extern void INPUT_Action(void);
extern void HICUT_Action(void);
extern void STECHO_Action(void);
extern void LOWCUT_Action(void);
extern void ANTI_Action(void);
extern void REV_Action(void);
extern void Limit_VR_Set_Action(void);
extern void HICUT_Action(void);
extern void LOWCUT_Action(void);

//extern void Set_Power_ON_VR_Preset_Action(void);
extern void SendStr(U8 IIC_Address,U8 IIC_Data);

//extern void AD7376_Music(U8 x_Music_Volume);
//extern void AD7376_Mic(U8 x_Mic_Volume);

extern void NJW1159SetData(U8 x_Volume, U8 x_Channel);
extern void NJW1159VolumeControlSet(U16 Volume_Temp);
extern void NJW1159Initial(void);

extern void Music_Volume_UP_Action(void);
extern void Music_Volume_DN_Action(void);
extern void Mic_Volume_UP_Action(void);
extern void Mic_Volume_DN_Action(void);
extern void Mic_Volume_Action(void);
extern void GetFirstRotateKeyData(void);
extern void	MicVolumeGetRotateKey(void);
extern void	MainVolumeGetRotateKey(void);
extern void	MainMicRotateKeyProcess(void);
extern void Delay_ms(unsigned int dly); 
extern void UART_Variable_Reset(void);
extern void UART_init(void);  //UART啟始程式
extern void UART_main(void);
extern void UART_Send(U8 Send_Data);
extern unsigned char count, Scan_Key_Data, ROW;  //基數=0
//extern void M65847AFP_Action(void);

extern void SP_Action(void);
extern void ASONG_Action(void);
//*** ~DA,240809,	extern void PWM_Main(U8 PWM_Pulse,U8 RGB_PWM_Pulse);
extern void VR_Stop(void);
//*** extern void PCLK1_Action(void);//OE1_Action ()
//*** extern void PCLK2_Action(void);//OE2_Action ()
//*** extern void PCLK3_Action(void);//OE3_Action ()

//*** extern void Music_Display_7SEG_units(void);//個位數 units
//*** extern void Music_Display_7SEG_tens(void);//十位數 tens
//*** extern void Mic_Display_7SEG_units(void);//個位數 units
//*** extern void Mic_Display_7SEG_tens(void);//十位數 tens

extern void Call_default_Init(void);
extern void UPDNKey_Action(void);
extern void ScanUDKeySwitch(void);

extern void T2delay1ms(int count);	// 使用Timer2 MODE1的時間延遲函式 ( X'tal 用 11.0592MHz 實測值為 1.0ms )

extern unsigned char	RS232_First_Code, RS232_RW_Code, RS232_Addr_Code, RS232_Data_Code, RS232_Last_Code;

extern void Get_SKey(void);
extern void SKey_Preset_OK_Switch(void);
extern void SKey_Limit_OK_Switch(void);

extern void Sdataout(char Skeyout);
extern void Restore_Preset_Function(void);
extern void Cancel_Remote_Key(void);
extern void Limit_VR_Set(void);	//設定 VR 限制時的副程式
extern void Set_Power_ON_VR_Preset(void);	//設定 VR 開機時的按鍵掃描副程式
extern void Set_Power_ON_VR_Preset_Action(void);
//extern void keyzero_Action(void);


//SPDIF-SW
//:Dennis, Migrated
extern void CT_7302_WriteOneByte(U8 IIC_Address, U8 IIC_Data);
extern void CT_7302_WriteTwoByte(U8 IIC_Address, U8 IIC_Data1, U8 IIC_Data2);
extern U8 CT_7302_ReadOneByte(U8 IIC_Address);

//:Dennis,A
extern void DIRECTPower_Preset_Function(void);
extern void INPUT_Preset_Function(char isrc);	

//~DA,240807
extern void delay10us(U16 kk);
extern void initIOPorts(U8 expICNum);
extern void ioWriteThreePorts(U8 expIOAddr, U8 IICSUB_Address, U8 *I2cDataPorts);
extern void ioWriteTwoPorts(U8 expIOAddr, U8 IICSUB_Address, U8 *I2cDataPorts, U8 index);
extern void ioWriteOnePorts(U8 expIOAddr, U8 IICSUB_Address, U8 *I2cDataPorts, U8 index);
extern void PCLK_Action(U8 expICNum);		//~DA,240807
extern void ShortDelay(U8 kk);
extern void resetIOEXP(void);
extern void I2CSS_Wait(U16 tt);



//


//============================================================
#define  D_MainKeyDebounce        0x01//不需修改
#define  D_MainRotateKeySuccess   0x02//不需修改
#define  D_MicKeyDebounce         0x10//不需修改
#define  D_MicRotateKeySuccess    0x20//不需修改
//#define  D_Main
//============================================================

//============================================================
#define  NJW1159_CLK_HI      KEY_SCLK = 1
#define  NJW1159_CLK_LW      KEY_SCLK = 0
#define  NJW1159_SDA_HI      KEY_Data = 1
#define  NJW1159_SDA_LW      KEY_Data = 0
#define  NJW1159_LATCH_HI    EVIC_ST = 1
#define  NJW1159_LATCH_LOW   EVIC_ST = 0
//============================================================

/*
#define  AD7376_CLK_HI           KEY_SCLK = 1
#define  AD7376_CLK_LOW          KEY_SCLK = 0
#define  AD7376_SDA_HI           KEY_Data = 1
#define  AD7376_SDA_LOW          KEY_Data = 0
#define  AD7376_Music_CS_HI      AD7376_Music_CS = 1
#define  AD7376_Music_CS_LOW     AD7376_Music_CS = 0
#define  AD7376_Mic_CS_HI        AD7376_Mic_CS = 1
#define  AD7376_Mic_CS_LOW       AD7376_Mic_CS = 0
*/
#define x55ms_counter_Preset    55
#define x25ms_counter_Preset    25		//~DA,240919
#define x10ms_counter_Preset    10		//~DA,240808
#define x5ms_counter_Preset      5		//~DA,240808
#define x220ms_counter_Preset    4
#define x2sec_counter_Preset   9
#define Remote_Delay_Time      110//108
#define Max_Volume   95//95//95//52//95
#define Min_Volume   0
#define Music_Chennal   0x00
#define Mic_Chennal     0x01
#define Scan_Display_counter_Preset 20
#define Display_Flash_Preset    2
#define Turbo_Volume_Counter_Preset 5

//***************************************************************************************************
//0~255 0x00最暗(平均電壓最高), 0x40(大約25%), 0x80(大約50%), 0xa0(大約75%), 0xff最亮(平均電壓最低)
#define RGB_PWM_Dimmer_0	RGB_PWM_Dimmer = 0x00 //0~255 0x00最暗(平均電壓最高)
#define RGB_PWM_Dimmer_5	RGB_PWM_Dimmer = 0x0C //0~255 0x0C(大約5%)
#define RGB_PWM_Dimmer_10	RGB_PWM_Dimmer = 0x19 //0~255 0x19(大約10%)
#define RGB_PWM_Dimmer_15	RGB_PWM_Dimmer = 0x25 //0~255 0x25(大約15%)
#define RGB_PWM_Dimmer_20	RGB_PWM_Dimmer = 0x31 //0~255 0x31(大約20%)

#define RGB_PWM_Dimmer_25	RGB_PWM_Dimmer = 0x40 //0~255 0x40(大約25%)
#define RGB_PWM_Dimmer_30	RGB_PWM_Dimmer = 0x4C //0~255 0x4C(大約30%)
#define RGB_PWM_Dimmer_35	RGB_PWM_Dimmer = 0x59 //0~255 0x59(大約35%)
#define RGB_PWM_Dimmer_40	RGB_PWM_Dimmer = 0x65 //0~255 0x65(大約40%)
#define RGB_PWM_Dimmer_45	RGB_PWM_Dimmer = 0x71 //0~255 0x71(大約45%)

#define RGB_PWM_Dimmer_50	RGB_PWM_Dimmer = 0x80 //0~255 0x80(大約50%)
#define RGB_PWM_Dimmer_75	RGB_PWM_Dimmer = 0xa0 //0~255 0xa0(大約75%)
#define RGB_PWM_Dimmer_100	RGB_PWM_Dimmer = 0xff //0~255 0xff最亮(平均電壓最低)
//***************************************************************************************************
//0~255 0x00最暗(平均電壓最高), 0x40(大約25%), 0x80(大約50%), 0xa0(大約75%), 0xff最亮(平均電壓最低)
#define PWM_Dimmer_0	PWM_Dimmer = 0x00 //0~255 0x00最暗(平均電壓最高)
#define PWM_Dimmer_5	PWM_Dimmer = 0x0C //0~255 0x0C(大約5%)
#define PWM_Dimmer_10	PWM_Dimmer = 0x19 //0~255 0x19(大約10%)
#define PWM_Dimmer_15	PWM_Dimmer = 0x25 //0~255 0x25(大約15%)
#define PWM_Dimmer_20	PWM_Dimmer = 0x31 //0~255 0x31(大約20%)

#define PWM_Dimmer_25	PWM_Dimmer = 0x40 //0~255 0x40(大約25%)
#define PWM_Dimmer_30	PWM_Dimmer = 0x4C //0~255 0x4C(大約30%)
#define PWM_Dimmer_35	PWM_Dimmer = 0x59 //0~255 0x59(大約35%)
#define PWM_Dimmer_40	PWM_Dimmer = 0x65 //0~255 0x65(大約40%)
#define PWM_Dimmer_45	PWM_Dimmer = 0x71 //0~255 0x71(大約45%)

#define PWM_Dimmer_50	PWM_Dimmer = 0x80 //0~255 0x80(大約50%)
#define PWM_Dimmer_75	PWM_Dimmer = 0xa0 //0~255 0xa0(大約75%)
#define PWM_Dimmer_100	PWM_Dimmer = 0xff //0~255 0xff最亮(平均電壓最低)

//***************************************************************************************************
#if 0
	//orig
	#define KTV		  0
	#define DVD		  1
	#define VCR		  2
	#define AUX		  3
#endif
//:Dennis--- //TX2---
#define KTV		  0
#define DVD		  1
#define AUX		  2
#define BT		  3
#define COAX	  4	
#define OPT1	  5
#define OPT2	  6
//:---

//#define HD		  4

#define STECHO_OFF_Function  0
#define STECHO_ON_Function   1

#define ASONG_OFF_Function	 0
#define ASONG_ON_Function	 1

#define WUTA_OFF_Function	 0
#define WUTA_ON_Function	 1

#define ECHO_OFF_Function	 0
#define ECHO_ON_Function	 1

#define LOUD_OFF_Function	 0
#define LOUD_ON_Function	 1

#define HICUT_OFF_Function	 0
#define HICUT_ON_Function	 1

#define LOWCUT_OFF_Function	 0
#define LOWCUT_ON_Function	 1

#define ANTI_OFF_Function	 0
#define ANTI_ON_Function	 1

#define REV_OFF_Function	 0
#define REV_ON_Function	     1


//:Dennis,M --- //for TX2---
/***
#define CH_LED_OFF     CH_SEL = 0x00, CH_LED = 0x7f 	//-1111111
#define CH_KTV         CH_SEL = 0x00, BT_SPDIF_CTL = 0x00, CH_LED = 0x7e 	//-1111110
#define CH_DVD         CH_SEL = 0x01, BT_SPDIF_CTL = 0x00, CH_LED = 0x7d 	//-1111101
#define CH_AUX         CH_SEL = 0x02, BT_SPDIF_CTL = 0x00, CH_LED = 0x7b 	//-1111011
#define CH_BT		   CH_SEL = 0x03, BT_SPDIF_CTL = 0x00, CH_LED = 0x77  //-1110111 --BT_SPDIF = 0
#define CH_COAX        CH_SEL = 0x03, BT_SPDIF_CTL = 0x10, CH_LED = 0x6f  //-1101111 --BT_SPDIF = 1, mroe: +SPDIF SW for COAX/OPT1/OPT2
#define CH_OPT1        CH_SEL = 0x03, BT_SPDIF_CTL = 0x10, CH_LED = 0x5f	//-1011111    |+ SPDIF SW for COAX/OPT1/OPT2
#define CH_OPT2        CH_SEL = 0x03, BT_SPDIF_CTL = 0x10, CH_LED = 0x3f  //-0111111    |+ SPDIF SW for COAX/OPT1/OPT2
***/
//~DA,240807,
#define CH_LED_OFF		CH_SEL = 0x00, CH_LED = 0x00 	                    //0000000-
#define CH_KTV			CH_SEL = 0x00, BT_SPDIF_CTL = 0x00, CH_LED = 0x80 	//1000000-
#define CH_DVD			CH_SEL = 0x02, BT_SPDIF_CTL = 0x00, CH_LED = 0x40 	//0100000-
#define CH_AUX			CH_SEL = 0x04, BT_SPDIF_CTL = 0x00, CH_LED = 0x20 	//0010000-
#define CH_BT			CH_SEL = 0x06, BT_SPDIF_CTL = 0x00, CH_LED = 0x10   //0001000- --BT_SPDIF = 0
#define CH_COAX			CH_SEL = 0x06, BT_SPDIF_CTL = 0x80, CH_LED = 0x08   //0000100- --BT_SPDIF = 1, mroe: +SPDIF SW for COAX/OPT1/OPT2
#define CH_OPT1			CH_SEL = 0x06, BT_SPDIF_CTL = 0x80, CH_LED = 0x04	//0000010-    |+ SPDIF SW for COAX/OPT1/OPT2
#define CH_OPT2			CH_SEL = 0x06, BT_SPDIF_CTL = 0x80, CH_LED = 0x02   //0000001-    |+ SPDIF SW for COAX/OPT1/OPT2
//:---
	

#define FAN_HiSpeed  FAN_CTL = 0x00
#define FAN_LowSpeed FAN_CTL = 0x80

#define AB_OFF  0
#define	A_ON    1
#define	AB_ON   2
#define	B_ON    3

/*** Prev. version,
//:Dennis,---
#define	SPA_ON        SPA_CTL = 0x00, SPA_LED = 0x00
#define SPA_OFF       SPA_CTL = 0x40, SPA_LED = 0x01

#define	SPB_ON        SPB_CTL = 0x00, SPB_LED = 0x00
#define SPB_OFF       SPB_CTL = 0x80, SPB_LED = 0x02

#define	LOUD_ON       LOUD_CTL = 0x04, LOUD_LED = 0x00      
#define	LOUD_OFF      LOUD_CTL = 0x00, LOUD_LED = 0x80  		//:Dennis,M  

#define	MUTE_ON       MUTE_CTL = 0x40, MUTE_LED = 0x00			//:Dennis, Canceled! MUTE_LED 0x10-->0x00
#define	MUTE_OFF      MUTE_CTL = 0x00, MUTE_LED = 0x00	

#define	ASONG_ON      AUTO_CTL = 0x08, AUTO_LED = 0x00	
#define	ASONG_OFF     AUTO_CTL = 0x00, AUTO_LED = 0x00			//:Dennis, Calceled! AUTO_LED 0x01-->0x00

#define	WUTA_ON		  WUTA_CTL = 0x00, WUTA_LED = 0x00	
#define	WUTA_OFF	  WUTA_CTL = 0x20, WUTA_LED = 0x00			//:Dennis, Calceled! WUTA_LED 0x02-->0x00
  
#define	HICUT_ON      HICUT_CTL = 0x00, HICUT_LED = 0x00
#define	HICUT_OFF     HICUT_CTL = 0x01, HICUT_LED = 0x20

#define	LOWCUT_ON     LOWCUT_CTL = 0x00, LOWCUT_LED = 0x00
#define	LOWCUT_OFF    LOWCUT_CTL = 0x02, LOWCUT_LED = 0x40

#define	ANTI_ON       ANTI_CTL = 0x00, ANTI_LED = 0x00
#define	ANTI_OFF      ANTI_CTL = 0x40, ANTI_LED = 0x80
	
#define REV_ON        REV_CTL = 0x00, REV_LED = 0x00	
#define REV_OFF       REV_CTL = 0x20, REV_LED = 0x10
//:---

//#define Key_ON    KEY_CTL = 0x00
//#define Key_OFF   KEY_CTL = 0x10
	
#define	POWER_BlueLED_ON    STB_LED = 0x04//0x06//0x06//Blue
#define	POWER_GreenLED_ON   STB_LED = 0x02//0x03//Green
#define	POWER_RedLED_ON     STB_LED = 0x01//0x05//0x05//Red	
#define	POWER_LED_All_OFF   STB_LED = 0x00//0x07//LED OFF	
#define	POWER_LED_All_ON    STB_LED = 0x07//0x00//LED ON	

#define	Mute_OFF_Function  0
#define	Mute_ON_Function   1

#define STB_CTL_ON   STB_CTL = 0x00
#define STB_CTL_OFF  STB_CTL = 0x80

#define VU_ON    DISP_CTL = 0x08
#define VU_OFF   DISP_CTL = 0x00
***/

//~DA,240807,
//////////////////////////////////////////////////////////////////////////
#define	SPA_ON        SPA_LED = 0x80, SPA_CTL = 0x00
#define SPA_OFF       SPA_LED = 0x00, SPA_CTL = 0x01

#define	SPB_ON        SPB_LED = 0x40, SPB_CTL = 0x00
#define SPB_OFF       SPB_LED = 0x00, SPB_CTL = 0x02

#define	LOUD_ON       LOUD_LED = 0x01, LOUD_CTL = 0x20      
#define	LOUD_OFF      LOUD_LED = 0x00, LOUD_CTL = 0x00  	

//#define	MUTE_ON       MUTE_CTL = 0x40, MUTE_LED = 0x00			
//#define	MUTE_OFF      MUTE_CTL = 0x00, MUTE_LED = 0x00
//Dennis, Canceled! MUTE_LED
#define	MUTE_ON       MUTE_CTL = 0x04		
#define	MUTE_OFF      MUTE_CTL = 0x00

//#define	ASONG_ON      AUTO_CTL = 0x08, AUTO_LED = 0x00	
//#define	ASONG_OFF     AUTO_CTL = 0x00, AUTO_LED = 0x00			//:Dennis, Calceled! AUTO_LED 0x01-->0x00
//:Dennis, Calceled! AUTO_LED
#define	ASONG_ON      AUTO_CTL = 0x08, AUTO_LED = 0x00	
#define	ASONG_OFF     AUTO_CTL = 0x00, AUTO_LED = 0x00			//:Dennis, Calceled! AUTO_LED 0x01-->0x00

//#define	WUTA_ON		  WUTA_CTL = 0x00, WUTA_LED = 0x00	
//#define	WUTA_OFF	  WUTA_CTL = 0x20, WUTA_LED = 0x00			//:Dennis, Calceled! WUTA_LED 0x02-->0x00
//:Dennis, Calceled! WUTA_LED 
#define	WUTA_ON		  WUTA_CTL = 0x10 	//*** WUTA_CTL = 0x00, ~DA,241007
#define	WUTA_OFF	  WUTA_CTL = 0x00 	//*** WUTA_CTL = 0x10, ~DA,241007

#define	HICUT_ON      HICUT_CTL = 0x00, HICUT_LED = 0x04
#define	HICUT_OFF     HICUT_CTL = 0x80, HICUT_LED = 0x00

#define	LOWCUT_ON     LOWCUT_CTL = 0x00, LOWCUT_LED = 0x02
#define	LOWCUT_OFF    LOWCUT_CTL = 0x40, LOWCUT_LED = 0x00

#define	ANTI_ON       ANTI_CTL = 0x00, ANTI_LED = 0x01
#define	ANTI_OFF      ANTI_CTL = 0x02, ANTI_LED = 0x00
	
#define REV_ON        REV_CTL = 0x00, REV_LED = 0x08	
#define REV_OFF       REV_CTL = 0x04, REV_LED = 0x00
//------

//#define Key_ON    KEY_CTL = 0x00
//#define Key_OFF   KEY_CTL = 0x10
	
#define	POWER_BlueLED_ON    STB_LED = 0x80  //Power Blue
#define	POWER_GreenLED_ON   STB_LED = 0x40 //Power Green
#define	POWER_RedLED_ON     STB_LED = 0x20 //Power Red	
#define	POWER_LED_All_OFF   STB_LED = 0x00 //Power ALLLED OFF	
#define	POWER_LED_All_ON    STB_LED = 0xE0 //Power ALLLED ON	

#define	Mute_OFF_Function  0
#define	Mute_ON_Function   1

#define STB_CTL_ON   STB_CTL = 0x00
#define STB_CTL_OFF  STB_CTL = 0x08

#define VU_ON    DISP_CTL = 0x40
#define VU_OFF   DISP_CTL = 0x00

//~DA,240812
/***orig, bug
#define ECHO_ON		ECHO_CTL=0x00, ECHO_LED=0x10
#define ECHO_OFF	ECHO_CTL=0x08, ECHO_LED=0x00
#define STECHO_ON	STECHO_CTL=0x00, STECHO_LED=0x20
#define STECHO_OFF  STECHO_CTL=0x10, STECHO_LED=0x00
***/
//~DA,240912,240918, Changed. (LED 1=light,0=nolight ; CTL 0=ON,1=OFF)
#define ECHO_ON		ECHO_LED = 0x10,   ECHO_CTL = 0x00, STECHO_CTL = 0x10, STECHO_LED = 0x00
#define ECHO_OFF	ECHO_LED = 0x00,   ECHO_CTL = 0x08, STECHO_CTL = 0x10  //, STECHO_LED = 0x00
#define STECHO_ON	STECHO_LED = 0x20, ECHO_CTL = 0x00, STECHO_CTL = 0x00, ECHO_LED = 0x00
#define STECHO_OFF  STECHO_LED = 0x00, ECHO_CTL = 0x08, STECHO_CTL = 0x10  //, ECHO_LED = 0x00



//////////////////////////////////////////////////////////////////////////


//============================================================
#define PCLK1_High      PCLK1 = 1
#define PCLK1_Low       PCLK1 = 0
#define PCLK2_High      PCLK2 = 1
#define PCLK2_Low       PCLK2 = 0
#define PCLK3_High      PCLK3 = 1
#define PCLK3_Low       PCLK3 = 0

#define EX0_High      EX0 = 1
#define EX0_Low       EX0 = 0
#define TR0_High      TR0 = 1
#define TR0_Low       TR0 = 0
#define TR1_High      TR1 = 1
#define TR1_Low       TR1 = 0

#define SCLK_High     SCLK = 1
#define SCLK_Low      SCLK = 0
#define PCLK_High     PCLK = 1
#define PCLK_Low      PCLK = 0

//---------- EEPROM address define ------------------------------
//#define EEPROM_24c04_Write  0xa0
//#define EEPROM_24c04_Read   0xa1
//0x00~0x1ff
#define Machine_code1_addr         0x00
#define Machine_code2_addr         0x01
#define Machine_code3_addr         0x02
#define Machine_code4_addr         0x03

#define Machine_code1_Data         0x54//0x54//T
#define Machine_code3_Data         0x58//X
#define Machine_code2_Data         0x2d//-
#define Machine_code4_Data         0x32//0x31//1-->2		//:Dennis, TX2


//:Dennis, EEPROM items, check, to be added for TX2?
//---------- EEPROM address define ------------------------------
#define  SP_Function_addr          0x05
#define  ECHO_Function_addr        0x06
#define  STECHO_Function_addr      0x07
#define  LOUD_Function_addr        0x08
#define  REV_Function_addr         0x09
#define  ANTI_Function_addr        0x0a
#define  HICUT_Function_addr       0x0b
#define  LOWCUT_Function_addr      0x0c

#define  Music_Limit_addr          0x0d
#define  Mic_Limit_addr            0x0e
#define  Preset_Music_addr         0x0f
#define  Preset_Mic_addr           0x10
#define  Limit_Flag_addr           0x11
#define  Remote_Key_Flag_addr      0x12
#define  VR_Auto_Flag_addr         0x13
#define  RunVR_Flag_addr           0x14

//:Dennis,A
#define  DIRECTPWR_Function_addr   0x15
#define  INPUT_Function_addr       0x16


//**************************************************
#define  SP_Function_Defaul           1
#define  ECHO_Function_Defaul         1
#define  STECHO_Function_Defaul       0
#define  LOUD_Function_Defaul         0
#define  REV_Function_Defaul          0
#define  ANTI_Function_Defaul         0
#define  HICUT_Function_Defaul        0
#define  LOWCUT_Function_Defaul       0

#define  Music_Limit_Defaul          70
#define  Mic_Limit_Defaul            80
#define  Preset_Music_Defaul         45
#define  Preset_Mic_Defaul           55
#define  Limit_Flag_Defaul            0
#define  Remote_Key_Flag_Defaul       1
#define  VR_Auto_Flag_Defaul          1

//:Dennis,A
#define  DIRECTPWR_Function_Defaul    0			//Standby
#define  INPUT_Function_Defaul        0			//KTV

//***********************************************

//******* R3 Remote Code  ************************
#define TDF_R3_Custom_Code         0x28
#define TDF_R3_Custom_Code_BAR         0xd7

#define TDF_R3_Power_Code          0x12
#define TDF_R3_Power_Code_BAR          0xed

#define TDF_R3_Music_UP_Code       0x1a
#define TDF_R3_Music_UP_Code_BAR       0xe5
#define TDF_R3_Music_DN_Code       0x1b
#define TDF_R3_Music_DN_Code_BAR       0xe4
#define TDF_R3_Mic_UP_Code         0x05
#define TDF_R3_Mic_UP_Code_BAR         0xfa
#define TDF_R3_Mic_DN_Code         0x03
#define TDF_R3_Mic_DN_Code_BAR         0xfc

#define	TDF_R3_Chennal_UP_Code     0x0f
#define	TDF_R3_Chennal_UP_Code_BAR     0xf0
#define	TDF_R3_Chennal_DN_Code     0x1e
#define	TDF_R3_Chennal_DN_Code_BAR     0xe1
#define	TDF_R3_SP_Code             0x00
#define	TDF_R3_SP_Code_BAR             0xff

/**
#if 0
#define	TDF_R3_Chennal_KTV_Code     0x2a 
#define	TDF_R3_Chennal_KTV_Code_BAR     0xd5
#define	TDF_R3_Chennal_DVD_Code     0x2b
#define	TDF_R3_Chennal_DVD_Code_BAR     0xd4
#define	TDF_R3_Chennal_VCR_Code     0x2c 
#define	TDF_R3_Chennal_VCR_Code_BAR     0xd3
#define	TDF_R3_Chennal_AUX_Code     0x2d
#define	TDF_R3_Chennal_AUX_Code_BAR     0xd2
#endif
*/
//:Dennis,A -- extends input Sources
#define TDF_R3_KTV_Channel				0x20
#define TDF_R3_KTV_Channel_BAR		0xdf
#define TDF_R3_DVD_Channel				0x21
#define TDF_R3_DVD_Channel_BAR		0xde
#define TDF_R3_AUX_Channel				0x22
#define TDF_R3_AUX_Channel_BAR		0xdd
#define TDF_R3_BT_Channel					0x23
#define TDF_R3_BT_Channel_BAR			0xdc
#define TDF_R3_COAX_Channel				0x24
#define TDF_R3_COAX_Channel_BAR		0xdb
#define TDF_R3_OPT1_Channel				0x25
#define TDF_R3_OPT1_Channel_BAR		0xda
#define TDF_R3_OPT2_Channel				0x26
#define TDF_R3_OPT2_Channel_BAR		0xd9



#define	TDF_R3_SPA_Code             0x24
#define	TDF_R3_SPA_Code_BAR             0xdb
#define	TDF_R3_SPB_Code             0x25
#define	TDF_R3_SPB_Code_BAR             0xda

#define	TDF_R3_Mute_Code           0x01
#define	TDF_R3_Mute_Code_BAR           0xfe

#define TDF_R3_ANTI_Code         0x04
#define TDF_R3_ANTI_Code_BAR         0xfb
#define TDF_R3_Loud_Code         0x09
#define TDF_R3_Loud_Code_BAR         0xf6
#define TDF_R3_Rev_Code          0x0e
#define TDF_R3_Rev_Code_BAR          0xf1
#define	TDF_R3_HICUT_Code        0x08
#define	TDF_R3_HICUT_Code_BAR        0xf7
#define TDF_R3_LOWCUT_Code        0x02
#define TDF_R3_LOWCUT_Code_BAR        0xfd
#define	TDF_R3_WUTA_Code         0x17
#define	TDF_R3_WUTA_Code_BAR         0xe8
#define	TDF_R3_AUTO_Code         0x1c
#define	TDF_R3_AUTO_Code_BAR         0xe3
#define	TDF_R3_STECHO_Code       0x13
#define	TDF_R3_STECHO_Code_BAR       0xec
#define	TDF_R3_MONOECHO_Code     0x16
#define	TDF_R3_MONOECHO_Code_BAR     0xe9

//**************************************************
//BMB
#define BMB_Custom_Code         0x4d
#define BMB_Custom_Code_BAR     0xb2

#define BMB_Music_UP_Code       0x02
#define BMB_Music_UP_Code_BAR       0xfd
#define BMB_Music_DN_Code       0x03
#define BMB_Music_DN_Code_BAR       0xfc
#define BMB_Mic_UP_Code         0x04
#define BMB_Mic_UP_Code_BAR         0xfb
#define BMB_Mic_DN_Code         0x05
#define BMB_Mic_DN_Code_BAR         0xfa
//#define BMB_Key_UP_Code         0x0b
//#define BMB_Key_DN_Code         0x0d
#define BMB_UP4_Code            0x08
#define BMB_UP3_Code            0x09
#define BMB_UP2_Code            0x0a
#define BMB_UP1_Code            0x0b
#define BMB_0_Code              0x0c
#define BMB_DN1_Code            0x0d
#define BMB_DN2_Code            0x0e
#define BMB_DN3_Code            0x0f
#define BMB_DN4_Code            0x10
//***************************************************
//金嗓	Golden Voice
#define	GVO_Custom_Code			0x04
#define	GVO_Custom_Code_BAR		0xfb
#define	GVN_Custom_Code			0x84
#define	GVN_Custom_Code_BAR		0x7b

#define GV_Music_UP_Code        0x08
#define GV_Music_UP_Code_BAR        0xf7
#define GV_Music_DN_Code        0x10
#define GV_Music_DN_Code_BAR        0xef
#define GV_Mic_UP_Code          0x09
#define GV_Mic_UP_Code_BAR          0xf6
#define GV_Mic_DN_Code          0x0f
#define GV_Mic_DN_Code_BAR          0xf0
//*****************************************************

//7segment display
#define Character_0     Character = 0x82//Disp = 0x82//0
#define Character_1     Character = 0xe7//Disp = 0xe7//1
#define Character_2     Character = 0x4a//Disp = 0x4a//2
#define Character_3     Character = 0x46//Disp = 0x46//3
#define Character_4     Character = 0x27//Disp = 0x27//4

#define Character_5     Character = 0x16//Disp = 0x16//5
#define Character_6     Character = 0x12//Disp = 0x21//6
#define Character_7     Character = 0xc7//Disp = 0xc7//7
#define Character_8     Character = 0x02//Disp = 0x02//8
#define Character_9     Character = 0x06//Disp = 0x06//9
#define Character_dp    Character = 0xfd//Disp = 0xfd//dp
#define Character_Null  Character = 0xff//Disp = 0xff//Null


//============= Zsound  TX-1  RS232 =======================
#define	RS232_Leader_Code   0x00
#define	RS232_End_Code      0xff
#define RS232_Write         0x57
#define RS232_Read          0x52

//:Dennis, RS232 EEPROM items, check, to be added for TX2?
#define ZSound_RS232_Power_Addr    0x12
#define PowerON_Data               0x01
#define Power_Completed            0x02
#define PowerOFF_Data              0x00

#define ZSound_RS232_Music_Addr      0x1a
#define ZSound_RS232_Music_UPDN_Addr 0x3a

#define ZSound_RS232_Mic_Addr        0x05
#define ZSound_RS232_Mic_UPDN_Addr   0x35


#define ZSound_RS232_Channel_Addr  0x2a

//#define ZSound_RS232_KTV_Addr      0x12
//#define ZSound_RS232_DVD_Addr      0x12
//#define ZSound_RS232_VCR_Addr      0x12
//#define ZSound_RS232_AUX_Addr      0x12

#define ZSound_RS232_SPA_Addr      0x24
#define ZSound_RS232_SPB_Addr      0x25
#define ZSound_RS232_Mute_Addr     0x01
#define ZSound_RS232_ANTI_Addr     0x04
#define ZSound_RS232_Loud_Addr     0x09
#define ZSound_RS232_Reverb_Addr   0x0e
#define ZSound_RS232_HiCut_Addr    0x08
#define ZSound_RS232_LowCut_Addr   0x02
#define ZSound_RS232_WUTA_Addr     0x17
#define ZSound_RS232_AUTO_Addr     0x1c
#define ZSound_RS232_STEcho_Addr   0x13
#define ZSound_RS232_MonoEcho_Addr 0x16
//:Dennis,A -->existed above ZSound_RS232_Channel_Addr
//#define ZSound_RS232_SelectInput_Addr 0x2a				//:Dennis,A


//=========================================================


//:Dennis, Migrated for SPDIF
//CT-7301C
#define SRC_Mode             0x30//SRC Mode 3 = asynchronous + freq_selector

#define SPDIF_IN_0           0x00//Software input channel selector SPDIF_IN_0
#define SPDIF_IN_1           0x01//Software input channel selector SPDIF_IN_1
#define SPDIF_IN_2           0x02//Software input channel selector SPDIF_IN_2
#define SPDIF_IN_3           0x03//Software input channel selector SPDIF_IN_3
#define SPDIF_IN_4           0x04//Software input channel selector SPDIF_IN_4
#define I2S_IN_0             0x05//Software input channel selector I2S_IN_0
#define I2S_IN_1             0x06//Software input channel selector I2S_IN_1
#define I2S_IN_2             0x07//Software input channel selector I2S_IN_2

#define SRC_Output_Freq      0x0C//Software SRC frequency selector , 0x0C = 384 kHz

#define SOFT_WARE_MODE       SOFT_Hardware_MODE = 0x01//Power on default is hardware mode , 0 = h/w mode , 1 = s/w mode
#define Hard_WARE_MODE       SOFT_Hardware_MODE = 0x00//Power on default is hardware mode , 0 = h/w mode , 1 = s/w mode

#define CT_7301C_FORCE_MUTE_OFF   0x00//Mute control : 0 = normal , 1 = mute all output volume          
#define CT_7301C_FORCE_MUTE_ON    0x01//Mute control : 0 = normal , 1 = mute all output volume    


//SPDIF Sources
enum {
	SPDIF_AUX_IN = 0,
	SPDIF_COAX_IN,
	SPDIF_OPT1_IN,
	SPDIF_OPT2_IN
};


//~DA,240808, for IOEXP6524
enum {
	IOEXP_FUNC = 0,
	IOEXP_SRC,
};

//~DA,240918
typedef struct __ACTMagic
{
	U8 g;
	U8 i;
} ACTMagic_t;

//~DA,240807, for IOEXP6524
//*** #define ADDR_PCAL6524_Write  0x44 //0x44
//*** #define ADDR_PCAL6524_Read   0x45
#define IOEXP1_ADDR 	   0x44 //VSS
#define IOEXP1_ADDR_Write  0x44 //VSS
#define IOEXP1_ADDR_Read   0x45
//
#define IOEXP2_ADDR		   0x46 //VDD
#define IOEXP2_ADDR_Write  0x46 //VDD
#define IOEXP2_ADDR_Read   0x47


//__EOF
