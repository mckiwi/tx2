#include "REG_MPC82G516.h" //for MPC82G516 SFR definition
#include "userdefine.h"
#include "string.h"

//=================================================================================================
//  此版程式針對 PCB PN:TX1MCU01D
//=================================================================================================
// 2015/05/15 修正 RS232 控制時,限制音量沒加入.
// 2015/06/25 修正 限制音量設定只能設定一次(欲設定第二次時,轉動 VR 無效)的 Bug.
// 2015/08/13 取消 靜音 Fade in 功能
//---
// 2022.0210 TX2 extends from TX1
// 2024.0808 TX2, HW MCU BD changed: XIO,7SEG.
// 2024.0912 TX2, ECHO,STECHO controls and Leds
//

#define SEG7_SIMU  0				//:Dennis, Boot LED: 1=Scroll digits at same time, 0=R->L

U8 data gc_ScanTimeOut;
U8 data gc_IrPowerOnOff = 0;
U8 data gc_poweroff = 0;
U8 data gc_Task = 0;
U8 data Remote_PowerKey_Flag;
U8 data RS232_PowerKey_Flag;
U8 data RemKey_Action_Flag = 0;
U8 data RS232_Action_Flag = 0;
//U8 data	delay_1ms = 10;
U16 data gw_TimerBase = 0;
U16 data Button_Timer = 0;

unsigned char	IR_STATE, RepeatFlag, NewKeyFlag, Ticks, BitCount, Remote_Flag, Remote_Key_Flag;
unsigned char	TempData, Custom_Code, Data_Code, Custom_Code_BAR, Data_Code_BAR;
unsigned char	x55ms_counter, x220ms_counter;
int  x1ms;
char INPUT_Function;

unsigned char	SP_Function, LOUD_Function, Mute_Function, WUTA_CTL;
unsigned char	ANTI_Function, LOWCUT_Function, HICUT_Function, REV_Function, ECHO_Function, STECHO_Function; 
unsigned char	MUTE_CTL, Limit_Music, Limit_Mic, VR_Auto_Flag, PWM_Dimmer;
unsigned char	Limit_Flag, Power_ON_Flag, ASONG_Function;
unsigned char	LOUD_CTL, CH_SEL, Music_Limit, Mic_Limit, Preset_Music, Preset_Mic, AUTO_CTL, WUTA_Function, STECHO_CTL;
unsigned char	Power_BlueLED_Flash_Flag, Power_RedLED_Flash_Flag, Power_GreenLED_Flash_Flag, ECHO_CTL, HICUT_Function, LOCUT_Function;
unsigned char	SPA_LED, SPB_LED, LOUD_LED, CH_LED, STB_LED, SPA_CTL, SPB_CTL, DISP_CTL, Volume_Action_Flag, ANTI_CTL, RGB_PWM_Dimmer;
unsigned char	ANTI_LED, LOWCUT_LED, HICUT_LED, REV_LED, ECHO_LED, STECHO_LED, WUTA_LED, AUTO_LED;
unsigned char	REV_CTL, HICUT_CTL, LOWCUT_CTL, STB_CTL, FAN_CTL, Last_Music_Volume_Counter;
unsigned char	Music_Character_tens, Music_Character_units, Mic_Character_tens, Mic_Character_units, Music_tens, Music_units, Mic_tens, Mic_units;
unsigned char	Music_Display_Flag, Mic_Display_Flag, Music_Volume_Counter, Mic_Volume_Counter, MUTE_LED, Last_Limit_Flag;
unsigned char	Music_UP_Flag, Mic_UP_Flag, Music_DN_Flag, Mic_DN_Flag, Turbo_Volume_Counter, Character, Music_Scan_Display_counter, Mic_Scan_Display_counter;
unsigned char	BT_SPDIF_CTL;				//:Dennis
unsigned char	SPDIF_SOURCE;				//:Dennis

//~DA,240807
unsigned char 	IOEXP_ERR = 0;
//unsigned char	R_LED, G_LED, B_LED;
//unsigned char	KTV_LED, DVD_LED, AUX_LED, BT_LED, COAX_LED, OP1_LED, OPT2_LED, LOUD_LED;
unsigned char   PROT_RST = 0;
unsigned char	x10ms_counter;
unsigned char	x5ms_counter;
volatile bit bXIOLock = 0;		//~DA,240813, XIO bus locker
unsigned char	x25ms_counter;	//~DA,240919
bit bRelMute	= 0;			//~DA,240919
unsigned char Mute_RelVolume_Counter = 0;		//~DA,240919


//:Dennis,A
char  preSet_DIRECTPWR = 0, preSet_INPUT_Function = KTV;
char  coldBoot = 1;
char  fromSTB_SW = 0;				//:Dennis
bit isBoot = 1;	//~DA,240812
bit toInit = 1; //AC Power init

//
void each1ms_event(void);
void each10ms_event(void);			//~DA,240808
void each25ms_event(void);			//~DA,240919
void T2delay1ms(int count);
void Clear_State();
void Get_Code(void);
void SP_Action(void);
void Music_Volume_Action(void);
void Mic_Volume_Action(void);

void main(void);
void VR_Stop(void);
void All_VR_DN(void);
void INPUT_Action(void);
void ASONG_Action(void);
void WUTA_Action(void);
void Call_EEPROM_Data(void);
void EEPROM_Write_Defaul(void);
void Clear_EEPROM(void);
void LOUD_Action(void);
void ClearAllLED(void);
void ScanKey(void);
void Limit_VR_Set_Action(void);

void HICUT_Action(void);
void STECHO_Action(void);
void LOWCUT_Action(void);
void ANTI_Action(void);
void REV_Action(void);
void Power_ON_Init(void);
void Scan_Volume(void);
void STB_ON_Action(void);
void STB_OFF_Action(void);
void Main_Loop(void);
void Set_EEPROM_ToZero(void);
void ScanUPDNKey(void);	//按鍵掃描副程式
void Remote_Power_Key(void);
void Remote_PowerKey_Scan(void);
void IR_Code_input();
void Main_Function_Loop();
void SP_Action(void);
void ASONG_Action(void);
/***Orig,
void PCLK1_Action(void);
void PCLK2_Action(void);
void PCLK3_Action(void);
***/
void PCLK_Action(U8 expICNum);		//~DA,240807
//*** void Music_Display_7SEG_units(void);//個位數 units
//*** void Music_Display_7SEG_tens(void);//十位數 tens
//*** void Mic_Display_7SEG_units(void);//個位數 units
//*** void Mic_Display_7SEG_tens(void);//十位數 tens
void Scan_Display(void);
void Call_default_Init(void);
void CUT_Action(void);
void STB_ON_Action(void);
void STB_OFF_Action(void);

U8 PowerOff_Set(void);

//----NEW

void STB_OFF(void);
U8 PowerOff(void);

//---- NEW End

void delay10us(U16 kk);		//~DA,240807


//:Dennis,Migrate fro SPDIF switches
void SPDIF_Select_Action(U8 Audio_Function);
//-----------------------------------------------------------------------------------
void CT7302_ChSel_Action(U8 CT7302_ChSel);
void CT7302_Init(void);

//~DA,240808,
void Display_7SEG_init(void);
void Music_Display_7SEG(U8 digit);
void Mic_Display_7SEG(U8 digit);
void PowerPlug_init(void); 
void init_taskTPreset(void);
//


//7SEG,digit,Common Annode.
//~DA,240808,Dennis
////////////////////////////////////////////////////////////
const unsigned char SEG_DGDAT[11] = {0x03,0x9F,0x25,0x0D,0x99,0x49,0x41,0x1F,0x01,0x09,0xFE};
////////////////////////////////////////////////////////////

//~DA,Dennis	//~DA,240807
//cali'd (5ms: cc=2, counts=500)
//////////////////////////////////////////////////
void repeatCC(U8 cc, U16 counts)
{
	U16 i;
	while (cc--)
	{
		for (i = 0; i < counts; i++) ;	
	}
}
//////////////////////////////////////////////////

//~DA,Dennis touched //~DA,240807
//cali'ed,500=5ms ; 5000=>50ms
//////////////////////////////////////////////////
void delay10us(U16 kk)
{
  	//~DA
	repeatCC(2, kk);		
	
}//End of delay10us(U16 kk)
//////////////////////////////////////////////////


void NJWDelay(U8 kk)
{
	while(kk != 0)
	kk--;
}//End of NJWDelay(U8 kk)

/***
//~DA,240812
void ShortDelay(U8 kk)
{
	while(kk != 0)
	kk--;
}//End of NJWDelay(U8 kk)
***/


//~DA,240812,
void init_taskTPreset(void)
{
	x55ms_counter = x55ms_counter_Preset;
	x220ms_counter = x220ms_counter_Preset;
	x10ms_counter = x10ms_counter_Preset;	//~DA,240808
	x5ms_counter = x5ms_counter_Preset;		//~DA,240808
	x25ms_counter = x25ms_counter_Preset;	//~DA,240919	
}


void T0_int(void) interrupt 1
{
	Ticks++;
    //KeyDN = !KeyDN;
  	TL0 = t0;//TIMER0_TL;   //設定Timer1初始值0.1ms
  	TH0 = t0 >> 8;//TIMER0_TH;//設定Timer1初始值0.1ms
  	TR0 = High;// 啟動Timer0
}//End of T0_int(void) interrupt 1

void T1_int(void) interrupt 3
{
 	TR1 = High;// 啟動Timer1
    //KeyReset = !KeyReset;
}//End of T1_int(void) interrupt 3

void T2_int(void) interrupt 5
{
	x1ms--;
    //KeyUP = !KeyUP;
	TF2 = 0;
	if(gw_TimerBase)
	{
		gw_TimerBase--;
	}
	each1ms_event();
}//End of T2_int(void) interrupt 5

void T2delay1ms(int count)	// 使用Timer2 MODE1的時間延遲函式 ( X'tal 用 11.0592MHz 實測值為 1.0ms )
{
	x1ms = count;          	// 設定x秒=2 ×count ×50ms
 
	TF2 = 0;
  	while(x1ms > 0);         // 等待時間到,則跳出
}//End of T2delay1ms


void Start_Timer0(void)		//啟動Timer0
{
  	Ticks = 0;
  	TL0 = t0;//TIMER0_TL;   //設定Timer1初始值0.1ms
  	TH0 = t0 >> 8;//TIMER0_TH;   //設定Timer1初始值0.1ms
  	TR0 = High;
}//End of Start_Timer0

//*************************************************
//		辨識遙控器按鍵發生錯誤之清除狀態副程式
void Clear_State()
{
	IR_STATE = WAITING_STATE;
	Ticks = 0;		//將Ticks歸零，以便計算下一個code的時間
	BitCount = 0;
	TempData = 0xff;
}//End of Clear_State()

//************************************************
//		紅外線接收  客戶碼,資料碼  副程式
void Get_Code(void)  //Get 8bit Code
{
	BitCount++;
	if (BIT_0(Ticks))// && (Ticks > 10))		//收到的bit是0
	{
		TempData >>= 1;
	}
	else if (BIT_1(Ticks))// && (Ticks > 15))	//收到的bit是1
	{
		TempData >>= 1;
		TempData |= 0x80;
	}
	else {Clear_State();}
}//End of Get_Code(void)

//************************************************

void EXint0() interrupt 0
{
	EX0 = Low;
	TR0 = Low;
	switch(IR_STATE)                                 
	{
		case(WAITING_STATE):
			{
				IR_STATE = WAIT_NEXT_INT;	//下次產生INT0時會進入WAIT_NEXT_INT的狀態
				Start_Timer0();
				break;
			}
		case(WAIT_NEXT_INT):
			{

				if (IR_START(Ticks))//13.5ms+-1ms(135個0.1ms)
				{
					IR_STATE = GET_CUSTOM;
					TempData = 0xff;
					BitCount = 0;
					RepeatFlag = 0;
					Start_Timer0();
				}
				else if (IR_REPEAT(Ticks))//11.25ms+-1ms(112個0.1ms)
				{
					RepeatFlag = 1;
					Clear_State();
				}
				break;
			}
		case(GET_CUSTOM):
			{
				Get_Code();
				Start_Timer0();	//這裡要啟動定時器中斷 0 (Timer0) , 設定為 0.1ms
				if (BitCount == 8) {Custom_Code = TempData; BitCount = 0;TempData = 0; IR_STATE = GET_CUSTOM_BAR;}//下次狀態改為GET_CUSTOM_BAR
				break;
			}
		case(GET_CUSTOM_BAR):
			{
				Get_Code();
				Start_Timer0();	//這裡要啟動定時器中斷 0 (Timer0) , 設定為 0.1ms
				if (BitCount == 8) {Custom_Code_BAR = TempData; BitCount = 0; TempData = 0; IR_STATE = GET_DATA;}//下次狀態改為GET_DATA
				break;
			}
		case(GET_DATA):
			{
				Get_Code();
				Start_Timer0();	//這裡要啟動定時器中斷 0 (Timer0) , 設定為 0.1ms
				if (BitCount == 8) {Data_Code = TempData; BitCount = 0; TempData = 0; IR_STATE = GET_DATA_BAR;}//下次狀態改為GET_DATA_BAR
				break;
			}
		case(GET_DATA_BAR):
            {
	            Get_Code();
				Start_Timer0();	//這裡要啟動定時器中斷 0 (Timer0) , 設定為 0.1ms
	            if (BitCount == 8)
	            {
					Data_Code_BAR = TempData;
					BitCount = 0;
					TempData = 0;
					NewKeyFlag = 1;
					Clear_State();																						
			        if ((Custom_Code == TDF_R3_Custom_Code) && (Custom_Code_BAR == TDF_R3_Custom_Code_BAR) && ((Data_Code == TDF_R3_Power_Code)))// || (Data_Code == 0x54)))
		            {
						gc_IrPowerOnOff = 1;//if(gc_IrPowerOnOff != 1) gc_IrPowerOnOff = 1;
		            }
				}
	            break;
			}
		default:
			{
				VR_Stop();
				Clear_State();	//分析發生錯誤，清除狀態，回到WAITING_STATE
				break;
			}
	}
	TR0 = High;
	EX0 = High;
}//End of EXint0() interrupt 0
//*************************************************

void each220ms_event(void)
{
	//****** 這邊插入每 220ms 想做的事 ******
	if(Music_UP_Flag)
	{
		if (Turbo_Volume_Counter < (Turbo_Volume_Counter_Preset + 1))
		{
			Turbo_Volume_Counter++;
			Music_Volume_UP_Action();//Music_Volume_Action();
		}
	}
	if(Music_DN_Flag)
	{
		if (Turbo_Volume_Counter < (Turbo_Volume_Counter_Preset + 1))
		{
			Turbo_Volume_Counter++;
			Music_Volume_DN_Action();//Music_Volume_Action();
		}
	}
	if(Mic_UP_Flag)
	{
		if (Turbo_Volume_Counter < (Turbo_Volume_Counter_Preset + 1))
		{
			Turbo_Volume_Counter++;
			Mic_Volume_UP_Action();//Mic_Volume_Action();
		}
	}
	if(Mic_DN_Flag)
	{
		if (Turbo_Volume_Counter < (Turbo_Volume_Counter_Preset + 1))
		{
			Turbo_Volume_Counter++;
			Mic_Volume_DN_Action();//Mic_Volume_Action();
		}
	}

}//End of each220ms_event(void)

void each55ms_event(void)
{
	U8 VR_LED_Flash;
	x220ms_counter--;
	if (x220ms_counter < 1)
	{
		x220ms_counter = x220ms_counter_Preset;
		each220ms_event();
	}
	//****** 這邊插入每 55ms 想做的事 ******
	if(Power_BlueLED_Flash_Flag == 1 && bXIOLock==0)	//~DA,240813
	{
		Power_GreenLED_Flash_Flag = 0;
		Power_RedLED_Flash_Flag = 0;
		VR_LED_Flash++;
		if (VR_LED_Flash > 7) {VR_LED_Flash = 0;}
		if (VR_LED_Flash < 4)
		{
			POWER_LED_All_OFF;
		}
		else//if (VR_LED_Flash > 3)
		{
			POWER_BlueLED_ON;
		}
		//*** PCLK1_Action();
		PCLK_Action(IOEXP_SRC);	//~DA,240808
	}
	if(Power_RedLED_Flash_Flag == 1 && bXIOLock==0)
	{			
		Power_GreenLED_Flash_Flag = 0;
		Power_BlueLED_Flash_Flag = 0;
		VR_LED_Flash++;
		if (VR_LED_Flash > 7) {VR_LED_Flash = 0;}
		if (VR_LED_Flash < 4)
		{
			POWER_LED_All_OFF;
		}
		else//if (VR_LED_Flash > 3)
		{
			POWER_RedLED_ON;
		}
		//*** PCLK1_Action();
		PCLK_Action(IOEXP_SRC);	//~DA,240808
	}
	if(Power_GreenLED_Flash_Flag == 1 && bXIOLock==0)
	{	
		Power_BlueLED_Flash_Flag = 0;
		Power_RedLED_Flash_Flag = 0;
		VR_LED_Flash++;
		if (VR_LED_Flash > 7) {VR_LED_Flash = 0;}
		if (VR_LED_Flash < 4)
		{
			POWER_LED_All_OFF;
		}
		else//if (VR_LED_Flash > 3)
		{
			POWER_GreenLED_ON;
		}
		//*** PCLK1_Action();
		PCLK_Action(IOEXP_SRC);	//~DA,240808	
	}
	if(Music_UP_Flag)
	{
		if (Turbo_Volume_Counter > Turbo_Volume_Counter_Preset)
		{
			Turbo_Volume_Counter++;
			Music_Volume_UP_Action();
		}
	}
	if(Music_DN_Flag)
	{
		if (Turbo_Volume_Counter > Turbo_Volume_Counter_Preset)
		{
			Turbo_Volume_Counter++;
			Music_Volume_DN_Action();
		}
	}
	if(Mic_UP_Flag)
	{
		if (Turbo_Volume_Counter > Turbo_Volume_Counter_Preset)
		{
			Turbo_Volume_Counter++;
			Mic_Volume_UP_Action();
		}
	}
	if(Mic_DN_Flag)
	{
		if (Turbo_Volume_Counter > Turbo_Volume_Counter_Preset)
		{
			Turbo_Volume_Counter++;
			Mic_Volume_DN_Action();
		}
	}
}//End of each55ms_event(void)


//
//25ms task
void each25ms_event(void)
{
    #if MUTE_RAMPUP	
	//~DA,240919
	//Mute Release, Ramp up
	if (bRelMute == 1)
	{
		if (Mute_RelVolume_Counter < Last_Music_Volume_Counter)
		{
			Music_Volume_Counter = Mute_RelVolume_Counter;
			Volume_Action_Flag = 1;
			Music_Volume_UP_Action();
			Mute_RelVolume_Counter++;		//next
		}
		else
		{
			Mute_RelVolume_Counter = 0;
			bRelMute = 0;					//Stop
			Music_Volume_Counter = Last_Music_Volume_Counter;
		}
	}        
    #endif	

}//End of each25ms_event


//
//1ms task
void each1ms_event(void)
{

	x25ms_counter--;
	if (x25ms_counter < 1)
	{
		x25ms_counter= x25ms_counter_Preset;
		each25ms_event();
	}

	x55ms_counter--;
	if(x55ms_counter < 1)
	{
		x55ms_counter = x55ms_counter_Preset;
		each55ms_event();
	}


	//****** 這邊插入每 1ms 想做的事 ******


	//Orig
    //***240918,	if(Volume_Action_Flag)      //~DA,240918,
    //***240918,	{
		if(Music_Display_Flag)
		{
			Music_Scan_Display_counter--;
			if(Music_Scan_Display_counter < 1)
			{
				Music_Scan_Display_counter = Scan_Display_counter_Preset;
			}
			if(Music_Scan_Display_counter > 9)
			{
				DP1A1 = 1;
				//*** Music_Display_7SEG_units();
				Music_Display_7SEG(Music_Character_units);	//~DA,240808
				DP1A2 = 0;
			}
			if((Music_Scan_Display_counter < 10) && (Music_Scan_Display_counter > 0))
			{
				DP1A2 = 1;
				//*** Music_Display_7SEG_tens();
				Music_Display_7SEG(Music_Character_tens); //~DA,240808
				DP1A1 = 0;
			}
		}
		if(Mic_Display_Flag)
		{
			Mic_Scan_Display_counter--;
			if(Mic_Scan_Display_counter < 1)
			{
				Mic_Scan_Display_counter = Scan_Display_counter_Preset;
			}
			if(Mic_Scan_Display_counter > 9)
			{
				DP2A1 = 1;
				//*** Mic_Display_7SEG_units();
				Mic_Display_7SEG(Mic_Character_units);	//~DA,240808
				DP2A2 = 0;
		
			}
			if((Mic_Scan_Display_counter < 10) && (Mic_Scan_Display_counter > 0))
			{
				DP2A2 = 1;
				//*** Mic_Display_7SEG_tens();
				Mic_Display_7SEG(Mic_Character_tens); //~DA,240808
				DP2A1 = 0;
			}
		}
    //***240918,	}   //~DA,240918,
	
}//End of each1ms_event(void)

void VR_Stop(void)
{
	Remote_Flag = 0;
	Music_UP_Flag = 0;
	Music_DN_Flag = 0;
	Mic_UP_Flag = 0;
	Mic_DN_Flag = 0;
	Custom_Code = 0xff;
	Custom_Code_BAR = 0xff;
	Data_Code = 0xff;
	Data_Code_BAR = 0xff;
	Turbo_Volume_Counter = 0;
}//End of VR_Stop(void)


//~DA,240807,Dennis
//~D, Changed to fit P6524,
//:Dennis, new for expander
//   remap ioEXP due to PCB layout trace changed
//   extends to support multi expICs: via expICNum = 0,1(IC#L, IC#R)
//////////////////////////////////////////////////////////////////////////////////////////////////////
#define regBASE  0x04
//             0     1     2     3     4     5     6     7
//act Pairs: (0,0),(1,0),(1,1),(2,0),(1,2),(3,0),(2,1),(3,0)
const ACTMagic_t actMagic[8] = 
{{0,0}, {1,0}, {1,1}, {2,0}, {1,2}, {3,0}, {2,1}, {3,0}};


void PCLK_Action(U8 expICNum)
{
	U8 iact,index;
	U8 expIOAddr;
	U8 IOPORTS_DAT[2][3];		//~DA,240813

	//~DA,240807, which?
	if (expICNum > 1)
	  return;				//NA
	if (expICNum == 0)
	  expIOAddr = IOEXP1_ADDR;
	else
	  expIOAddr = IOEXP2_ADDR;
	
	bXIOLock = High;		//lock	
	//
	switch(expICNum)
	{
		case 0:
			IOPORTS_DAT[expICNum][0] = STB_CTL | MUTE_CTL | SPB_CTL | SPA_CTL;	//P0
			IOPORTS_DAT[expICNum][1] = HICUT_CTL | LOWCUT_CTL | LOUD_CTL | STECHO_CTL | ECHO_CTL | REV_CTL | ANTI_CTL;	//P1
			IOPORTS_DAT[expICNum][2] = SPA_LED | SPB_LED | STECHO_LED | ECHO_LED | REV_LED | HICUT_LED | LOWCUT_LED | ANTI_LED;	//P2
		 	break;
		case 1:
			IOPORTS_DAT[expICNum][0] = BT_SPDIF_CTL | DISP_CTL;	//P0
			IOPORTS_DAT[expICNum][1] = STB_LED | WUTA_CTL | CH_SEL | PROT_RST;	//P1
			IOPORTS_DAT[expICNum][2] = CH_LED | LOUD_LED;	//P2
			break;
		default:
			break;
	}

	//~DA,240809, maps changes
	//-------------------------------------------------------------------
	iact = 0;	//[bbb],~DA,240809, 
	//P0
	if (IOPORTS_DAT[expICNum][0] != IOPORTS_DATBK[expICNum][0])
	{
		iact += 1;
		IOPORTS_DATBK[expICNum][0] = IOPORTS_DAT[expICNum][0];
	}	
	//P1
	if (IOPORTS_DAT[expICNum][1] != IOPORTS_DATBK[expICNum][1])
	{
		iact += 2;
		IOPORTS_DATBK[expICNum][1] = IOPORTS_DAT[expICNum][1];
	}
	//P2
	if (IOPORTS_DAT[expICNum][2] != IOPORTS_DATBK[expICNum][2])
	{
		iact += 4;
		IOPORTS_DATBK[expICNum][2] = IOPORTS_DAT[expICNum][2];
	}
	//-------------------------------------------------------------------
	
	//take Act! depends Jobs to carry
	//--------------------------------------------------------------------------------- 
	//~DA,240918, Enhanced
    //(0,0),(1,0),(1,1),(2,0),(1,2),(3,0),(2,1),(3,0)
	switch(actMagic[iact].g)
	{
		case 0:	
			break;
		case 1:
			index = actMagic[iact].i;
			ioWriteOnePorts(expIOAddr, regBASE+index, &IOPORTS_DAT[expICNum][0], index);
			break;
		case 2:
			index = actMagic[iact].i;
			ioWriteTwoPorts(expIOAddr, regBASE+index, &IOPORTS_DAT[expICNum][0], index);		
			break;
		case 3:
			ioWriteThreePorts(expIOAddr, regBASE, &IOPORTS_DAT[expICNum][0]);
			break;
		default:
			break;
	}	
	//---------------------------------------------------------------------------------
	bXIOLock = Low;		//released	

}//End of PCLK_Action()
//////////////////////////////////////////////////////////////////////////////////////////////////////



//
//~DA,240808, init 7 SEG
void Display_7SEG_init(void)
{
	//DP1
	SRCLR_DP1 = Low;	//clear shift
	//delay10us(2);		//20us
	SRCLR_DP1 = High;
	SRCLK_DP1 = Low;	//start at L
	RCLK_DP1 = Low;		
	//DP2
	SRCLR_DP2 = Low;	//clear shift
	//delay10us(2);		//20us
	SRCLR_DP2 = High;
	SRCLK_DP2 = Low;	//start at L
	RCLK_DP2 = Low;	

	//ALL OFF
	DP1A1 = 1;		//OFF
	DP1A2 = 1;
	DP2A1 = 1;
	DP2A2 = 1;

	//
			
}


/***
//~DA,240809,
void Music_7SEG_CLK(void)
{
	delay10us(5);	//50us		
	SRCLK_DP1 = High;
	delay10us(5);	//50us
	SRCLK_DP1 = Low;
}

void Music_7SEG_LATCH(void)
{
	//latched out
	RCLK_DP1 = Low;	
	delay10us(5);		//50us		
	RCLK_DP1 = High;
	delay10us(5);		//50us	
	RCLK_DP1 = Low;	
}

//~DA,240809,
void Mic_7SEG_CLK(void)
{
	delay10us(5);	//50us		
	SRCLK_DP2 = High;
	delay10us(5);	//50us
	SRCLK_DP2 = Low;
}

void Mic_7SEG_LATCH(void)
{
	//latched out
	RCLK_DP2 = Low;	
	delay10us(5);		//50us		
	RCLK_DP2 = High;
	delay10us(5);		//50us	
	RCLK_DP2 = Low;	
}
***/


#if 0
//*** (((reserved Original))) --------------------------------------------------------------
//
//~DA,240808, Dennis
// rewrite 7 SEG display. Music,
///////////////////////////////////////////////////////////
void Music_Display_7SEG(U8 digit)
{
	U8 dat,i;

	if (digit > 10)
	  return;
	dat = SEG_DGDAT[digit];

	//MR
	SRCLR_DP1 = Low;	//clear shift
	SRCLR_DP1 = High;

	//Start
	SRCLK_DP1 = Low;	//start at L	
	for (i = 0; i < 8; i++)
	{
		SER_DP1 = (dat & 0x80) ? High:Low;
		SRCLK_DP1 = High;
		SRCLK_DP1 = Low;		
		if (i < 7) dat <<= 1;	//LSB 1st

	}
	//
	//latched out
	RCLK_DP1 = Low;	
	RCLK_DP1 = High;
	RCLK_DP1 = Low;	
}
#endif


//
//~DA,240808, Dennis
//Common Annode
// rewrite 7 SEG display. Music,
//dat = 0x25;	//abged --> 00100101 = 0x25 "2"
///////////////////////////////////////////////////////////
void Music_Display_7SEG(U8 digit)
{
	U8 dat,i;

	if (digit > 10)
	  return;
	
	//D  
	dat = SEG_DGDAT[digit];
	//MR
	SRCLR_DP1 = Low;	//clear shift
	SRCLR_DP1 = High;
	//Start
	SRCLK_DP1 = Low;	//start at L	
	RCLK_DP1 = Low;		//Latch
	for (i = 0; i < 8; i++)
	{
		//DAT ROLL
		SER_DP1 = (dat & 0x1) ? High:Low;
		//CLK
		SRCLK_DP1 = High;
		SRCLK_DP1 = Low;				
		dat >>= 1;	//LSB 1st
	}
	//latched out
	RCLK_DP1 = High;	//Latched
	RCLK_DP1 = Low;	
}

//
//~DA,240808, Dennis
//Common Annode
// rewrite 7 SEG display. Mic,
///////////////////////////////////////////////////////////
void Mic_Display_7SEG(U8 digit)
{
	U8 dat,i;

	if (digit > 10)
	  return;

	//D
	dat = SEG_DGDAT[digit];	
	//MR
	SRCLR_DP2 = Low;	//clear shift
	SRCLR_DP2 = High;
	//Start
	SRCLK_DP2 = Low;	//start at L
	RCLK_DP2 = Low;		//Latch		
	for (i = 0; i < 8; i++)
	{
		SER_DP2 = (dat & 0x01) ? High:Low;
		SRCLK_DP2 = High;
		SRCLK_DP2 = Low;
		dat >>= 1;	//LSB 1st
	}
	//
	//latched out
	RCLK_DP2 = High;	//Latched
	RCLK_DP2 = Low;	
}
///////////////////////////////////////////////////////////



#if 0
/***Orig, //~DA,240808, purged from Dennis
void Music_Display_7SEG_units(void)//個位數 units
{
	switch(Music_Character_units)//個位數 units
	{
		case(0):{Character_0; PCLK3_Action(); break;}//0
		case(1):{Character_1; PCLK3_Action(); break;}//1
		case(2):{Character_2; PCLK3_Action(); break;}//2
		case(3):{Character_3; PCLK3_Action(); break;}//3
		case(4):{Character_4; PCLK3_Action(); break;}//4
		case(5):{Character_5; PCLK3_Action(); break;}//5
		case(6):{Character_6; PCLK3_Action(); break;}//6
		case(7):{Character_7; PCLK3_Action(); break;}//7
		case(8):{Character_8; PCLK3_Action(); break;}//8
		case(9):{Character_9; PCLK3_Action(); break;}//9
		case(15):{Character_Null; PCLK3_Action(); break;}//Null
		default:{break;}
	}
}//End of Music_Display_7SEG_units(void)//個位數 units


void Music_Display_7SEG_tens(void)//十位數 tens
{
	switch(Music_Character_tens)//十位數 tens
	{
		case(0):{Character_0; PCLK3_Action(); break;}//0
		case(1):{Character_1; PCLK3_Action(); break;}//1
		case(2):{Character_2; PCLK3_Action(); break;}//2
		case(3):{Character_3; PCLK3_Action(); break;}//3
		case(4):{Character_4; PCLK3_Action(); break;}//4
		case(5):{Character_5; PCLK3_Action(); break;}//5
		case(6):{Character_6; PCLK3_Action(); break;}//6
		case(7):{Character_7; PCLK3_Action(); break;}//7
		case(8):{Character_8; PCLK3_Action(); break;}//8
		case(9):{Character_9; PCLK3_Action(); break;}//9
		case(15):{Character_Null; PCLK3_Action(); break;}//Null
		default:{break;}
	}
}//End of Music_Display_7SEG_tens(void)//十位數 tens


void Mic_Display_7SEG_units(void)//個位數 units
{
	switch(Mic_Character_units)//個位數 units
	{
		case(0):{Character_0; PCLK2_Action(); break;}//0
		case(1):{Character_1; PCLK2_Action(); break;}//1
		case(2):{Character_2; PCLK2_Action(); break;}//2
		case(3):{Character_3; PCLK2_Action(); break;}//3
		case(4):{Character_4; PCLK2_Action(); break;}//4
		case(5):{Character_5; PCLK2_Action(); break;}//5
		case(6):{Character_6; PCLK2_Action(); break;}//6
		case(7):{Character_7; PCLK2_Action(); break;}//7
		case(8):{Character_8; PCLK2_Action(); break;}//8
		case(9):{Character_9; PCLK2_Action(); break;}//9
		case(15):{Character_Null; PCLK2_Action(); break;}//Null
		default:{break;}
	}
}//End of Mic_Display_7SEG_units(void)//個位數 units


void Mic_Display_7SEG_tens(void)//十位數 tens
{
	switch(Mic_Character_tens)//十位數 tens
	{
		case(0):{Character_0; PCLK2_Action(); break;}//0
		case(1):{Character_1; PCLK2_Action(); break;}//1
		case(2):{Character_2; PCLK2_Action(); break;}//2
		case(3):{Character_3; PCLK2_Action(); break;}//3
		case(4):{Character_4; PCLK2_Action(); break;}//4
		case(5):{Character_5; PCLK2_Action(); break;}//5
		case(6):{Character_6; PCLK2_Action(); break;}//6
		case(7):{Character_7; PCLK2_Action(); break;}//7
		case(8):{Character_8; PCLK2_Action(); break;}//8
		case(9):{Character_9; PCLK2_Action(); break;}//9
		case(15):{Character_Null; PCLK2_Action(); break;}//Null
		default:{break;}
	}
}//End of Mic_Display_7SEG_tens(void)//十位數 tens
***/
#endif


void Music_Volume_Action(void)
{
	U8 x_Music_TempBuffer, Music_count;
	Music_count = Music_Volume_Counter;
	x_Music_TempBuffer = Max_Volume - Music_count;

	Music_tens = (Music_count / 10);
	//***Orig,~DA,240809, if (Music_tens < 1) {Music_Character_tens = 15;}
	if (Music_tens < 1) {Music_Character_tens = 0;}		//~DA,240809
	else {Music_Character_tens = Music_tens;}

	//***DA,240809,Orig, Music_units = (Music_count %= 10);
	Music_units = (Music_count % 10);
	Music_Character_units = Music_units;
	Music_Display_Flag = 1;

	NJW1159SetData(x_Music_TempBuffer, Music_Chennal);

}//End of Volume_Action(void)


void Mic_Volume_Action(void)
{
    U8 x_Mic_TempBuffer, Mic_count, Mic_Vol_Value, Mic_Temp_Vol;
	Mic_count = Mic_Volume_Counter;
    //x_Mic_TempBuffer = Max_Volume - Mic_count;
	 
    //--- For Displaying And Performing Value Conversions ---//
    
    Mic_Vol_Value = Mic_count;
    
    if(Mic_Vol_Value == 0X00)          					    //7 Segment Display Volume Value 95 --- 00
    {
        Mic_Temp_Vol = 0X00;                                //NJW1159 Execute Volume Value 0dB
    }
	else if(Mic_Vol_Value == 0X01)                          //7 Segment Display Volume Value 94 --- 01
    {
        Mic_Temp_Vol = 0X01;                                //NJW1159 Execute Volume Value -1dB
    }
    else if(Mic_Vol_Value == 0X02)     						//7 Segment Display Volume Value 93 --- 02
    {
        Mic_Temp_Vol = 0X03;                                //NJW1159 Execute Volume Value -1dB
    }
    else if(Mic_Vol_Value == 0X03)                          //7 Segment Display Volume Value 92 --- 03
    {
        Mic_Temp_Vol = 0X05;                                //NJW1159 Execute Volume Value -2dB
    }
    else if(Mic_Vol_Value == 0X04)							//7 Segment Display Volume Value 91 --- 04
    {
        Mic_Temp_Vol = 0X07;                                //NJW1159 Execute Volume Value -2dB
    }
	else if(Mic_Vol_Value == 0X05)    					    //7 Segment Display Volume Value 90 --- 05
    {
        Mic_Temp_Vol = 0X09;                                //NJW1159 Execute Volume Value -7dB
    }
    else if(Mic_Vol_Value == 0X06)    						//7 Segment Display Volume Value 89 --- 06
    {
        Mic_Temp_Vol = 0X0B;                                //NJW1159 Execute Volume Value -3dB
    }		
	else if(Mic_Vol_Value == 0X07)     						//7 Segment Display Volume Value 88 --- 07
    {
        Mic_Temp_Vol = 0X0D;                                //NJW1159 Execute Volume Value -7dB
    }
	else if(Mic_Vol_Value == 0X08)		    				//7 Segment Display Volume Value 87 --- 08
    {
        Mic_Temp_Vol = 0X0F;                                //NJW1159 Execute Volume Value -4dB
    }
	else if(Mic_Vol_Value == 0X09)     						//7 Segment Display Volume Value 86 --- 09
    {
        Mic_Temp_Vol = 0X11;                                //NJW1159 Execute Volume Value -7dB
    }
    else if(Mic_Vol_Value == 0X0A)   						//7 Segment Display Volume Value 85 --- 10
    {
        Mic_Temp_Vol = 0X13;                                //NJW1159 Execute Volume Value -5dB
    }
	else if(Mic_Vol_Value == 0X0B)     					    //7 Segment Display Volume Value 84 --- 11
    {
        Mic_Temp_Vol = 0X15;                                //NJW1159 Execute Volume Value -7dB
    }
    else if(Mic_Vol_Value == 0X0C)     						//7 Segment Display Volume Value 83 --- 12
    {
        Mic_Temp_Vol = 0X17;                                //NJW1159 Execute Volume Value -6dB
    }
	else if(Mic_Vol_Value == 0X0D)     						//7 Segment Display Volume Value 82 --- 13
    {
        Mic_Temp_Vol = 0X19;                                //NJW1159 Execute Volume Value -7dB
    }
    else if(Mic_Vol_Value == 0X0E)     						//7 Segment Display Volume Value 81 --- 14
    {
        Mic_Temp_Vol = 0X1B;                                //NJW1159 Execute Volume Value -7dB
    }
	else if(Mic_Vol_Value == 0X0F)     					    //7 Segment Display Volume Value 80 --- 15
    {
        Mic_Temp_Vol = 0X1C;                                //NJW1159 Execute Volume Value -8dB
    }		
    else if(Mic_Vol_Value == 0X10)     						//7 Segment Display Volume Value 79 --- 16
    {
        Mic_Temp_Vol = 0X1D;                                //NJW1159 Execute Volume Value -8dB
    }
	else if(Mic_Vol_Value == 0X11)     					    //7 Segment Display Volume Value 78 --- 17
    {
        Mic_Temp_Vol = 0X1E;                                //NJW1159 Execute Volume Value -11dB
    }
    else if(Mic_Vol_Value == 0X12)    						//7 Segment Display Volume Value 77 --- 18
    {
        Mic_Temp_Vol = 0X1F;                                //NJW1159 Execute Volume Value -9dB
    }
	else if(Mic_Vol_Value == 0X13)     						//7 Segment Display Volume Value 76 --- 19
    {
        Mic_Temp_Vol = 0X20;                                //NJW1159 Execute Volume Value -11dB
    }
    else if(Mic_Vol_Value == 0X14)     						//7 Segment Display Volume Value 75 --- 20
    {
        Mic_Temp_Vol = 0X21;                                //NJW1159 Execute Volume Value -10dB
    }    
	else if(Mic_Vol_Value == 0X15)     						//7 Segment Display Volume Value 74 --- 21
    {
        Mic_Temp_Vol = 0X22;                                //NJW1159 Execute Volume Value -11dB
    }
    else if(Mic_Vol_Value == 0X16)     						//7 Segment Display Volume Value 73 --- 22
    {
        Mic_Temp_Vol = 0X23;                                //NJW1159 Execute Volume Value -11dB
    }
	else if(Mic_Vol_Value == 0X17)							//7 Segment Display Volume Value 72 --- 23
    {
        Mic_Temp_Vol = 0X24;                               //NJW1159 Execute Volume Value -12dB
    }
    else if(Mic_Vol_Value == 0X18)							//7 Segment Display Volume Value 71 --- 24
    {
        Mic_Temp_Vol = 0X25;                                //NJW1159 Execute Volume Value -12dB
    }
	else if(Mic_Vol_Value == 0X19)                          //7 Segment Display Volume Value 70 --- 25
    {
        Mic_Temp_Vol = 0X26;                                //NJW1159 Execute Volume Value -13dB
    }
    else if(Mic_Vol_Value == 0X1A)                          //7 Segment Display Volume Value 69 --- 26
    {
        Mic_Temp_Vol = 0X27;                                //NJW1159 Execute Volume Value -13dB
    }    
    else if(Mic_Vol_Value == 0X1B)                          //7 Segment Display Volume Value 68 --- 27
    {
        Mic_Temp_Vol = 0X28;                                //NJW1159 Execute Volume Value -14dB
    }
    else if(Mic_Vol_Value == 0X1C)                          //7 Segment Display Volume Value 67 --- 28
    {
        Mic_Temp_Vol = 0X29;                                //NJW1159 Execute Volume Value -15dB
    }
    else if(Mic_Vol_Value == 0X1D)                          //7 Segment Display Volume Value 66 --- 29
    {
        Mic_Temp_Vol = 0X2A;                                //NJW1159 Execute Volume Value -16dB
    }
    else if(Mic_Vol_Value == 0X1E)                          //7 Segment Display Volume Value 65 --- 30
    {
        Mic_Temp_Vol = 0X2B;                                //NJW1159 Execute Volume Value -17dB
    }
    else if(Mic_Vol_Value == 0X1F)                          //7 Segment Display Volume Value 64 --- 31 
    {
        Mic_Temp_Vol = 0X2C;                                //NJW1159 Execute Volume Value -18dB
    }
    else if(Mic_Vol_Value == 0X20)                          //7 Segment Display Volume Value 63 --- 32
    {
        Mic_Temp_Vol = 0X2D;                                //NJW1159 Execute Volume Value -19dB
    }
    else if(Mic_Vol_Value == 0X21)                          //7 Segment Display Volume Value 62 --- 33
    {
        Mic_Temp_Vol = 0X2E;                                //NJW1159 Execute Volume Value -20dB
    }
    else if(Mic_Vol_Value == 0X22)                          //7 Segment Display Volume Value 61 --- 34
    {
        Mic_Temp_Vol = 0X2F;                                //NJW1159 Execute Volume Value -21dB
    }
    else if(Mic_Vol_Value == 0X23)                          //7 Segment Display Volume Value 60 --- 35
    {
        Mic_Temp_Vol = 0X30;                                //NJW1159 Execute Volume Value -22dB
    }
    else if(Mic_Vol_Value == 0X24)                          //7 Segment Display Volume Value 59 --- 36
    {
        Mic_Temp_Vol = 0X31;                                //NJW1159 Execute Volume Value -23dB
    }
    else if(Mic_Vol_Value == 0X25)                          //7 Segment Display Volume Value 58 --- 37
    {
        Mic_Temp_Vol = 0X32;                                //NJW1159 Execute Volume Value -24dB
    }
    else if(Mic_Vol_Value == 0X26)                          //7 Segment Display Volume Value 57 --- 38
    {
        Mic_Temp_Vol = 0X33;                                //NJW1159 Execute Volume Value -25dB
    }
    else if(Mic_Vol_Value == 0X27)                          //7 Segment Display Volume Value 56 --- 39
    {
        Mic_Temp_Vol = 0X34;                                //NJW1159 Execute Volume Value -26dB
    }
    else if(Mic_Vol_Value == 0X28)                          //7 Segment Display Volume Value 55 --- 40
    {
        Mic_Temp_Vol = 0X35;                                //NJW1159 Execute Volume Value -27dB
    }
    else if(Mic_Vol_Value == 0X29)                          //7 Segment Display Volume Value 54 --- 41
    {
        Mic_Temp_Vol = 0X36;                                //NJW1159 Execute Volume Value -28dB
    }
    else if(Mic_Vol_Value == 0X2A)                          //7 Segment Display Volume Value 53 --- 42
    {
        Mic_Temp_Vol = 0X37;                                //NJW1159 Execute Volume Value -29dB
    }    
    else if(Mic_Vol_Value == 0X2B)                          //7 Segment Display Volume Value 52 --- 43
    {
        Mic_Temp_Vol = 0X38;                                //NJW1159 Execute Volume Value -30dB
    }
    else if(Mic_Vol_Value == 0X2C)                          //7 Segment Display Volume Value 51 --- 44
    {
        Mic_Temp_Vol = 0X39;                                //NJW1159 Execute Volume Value -31dB
    }
    else if(Mic_Vol_Value == 0X2D)                          //7 Segment Display Volume Value 50 --- 45
    {
        Mic_Temp_Vol = 0X3A;                                //NJW1159 Execute Volume Value -32dB
    }
    else if(Mic_Vol_Value == 0X2E)                          //7 Segment Display Volume Value 49 --- 46
    {
        Mic_Temp_Vol = 0X3B;                                //NJW1159 Execute Volume Value -33dB
    }
    else if(Mic_Vol_Value == 0X2F)                          //7 Segment Display Volume Value 48 --- 47
    {
        Mic_Temp_Vol = 0X3C;                                //NJW1159 Execute Volume Value -34dB
    }
    else if(Mic_Vol_Value == 0X30)                          //7 Segment Display Volume Value 47 --- 48
    {
        Mic_Temp_Vol = 0X3D;                                //NJW1159 Execute Volume Value -35dB
    }
    else if(Mic_Vol_Value == 0X31)                          //7 Segment Display Volume Value 46 --- 49
    {
        Mic_Temp_Vol = 0X3E;                                //NJW1159 Execute Volume Value -36dB
    }
    else if(Mic_Vol_Value == 0X32)                          //7 Segment Display Volume Value 45 --- 50
    {
        Mic_Temp_Vol = 0X3F;                                //NJW1159 Execute Volume Value -37dB
    }
    else if(Mic_Vol_Value == 0X33)                          //7 Segment Display Volume Value 44 --- 51
    {
        Mic_Temp_Vol = 0X40;                                //NJW1159 Execute Volume Value -38dB
    }
    else if(Mic_Vol_Value == 0X34)                          //7 Segment Display Volume Value 43 --- 52
    {
        Mic_Temp_Vol = 0X41;                                //NJW1159 Execute Volume Value -39dB
    }
    else if(Mic_Vol_Value == 0X35)                          //7 Segment Display Volume Value 42 --- 53
    {
        Mic_Temp_Vol = 0X42;                                //NJW1159 Execute Volume Value -40dB
    }
    else if(Mic_Vol_Value == 0X36)                          //7 Segment Display Volume Value 41 --- 54
    {
        Mic_Temp_Vol = 0X43;                                //NJW1159 Execute Volume Value -41dB
    }
    else if(Mic_Vol_Value == 0X37)                          //7 Segment Display Volume Value 40 --- 55
    {
        Mic_Temp_Vol = 0X44;                                //NJW1159 Execute Volume Value -42dB
    }
    else if(Mic_Vol_Value == 0X38)                          //7 Segment Display Volume Value 39 --- 56
    {
        Mic_Temp_Vol = 0X45;                                //NJW1159 Execute Volume Value -43dB
    }
    else if(Mic_Vol_Value == 0X39)                          //7 Segment Display Volume Value 38 --- 57
    {
        Mic_Temp_Vol = 0X46;                                //NJW1159 Execute Volume Value -44dB
    }
    else if(Mic_Vol_Value == 0X3A)                          //7 Segment Display Volume Value 37 --- 58
    {
        Mic_Temp_Vol = 0X47;                                //NJW1159 Execute Volume Value -45dB
    }    
    else if(Mic_Vol_Value == 0X3B)                          //7 Segment Display Volume Value 36 --- 59
    {
        Mic_Temp_Vol = 0X48;                                //NJW1159 Execute Volume Value -46dB
    }
    else if(Mic_Vol_Value == 0X3C)                          //7 Segment Display Volume Value 35 --- 60
    {
        Mic_Temp_Vol = 0X49;                                //NJW1159 Execute Volume Value -47dB
    }
    else if(Mic_Vol_Value == 0X3D)                          //7 Segment Display Volume Value 34 --- 61
    {
        Mic_Temp_Vol = 0X4A;                                //NJW1159 Execute Volume Value -48dB
    }
    else if(Mic_Vol_Value == 0X3E)                          //7 Segment Display Volume Value 33 --- 62
    {
        Mic_Temp_Vol = 0X4B;                                //NJW1159 Execute Volume Value -49dB
    }
    else if(Mic_Vol_Value == 0X3F)                          //7 Segment Display Volume Value 32 --- 63
    {
        Mic_Temp_Vol = 0X4C;                                //NJW1159 Execute Volume Value -50dB
    }
    else if(Mic_Vol_Value == 0X40)                          //7 Segment Display Volume Value 31 --- 64
    {
        Mic_Temp_Vol = 0X4D;                                //NJW1159 Execute Volume Value -51dB
    }
    else if(Mic_Vol_Value == 0X41)                          //7 Segment Display Volume Value 30 --- 65
    {
        Mic_Temp_Vol = 0X4E;                                //NJW1159 Execute Volume Value -52dB
    }
    else if(Mic_Vol_Value == 0X42)                          //7 Segment Display Volume Value 29 --- 66
    {
        Mic_Temp_Vol = 0X4F;                                //NJW1159 Execute Volume Value -53dB
    }
    else if(Mic_Vol_Value == 0X43)                          //7 Segment Display Volume Value 28 --- 67
    {
        Mic_Temp_Vol = 0X50;                                //NJW1159 Execute Volume Value -54dB
    }
    else if(Mic_Vol_Value == 0X44)                          //7 Segment Display Volume Value 27 --- 68
    {
        Mic_Temp_Vol = 0X51;                                //NJW1159 Execute Volume Value -55dB
    }
    else if(Mic_Vol_Value == 0X45)                          //7 Segment Display Volume Value 26 --- 69
    {
        Mic_Temp_Vol = 0X52;                                //NJW1159 Execute Volume Value -56dB
    }
    else if(Mic_Vol_Value == 0X46)                          //7 Segment Display Volume Value 25 --- 70
    {
        Mic_Temp_Vol = 0X53;                                //NJW1159 Execute Volume Value -57dB
    }
    else if(Mic_Vol_Value == 0X47)                          //7 Segment Display Volume Value 24 --- 71
    {
        Mic_Temp_Vol = 0X53;                                //NJW1159 Execute Volume Value -58dB
    }
    else if(Mic_Vol_Value == 0X48)                          //7 Segment Display Volume Value 23 --- 72
    {
        Mic_Temp_Vol = 0X54;                                //NJW1159 Execute Volume Value -59dB
    }
    else if(Mic_Vol_Value == 0X49)                          //7 Segment Display Volume Value 22 --- 73
    {
        Mic_Temp_Vol = 0X54;                                //NJW1159 Execute Volume Value -60dB
    }
    else if(Mic_Vol_Value == 0X4A)                          //7 Segment Display Volume Value 21 --- 74
    {
        Mic_Temp_Vol = 0X55;                                //NJW1159 Execute Volume Value -61dB
    }    
    else if(Mic_Vol_Value == 0X4B)                          //7 Segment Display Volume Value 20 --- 75
    {
        Mic_Temp_Vol = 0X55;                                //NJW1159 Execute Volume Value -62dB
    }
    else if(Mic_Vol_Value == 0X4C)                          //7 Segment Display Volume Value 19 --- 76
    {
        Mic_Temp_Vol = 0X56;                                //NJW1159 Execute Volume Value -63dB
    }
    else if(Mic_Vol_Value == 0X4D)                          //7 Segment Display Volume Value 18 --- 77
    {
        Mic_Temp_Vol = 0X56;                                //NJW1159 Execute Volume Value -64dB
    }
    else if(Mic_Vol_Value == 0X4E)                          //7 Segment Display Volume Value 17 --- 78
    {
        Mic_Temp_Vol = 0X57;                                //NJW1159 Execute Volume Value -65dB
    }
    else if(Mic_Vol_Value == 0X4F)                          //7 Segment Display Volume Value 16 --- 79
    {
        Mic_Temp_Vol = 0X57;                                //NJW1159 Execute Volume Value -66dB
    }
    else if(Mic_Vol_Value == 0X50)                          //7 Segment Display Volume Value 15 --- 80
    {
        Mic_Temp_Vol = 0X58;                                //NJW1159 Execute Volume Value -67dB
    }
    else if(Mic_Vol_Value == 0X51)                          //7 Segment Display Volume Value 14 --- 81
    {
        Mic_Temp_Vol = 0X58;                                //NJW1159 Execute Volume Value -69dB
    }
    else if(Mic_Vol_Value == 0X52)                          //7 Segment Display Volume Value 13 --- 82
    {
        Mic_Temp_Vol = 0X59;                                //NJW1159 Execute Volume Value -71dB
    }
    else if(Mic_Vol_Value == 0X53)                          //7 Segment Display Volume Value 12 --- 83
    {
        Mic_Temp_Vol = 0X59;                                //NJW1159 Execute Volume Value -73dB
    }
    else if(Mic_Vol_Value == 0X54)                          //7 Segment Display Volume Value 11 --- 84
    {
        Mic_Temp_Vol = 0X5A;                                //NJW1159 Execute Volume Value -75dB
    }
    else if(Mic_Vol_Value == 0X55)                          //7 Segment Display Volume Value 10 --- 85
    {
        Mic_Temp_Vol = 0X5A;                                //NJW1159 Execute Volume Value -77dB
    }
    else if(Mic_Vol_Value == 0X56)                          //7 Segment Display Volume Value 09 --- 86
    {
        Mic_Temp_Vol = 0X5B;                                //NJW1159 Execute Volume Value -79dB
    }
    else if(Mic_Vol_Value == 0X57)                          //7 Segment Display Volume Value 08 --- 87
    {
        Mic_Temp_Vol = 0X5B;                                //NJW1159 Execute Volume Value -81dB
    }
    else if(Mic_Vol_Value == 0X58)                          //7 Segment Display Volume Value 07 --- 88
    {
        Mic_Temp_Vol = 0X5C;                                //NJW1159 Execute Volume Value -83dB
    }
    else if(Mic_Vol_Value == 0X59)                          //7 Segment Display Volume Value 06 --- 89
    {
        Mic_Temp_Vol = 0X5C;                                //NJW1159 Execute Volume Value -85dB
    }
    else if(Mic_Vol_Value == 0X5A)                          //7 Segment Display Volume Value 05 --- 90
    {
        Mic_Temp_Vol = 0X5D;                                //NJW1159 Execute Volume Value -87dB
    }
    else if(Mic_Vol_Value == 0X5B)                          //7 Segment Display Volume Value 04 --- 91
    {
        Mic_Temp_Vol = 0X5D;                                //NJW1159 Execute Volume Value -89dB
    }
    else if(Mic_Vol_Value == 0X5C)                          //7 Segment Display Volume Value 03 --- 92
    {
        Mic_Temp_Vol = 0X5E;                                //NJW1159 Execute Volume Value -91dB
    }
    else if(Mic_Vol_Value == 0X5D)                          //7 Segment Display Volume Value 02 --- 93
    {
        Mic_Temp_Vol = 0X5E;                                //NJW1159 Execute Volume Value -93dB
    }
    else if(Mic_Vol_Value == 0X5E)                          //7 Segment Display Volume Value 01 --- 94
    {
        Mic_Temp_Vol = 0X5F;                                //NJW1159 Execute Volume Value -95dB
    } 
    else if(Mic_Vol_Value == 0X5F)                          //7 Segment Display Volume Value 01 --- 95
    {
        Mic_Temp_Vol = 0X5F;                                //NJW1159 Execute Volume Value -95dB
    }
    
    //--- End Code ---//
    
    Mic_tens = (Mic_count / 10);
    
	//***Orig,~DA240809, if (Mic_tens < 1) {Mic_Character_tens = 15;}
	if (Mic_tens < 1) {Mic_Character_tens = 0;}		//~DA,240809
	else {Mic_Character_tens = Mic_tens;}

	//***DA,240809,Orig, Mic_units = (Mic_count %= 10);
	Mic_units = (Mic_count % 10);
	Mic_Character_units = Mic_units;
	Mic_Display_Flag = 1;
    
    //--- For Displaying And Performing Value Conversions ---//
    
    Mic_count = Mic_Temp_Vol;
    
    //--- End Code ---//
    
    x_Mic_TempBuffer = Max_Volume - Mic_count;
    
	NJW1159SetData(x_Mic_TempBuffer, Mic_Chennal);

}//End of Volume_Action(void)


void Music_Volume_UP_Action(void)
{
	if(Limit_Flag)
	{
		if((Music_Volume_Counter < Music_Limit) && (Music_Volume_Counter < Max_Volume))
		{
			Music_Volume_Counter++;
		}
	}
	else//if (Limit_Flag == 0)
	{
		if(Music_Volume_Counter < Max_Volume)
		{
			Music_Volume_Counter++;
		}
	}
	Music_Volume_Action();
}//End of Music_UP_Action(void)


void Music_Volume_DN_Action(void)
{
	if(Music_Volume_Counter > Min_Volume)
	{
		Music_Volume_Counter--;
	}
	Music_Volume_Action();
}//End of Music_DN_Action(void)


void Mic_Volume_UP_Action(void)
{
	if(Limit_Flag)
	{
		if((Mic_Volume_Counter < Mic_Limit) && (Mic_Volume_Counter < Max_Volume))
		{
			Mic_Volume_Counter++;
		}
	}
	else//if (Limit_Flag == 0)
	{
		if(Mic_Volume_Counter < Max_Volume)
		{
			Mic_Volume_Counter++;
		}
	}
	Mic_Volume_Action();
}//End of Music_UP_Action(void)


void Mic_Volume_DN_Action(void)
{
	if(Mic_Volume_Counter > Min_Volume)
	{
		Mic_Volume_Counter--;
	}
	Mic_Volume_Action();
}//End of Music_DN_Action(void)


void Music_UP_Action(void)
{
	Volume_Action_Flag = 1;
	Music_UP_Flag = 1;
	T2delay1ms(Remote_Delay_Time);
}//End of Music_UP_Action(void)


void Music_DN_Action(void)
{
	Volume_Action_Flag = 1;
	Music_DN_Flag = 1;
	T2delay1ms(Remote_Delay_Time);
}//End of Music_DN_Action(void)


void Mic_UP_Action(void)
{
	Volume_Action_Flag = 1;
	Mic_UP_Flag = 1;
	T2delay1ms(Remote_Delay_Time);
}//End of Mic_UP_Action(void)


void Mic_DN_Action(void)
{
	Volume_Action_Flag = 1;
	Mic_DN_Flag = 1;
	T2delay1ms(Remote_Delay_Time);
}//End of Mic_DN_Action(void)


void Mute_Action(void)
{
//	U8 i, Last_Music_Volume_Counter;
//	U8 Last_Music_Volume_Counter;
	Volume_Action_Flag = 0;
	if(Mute_Function > 1) {Mute_Function = Mute_OFF_Function;}
	switch(Mute_Function)
	{
		case(Mute_OFF_Function):// Mute OFF
		{
			Power_RedLED_Flash_Flag = 0;
			MUTE_OFF;
			POWER_BlueLED_ON;
			//*** PCLK1_Action();
			PCLK_Action(IOEXP_SRC);		//~DA,240808
			//*** PCLK3_Action();
			PCLK_Action(IOEXP_FUNC);	//~DA,240808
//			Music_Volume_Counter = 0;

			//~DA,240919, Support Mute release ramp up
			#if MUTE_RAMPUP
				//Ramp up, Last_Music_Volume_Counter
				bRelMute = 1;						//~DA,240919, Start Mute Release
				Mute_RelVolume_Counter =  0;		//~DA,240919, init start
			#else
				//Orig.
				Music_Volume_Counter = (Last_Music_Volume_Counter - 1);
				Volume_Action_Flag = 1;
				Music_Volume_UP_Action();
			#endif			

/*			gw_TimerBase = 500;
			while(gw_TimerBase) //wait
			{	   

			}
			for(i = 0; i < Last_Music_Volume_Counter; i++)
			{
				Music_Volume_UP_Action();
	
				gw_TimerBase = 25;
				while(gw_TimerBase) //wait
				{	   

				}
			}
*/
			break;
		}
		case(Mute_ON_Function)://Mute ON
		{
			MUTE_ON;
			//*** PCLK1_Action();
			PCLK_Action(IOEXP_SRC);	//~DA,240808			
			if (Power_ON_Flag) PCLK_Action(IOEXP_FUNC);	//~DA,240808  //*** PCLK3_Action();
			//
			//~DA,240919, Support ramp up
			#if MUTE_RAMPUP	
				if (bRelMute == 0)		//during releasing, do not set again
					Last_Music_Volume_Counter = Music_Volume_Counter;
				//
				Music_Volume_Counter = 0;
				Music_Volume_Action();
				Power_RedLED_Flash_Flag = 1;
				//					
				Mute_RelVolume_Counter =  0;		//~DA,240919, clear
				bRelMute = 0;						//~DA,240919, clear				
			#else			
				Last_Music_Volume_Counter = Music_Volume_Counter;
				Music_Volume_Counter = 0;
				Music_Volume_Action();
				Power_RedLED_Flash_Flag = 1;
			#endif			
					
			break;
		}
		default:{break;}
	}
	Music_Scan_Display_counter = Scan_Display_counter_Preset;
	Volume_Action_Flag = 1;
}//End of Mute_Action



#if 0
/*** Orig,
//TX1 ---------------------------------------------
void INPUT_Action(void)
{
	Volume_Action_Flag = 0;
	if(INPUT_Function < 0) INPUT_Function = 3;
	if(INPUT_Function > 3) INPUT_Function = 0;
	switch(INPUT_Function)
	{
		case(KTV):{CH_KTV; PCLK1_Action(); PCLK3_Action(); break;}//KTV
		case(DVD):{CH_DVD; PCLK1_Action(); PCLK3_Action(); break;}//DVD
		case(VCR):{CH_VCR; PCLK1_Action(); PCLK3_Action(); break;}//VCR
		case(AUX):{CH_AUX; PCLK1_Action(); PCLK3_Action(); break;}//AUX
		default:{break;}
	}
	Music_Scan_Display_counter = Scan_Display_counter_Preset;
	Volume_Action_Flag = 1;
}//End of INPUT_SEL_Action(void)	
***/
#endif



//
//revised...TX-2
//TX2 ---------------------------------------------
void INPUT_Action(void)
{
	Volume_Action_Flag = 0;		
	//:Dennis---	Volume_Action_Flag = 0;
	if(INPUT_Function < 0) INPUT_Function = 6;			//:Dennis, [INPUT_Function], revised to add sources input
	if(INPUT_Function > 6) INPUT_Function = 0;
	
	switch(INPUT_Function)
	{
		//~DA,240808
		case(KTV):{CH_KTV; PCLK_Action(IOEXP_SRC); PCLK_Action(IOEXP_FUNC); /*PCLK1_Action(); PCLK3_Action();*/ break;}	//KTV //~DA,240808
		case(DVD):{CH_DVD; PCLK_Action(IOEXP_SRC); PCLK_Action(IOEXP_FUNC); /*PCLK1_Action(); PCLK3_Action();*/ break;}	//DVD
		case(AUX):{CH_AUX; PCLK_Action(IOEXP_SRC); PCLK_Action(IOEXP_FUNC); /*PCLK1_Action(); PCLK3_Action();*/ break;}	//AUX(VCR)
		case(BT): {CH_BT; PCLK_Action(IOEXP_SRC); PCLK_Action(IOEXP_FUNC); /*PCLK1_Action(); PCLK3_Action();*/ break;}		//BT   //:Dennis --> debug test on new target of "CT7302" control???
		case (COAX): {SPDIF_Select_Action(SPDIF_COAX_IN); CH_COAX; PCLK_Action(IOEXP_SRC); PCLK_Action(IOEXP_FUNC); /*PCLK1_Action(); PCLK3_Action();*/ break;}	//COAX, Need more action for SPDIF Switches on CT7302
		case (OPT1): {SPDIF_Select_Action(SPDIF_OPT1_IN); CH_OPT1; PCLK_Action(IOEXP_SRC); PCLK_Action(IOEXP_FUNC); /*PCLK1_Action(); PCLK3_Action();*/ break;}	//OPT1, CT7302	
		case (OPT2): {SPDIF_Select_Action(SPDIF_OPT2_IN); CH_OPT2; PCLK_Action(IOEXP_SRC); PCLK_Action(IOEXP_FUNC); /*PCLK1_Action(); PCLK3_Action();*/ break;}	//OPT2, CT7302
		default:{break;}
	}	
	//:---
	Music_Scan_Display_counter = Scan_Display_counter_Preset;
	Volume_Action_Flag = 1;
}//End of INPUT_SEL_Action(void)



void SP_Action(void)
{
	Volume_Action_Flag = 0;
	if(SP_Function > 3) SP_Function = 0;
	switch(SP_Function)
	{
		//~DA,240808
		case(AB_OFF):{FAN_LowSpeed; SPA_OFF; SPB_OFF;  PCLK_Action(IOEXP_SRC); PCLK_Action(IOEXP_FUNC); /*PCLK1_Action(); PCLK2_Action(); PCLK3_Action();*/ break;}//KTV
		case(A_ON)  : {FAN_HiSpeed; SPA_ON;  SPB_OFF;  PCLK_Action(IOEXP_SRC); PCLK_Action(IOEXP_FUNC); /*PCLK1_Action(); PCLK2_Action(); PCLK3_Action();*/ break;}//DVD
		case(AB_ON) : {FAN_HiSpeed; SPA_ON;  SPB_ON ;  PCLK_Action(IOEXP_SRC); PCLK_Action(IOEXP_FUNC); /*PCLK1_Action(); PCLK2_Action(); PCLK3_Action();*/ break;}//AUX
		case(B_ON)  : {FAN_HiSpeed; SPA_OFF; SPB_ON ;  PCLK_Action(IOEXP_SRC); PCLK_Action(IOEXP_FUNC); /*PCLK1_Action(); PCLK2_Action(); PCLK3_Action();*/ break;}//VCR
		default:{break;}
	}
 	if(Power_ON_Flag)
	{
		TDF24c04WriteOneByte(SP_Function_addr, SP_Function);
	}
	Music_Scan_Display_counter = Scan_Display_counter_Preset;
	Volume_Action_Flag = 1;
}//End of SPA_Action(void)



void ASONG_Action(void)
{
#if 0
//~DD,240912, removed function	
	Volume_Action_Flag = 0;
	if(ASONG_Function > 1) {ASONG_Function = 0;}
	switch(ASONG_Function)
	{
		//~DA,240808,
		case(ASONG_OFF_Function):{ASONG_OFF; PCLK_Action(IOEXP_SRC); /*PCLK1_Action();*/ break;} // ASONG_OFF
		case(ASONG_ON_Function):{WUTA_Function = 0; WUTA_OFF; ASONG_ON; PCLK_Action(IOEXP_SRC); /*PCLK1_Action();*/ break;} // ASONG_ON
		default:{break;}
	}
	Music_Scan_Display_counter = Scan_Display_counter_Preset;
	Volume_Action_Flag = 1;
#endif	
}//End of A_SONG_Action(void)

void WUTA_Action(void)
{
	Volume_Action_Flag = 0;
	if(WUTA_Function > 1)	{WUTA_Function = 0;}
	switch(WUTA_Function)
	{
		//~DA,240808
		case(WUTA_OFF_Function):{WUTA_OFF; PCLK_Action(IOEXP_SRC); /*PCLK1_Action();*/ break;} // WUTA_OFF
		case(WUTA_ON_Function):{/*~DD,240912, ASONG_Function = 0; ASONG_OFF; */ WUTA_ON; PCLK_Action(IOEXP_SRC); /*PCLK1_Action();*/ break;} // WUTA_ON
		default:{break;}
	}
	Music_Scan_Display_counter = Scan_Display_counter_Preset;
	Volume_Action_Flag = 1;
}//End of WUTA_Action(void)

void STECHO_Action(void)
{
	Volume_Action_Flag = 0;
	if(STECHO_Function > 1) {STECHO_Function = 0;}
	switch(STECHO_Function)
	{
		case(STECHO_OFF_Function):// STECHO OFF
			{
				/***
				ECHO_Function = 0;
				ECHO_CTL = 0x10;
				ECHO_LED = 0x08;
				STECHO_CTL = 0x08;
				STECHO_LED = 0x04;
				//*** PCLK1_Action();
				//*** PCLK2_Action();
				***/
				//~DA,240812,
				//***Orig, ECHO_Function = 0;				
				ECHO_Function = 0;			//~DA,240912
				#if 0
					STECHO_CTL = 0x10;		//STECHO_CTL=0x10,OFF
					STECHO_LED = 0x00;		//STECHO_LED=0x00,OFF
					ECHO_CTL = 0x08;		//ECHO_CTL=0x08,OFF
					ECHO_LED = 0x00;		//ECHO_LED=0x00,OFF	
				#else
					//STECHO_OFF;
					//ECHO_OFF;
					STECHO_OFF;		//~DA,240909					
				#endif				
				//~DA,240808
				PCLK_Action(IOEXP_SRC);		//~DA,240808
				PCLK_Action(IOEXP_FUNC);	//~DA,240808
				break;
			}
		case(STECHO_ON_Function):// STECHO ON
			{
				/***
				ECHO_Function = 0;
				ECHO_CTL = 0x00;
				ECHO_LED = 0x08;
				STECHO_CTL = 0x00;
				STECHO_LED = 0x00;
				//*** PCLK1_Action();
				//*** PCLK2_Action();
				***/
				//~DA,240812,
				ECHO_Function = 0;
				#if 0
					STECHO_CTL = 0x00;		//STECHO_CTL=0x00,ON
					STECHO_LED = 0x20;		//STECHO_LED=0x20,ON					
					ECHO_CTL = 0x08;		//ECHO_CTL=0x08,OFF
					ECHO_LED = 0x00;		//ECHO_LED=0x00,OFF	
				#else
					//STECHO_ON;
					//ECHO_OFF;
					STECHO_ON;		//~DA,240909				
				#endif
				//~DA,240808
				PCLK_Action(IOEXP_SRC);		//~DA,240808
				PCLK_Action(IOEXP_FUNC);	//~DA,240808				
				break;
			}
		default:{break;}
	}
	if(Power_ON_Flag)
	{
		TDF24c04WriteOneByte(STECHO_Function_addr, STECHO_Function);
		TDF24c04WriteOneByte(ECHO_Function_addr, ECHO_Function);
	}
	Music_Scan_Display_counter = Scan_Display_counter_Preset;
	Volume_Action_Flag = 1;
}//End of ECHO_Action(void)



void ECHO_Action(void)
{
	Volume_Action_Flag = 0;
	if(ECHO_Function > 1) {ECHO_Function = 0;}
	switch(ECHO_Function)
	{
		case(ECHO_OFF_Function):
			{
				/***
				STECHO_Function = 0;
				ECHO_CTL = 0x10;
				ECHO_LED = 0x08;
				STECHO_LED = 0x04;
				//*** PCLK1_Action();
				//*** PCLK2_Action();
				***/
				//~DA,240812,
				STECHO_Function = 0;				
				#if 0
					ECHO_CTL = 0x08;		//ECHO_CTL=0x08,OFF
					ECHO_LED = 0x00;		//ECHO_LED=0x00,OFF
					STECHO_CTL = 0x10;		//STECHO_CTL=0x10,OFF		
					STECHO_LED = 0x00;		//STECHO_LED=0x00,OFF
				#else
					//ECHO_OFF;
					//STECHO_OFF;
					ECHO_OFF;				//~DA,240909
				#endif			
				//~DA,240808
				PCLK_Action(IOEXP_SRC);		//~DA,240808
				PCLK_Action(IOEXP_FUNC);	//~DA,240808				
				break;
			} // ECHO OFF
		case(ECHO_ON_Function):
			{
				/***
				STECHO_Function = 0;
				STECHO_CTL = 0x00;
				STECHO_LED = 0x04;
				ECHO_CTL = 0x08;
				ECHO_LED = 0x00;
				//*** PCLK1_Action();
				//*** PCLK2_Action();
				***/
				//~DA,240812,
				STECHO_Function = 0;
				#if 0
					ECHO_CTL = 0x00;		//ECHO_CTL=0x00,ON
					ECHO_LED = 0x10;		//ECHO_LED=0x10,ON
					STECHO_CTL = 0x10;		//STECHO_CTL=0x10,OFF
					STECHO_LED = 0x00;		//STECHO_LED=0x00,OFF	
				#else
					//ECHO_ON;
					//STECHO_OFF;		
					ECHO_ON;				//~DA,240909		
				#endif						
				//~DA,240808
				PCLK_Action(IOEXP_SRC);		//~DA,240808
				PCLK_Action(IOEXP_FUNC);	//~DA,240808				
				break;
			} // ECHO ON
		default:{break;}
	}
	if(Power_ON_Flag)
	{
		TDF24c04WriteOneByte(ECHO_Function_addr, ECHO_Function);
		TDF24c04WriteOneByte(STECHO_Function_addr, STECHO_Function);
	}
	Music_Scan_Display_counter = Scan_Display_counter_Preset;
	Volume_Action_Flag = 1;
}//End of ECHO_Action(void)



void LOUD_Action(void)
{
	Volume_Action_Flag = 0;
	if(LOUD_Function > 1) {LOUD_Function = 0;}
	switch(LOUD_Function)
	{
		//~DA,240808
		case(LOUD_OFF_Function):{LOUD_OFF; PCLK_Action(IOEXP_SRC); PCLK_Action(IOEXP_FUNC); /*PCLK2_Action(); PCLK3_Action();*/ break;} // LOUD OFF
		case(LOUD_ON_Function) :{LOUD_ON ; PCLK_Action(IOEXP_SRC); PCLK_Action(IOEXP_FUNC); /*PCLK2_Action(); PCLK3_Action();*/ break;} // LOUD ON
		default:{break;}
	}
	if(Power_ON_Flag)
	{
		TDF24c04WriteOneByte(LOUD_Function_addr, LOUD_Function);
	}
	Music_Scan_Display_counter = Scan_Display_counter_Preset;
	Volume_Action_Flag = 1;
}//End of LOUD_Action(void)



void HICUT_Action(void)
{
	Volume_Action_Flag = 0;
	if(HICUT_Function > 1) {HICUT_Function = 0;}
	switch(HICUT_Function)
	{
		//~DA,240808
		case(HICUT_OFF_Function):{HICUT_OFF; PCLK_Action(IOEXP_SRC); PCLK_Action(IOEXP_FUNC); /*PCLK1_Action(); PCLK2_Action();*/ break;} // HICUT OFF
		case(HICUT_ON_Function):{HICUT_ON; PCLK_Action(IOEXP_SRC); PCLK_Action(IOEXP_FUNC); /*PCLK1_Action(); PCLK2_Action();*/ break;} // HICUT ON
		default:{break;}
	}
	if(Power_ON_Flag)
	{
		TDF24c04WriteOneByte(HICUT_Function_addr, HICUT_Function);
	}
	Music_Scan_Display_counter = Scan_Display_counter_Preset;
	Volume_Action_Flag = 1;
}//End of HICUT_Action(void)



void LOWCUT_Action(void)
{
	Volume_Action_Flag = 0;
	if(LOWCUT_Function > 1) {LOWCUT_Function = 0;}
	switch(LOWCUT_Function)
	{
		//~DA,240808
		case(LOWCUT_OFF_Function):{LOWCUT_OFF; PCLK_Action(IOEXP_SRC); PCLK_Action(IOEXP_FUNC); /*PCLK1_Action(); PCLK2_Action();*/ break;} // HICUT OFF
		case(LOWCUT_ON_Function):{LOWCUT_ON; PCLK_Action(IOEXP_SRC); PCLK_Action(IOEXP_FUNC); /*PCLK1_Action(); PCLK2_Action();*/ break;} // HICUT ON
		default:{break;}
	}
	if(Power_ON_Flag)
	{
		TDF24c04WriteOneByte(LOWCUT_Function_addr, LOWCUT_Function);
	}
	Music_Scan_Display_counter = Scan_Display_counter_Preset;
	Volume_Action_Flag = 1;
}//End of LOWCUT_Action(void)



void ANTI_Action(void)
{
	Volume_Action_Flag = 0;
	if(ANTI_Function > 1) {ANTI_Function = 0;}
	switch(ANTI_Function)
	{
		//~DA,240808
		case(ANTI_OFF_Function):{ANTI_OFF; PCLK_Action(IOEXP_SRC); PCLK_Action(IOEXP_FUNC); /*PCLK1_Action(); PCLK2_Action();*/ break;} // ANTI OFF
		case(ANTI_ON_Function) :{ANTI_ON ; PCLK_Action(IOEXP_SRC); PCLK_Action(IOEXP_FUNC); /*PCLK1_Action(); PCLK2_Action();*/ break;} // ANTI ON
		default:{break;}
	}
	if(Power_ON_Flag)
	{
		TDF24c04WriteOneByte(ANTI_Function_addr, ANTI_Function);
	}
	Music_Scan_Display_counter = Scan_Display_counter_Preset;
	Volume_Action_Flag = 1;
}//End of ANTI_Action(void)



void REV_Action(void)
{
	Volume_Action_Flag = 0;
	if(REV_Function > 1) {REV_Function = 0;}
	switch(REV_Function)
	{
		//~DA,240808
		case(REV_OFF_Function):{REV_OFF; PCLK_Action(IOEXP_SRC); PCLK_Action(IOEXP_FUNC); /*PCLK1_Action(); PCLK2_Action();*/ break;} // REVERB OFF
		case(REV_ON_Function) :{ REV_ON; PCLK_Action(IOEXP_SRC); PCLK_Action(IOEXP_FUNC); /*PCLK1_Action(); PCLK2_Action();*/ break;} // REVERB ON
		default:{break;}
	}
	if(Power_ON_Flag)
	{
		TDF24c04WriteOneByte(REV_Function_addr, REV_Function);
	}
	Music_Scan_Display_counter = Scan_Display_counter_Preset;
	Volume_Action_Flag = 1;
}//End of REV_Action(void)



void RS232_Write_Data_Action(void)
{
	U8 Temp_SP_Function;
	if(RS232_Addr_Code == ZSound_RS232_Power_Addr)
    {
        switch(RS232_Data_Code)
        {
            case (PowerON_Data): //Power ON
			{
				if((RS232_PowerKey_Flag == 0) && (gc_Task == 0))
				{
				  gc_Task = 1;
				  RS232_PowerKey_Flag = 1;
				}
				break;
			}
            case (PowerOFF_Data): //Power OFF
			{
				if((RS232_PowerKey_Flag == 2) && (gc_Task == 2))
				{
				  RS232_PowerKey_Flag = 3;
				  gc_poweroff = 1;
				}
				break;
			}
            default:{break;}
		}
    }
	if((RS232_PowerKey_Flag == 2) && (gc_Task == 2))//Power ON
	{
	    switch(RS232_Addr_Code)
	    {
	        case (ZSound_RS232_Music_Addr):
				{
					Music_Volume_Counter = RS232_Data_Code;
					if(Limit_Flag) {if(Music_Volume_Counter > Music_Limit) Music_Volume_Counter = Music_Limit;}
					else{if(Music_Volume_Counter > 95) Music_Volume_Counter = 95;}
					Music_Volume_Action();
					break;
				}
	        case (ZSound_RS232_Music_UPDN_Addr):
				{
					if(RS232_Data_Code == 0x00)
					{
						if(Music_Volume_Counter > 0)
						{
							Music_Volume_Counter--;
						}
						Music_Volume_Action();
					}
					if(RS232_Data_Code == 0x01)
					{
						if(Limit_Flag)
						{
							if((Music_Volume_Counter < Music_Limit) && (Music_Volume_Counter < Max_Volume))
							{
								Music_Volume_Counter++;
							}
						}
						else
						{
							if(Music_Volume_Counter < Max_Volume)
							{
								Music_Volume_Counter++;
							}
						}
						Music_Volume_Action();
					}
					break;
				}

	        case (ZSound_RS232_Mic_Addr):
				{
					Mic_Volume_Counter = RS232_Data_Code;
					if(Limit_Flag) {if(Mic_Volume_Counter > Mic_Limit) Mic_Volume_Counter = Mic_Limit;}
					else{if(Mic_Volume_Counter > 95) Mic_Volume_Counter = 95;}
					Mic_Volume_Action();
					break;
				}
	        case (ZSound_RS232_Mic_UPDN_Addr):
				{
					if(RS232_Data_Code == 0x00)
					{
						if(Mic_Volume_Counter > 0)
						{
							Mic_Volume_Counter--;
						}
						Mic_Volume_Action();
					}
					if(RS232_Data_Code == 0x01)
					{
						if(Limit_Flag)
						{
							if((Mic_Volume_Counter < Mic_Limit) && (Mic_Volume_Counter < Max_Volume))
							{
								Mic_Volume_Counter++;
							}
						}
						else
						{
							if(Mic_Volume_Counter < Max_Volume)
							{
								Mic_Volume_Counter++;
							}
						}
						Mic_Volume_Action();
					}
					break;
				}
				//:Dennis,extends 0~6,	case (ZSound_RS232_Channel_Addr):{INPUT_Function = RS232_Data_Code; if(INPUT_Function < 4) INPUT_Action(); break;}
				case (ZSound_RS232_Channel_Addr):{INPUT_Function = RS232_Data_Code; if(INPUT_Function <= 6) INPUT_Action(); break;}					//:Dennis,M
	        case (ZSound_RS232_SPA_Addr):
				{
					Temp_SP_Function = RS232_Data_Code;
					if(Temp_SP_Function < 2)
					{
						if(Temp_SP_Function)
						{
							if(SP_Function == AB_OFF) SP_Function = A_ON;
							else if(SP_Function == B_ON) SP_Function = AB_ON;
						}
						else
						{
							if(SP_Function == A_ON) SP_Function = AB_OFF;
							else if(SP_Function == AB_ON) SP_Function = B_ON;
						}
						SP_Action();
					}
					break;
				}
	        case (ZSound_RS232_SPB_Addr):
				{
					Temp_SP_Function = RS232_Data_Code;
					if(Temp_SP_Function < 2)
					{
						if(Temp_SP_Function)
						{
							if(SP_Function == AB_OFF) SP_Function = B_ON;
							else if(SP_Function == A_ON) SP_Function = AB_ON;
						}
						else
						{
							if(SP_Function == B_ON) SP_Function = AB_OFF;
							else if(SP_Function == AB_ON) SP_Function = A_ON;
						}
					SP_Action();
					}
					break;
				}

	        case (ZSound_RS232_Mute_Addr):{Mute_Function = RS232_Data_Code; if(Mute_Function < 2) Mute_Action(); break;}
	        case (ZSound_RS232_ANTI_Addr):{ANTI_Function = RS232_Data_Code; if(ANTI_Function < 2)  ANTI_Action(); break;}
	        case (ZSound_RS232_Loud_Addr):{LOUD_Function = RS232_Data_Code; if(LOUD_Function < 2)  LOUD_Action(); break;}
	        case (ZSound_RS232_Reverb_Addr):{REV_Function = RS232_Data_Code; if(REV_Function < 2)  REV_Action(); break;}
	        case (ZSound_RS232_HiCut_Addr):{HICUT_Function = RS232_Data_Code; if(HICUT_Function < 2)  HICUT_Action(); break;}

	        case (ZSound_RS232_LowCut_Addr):{LOWCUT_Function = RS232_Data_Code; if(LOWCUT_Function < 2)  LOWCUT_Action(); break;}
	        case (ZSound_RS232_WUTA_Addr):{WUTA_Function = RS232_Data_Code; if(WUTA_Function < 2)  WUTA_Action(); break;}
	        case (ZSound_RS232_AUTO_Addr):{ASONG_Function = RS232_Data_Code; if(ASONG_Function < 2)  ASONG_Action(); break;}
	        case (ZSound_RS232_STEcho_Addr):{STECHO_Function = RS232_Data_Code; if(STECHO_Function < 2)  STECHO_Action(); break;}
	        case (ZSound_RS232_MonoEcho_Addr):{ECHO_Function = RS232_Data_Code; if(ECHO_Function < 2)  ECHO_Action(); break;}

			default:{break;}//UART_Variable_Reset(); break;}
		}
	}

}//End of RS232_Action(void)

void Remote_Key(void)
{
	if((Custom_Code == TDF_R3_Custom_Code) && (Custom_Code_BAR == TDF_R3_Custom_Code_BAR))//ZSound R3
    {
        switch(Data_Code)
        {
            case (TDF_R3_Power_Code): //Power
				  {
						if((Remote_PowerKey_Flag == 2) && (gc_Task == 2))//Power OFF
						{
						  Remote_PowerKey_Flag = 3;
						  gc_poweroff = 1;
						}
						if((Remote_PowerKey_Flag == 0) && (gc_Task == 0))//Power ON
						{
						  gc_Task = 1;
						  Remote_PowerKey_Flag = 1;
						}
						break;
				  }
                  break;
            default:{break;}
		}
    }
	if ((Custom_Code == TDF_R3_Custom_Code) && (Custom_Code_BAR == TDF_R3_Custom_Code_BAR))//ZSound R3
    {
        switch(Data_Code)
        {
            case (0x54): //Power
				  {
						if((Remote_PowerKey_Flag == 2) && (gc_Task == 2))//Power OFF
						{
						  Remote_PowerKey_Flag = 3;
						  gc_poweroff = 1;
						}
						if((Remote_PowerKey_Flag == 0) && (gc_Task == 0))//Power ON
						{
						  gc_Task = 1;
						  Remote_PowerKey_Flag = 1;
						}
						break;
				  }
                  break;
            default:{break;}
		}
    }
	if((RemKey_Action_Flag == 2) && (gc_Task == 2) && (Mute_Function == 0))
    {
		if((Custom_Code == TDF_R3_Custom_Code) && (Custom_Code_BAR == TDF_R3_Custom_Code_BAR))//ZSound R3
	    {
	        switch(Data_Code)
	        {
					case (TDF_R3_Music_UP_Code): {Music_UP_Action(); break;}
					case (TDF_R3_Music_DN_Code): {Music_DN_Action(); break;}
					case (TDF_R3_Mic_UP_Code): {Mic_UP_Action(); break;}
					case (TDF_R3_Mic_DN_Code): {Mic_DN_Action(); break;}
	            	default:{break;}
			}
		}
		if(((Custom_Code==GVO_Custom_Code)&&(Custom_Code_BAR == GVO_Custom_Code_BAR))||((Custom_Code==GVN_Custom_Code)&&(Custom_Code_BAR==GVN_Custom_Code_BAR)))
		{
			switch(Data_Code)
			{
				case (GV_Music_UP_Code): {Music_UP_Action(); break;}
				case (GV_Music_DN_Code): {Music_DN_Action(); break;}
				case (GV_Mic_UP_Code): {Mic_UP_Action(); break;}
				case (GV_Mic_DN_Code): {Mic_DN_Action(); break;}
				default:{break;}
			}
		}
		if((Custom_Code == BMB_Custom_Code) && (Custom_Code_BAR == BMB_Custom_Code_BAR))//BMB
		{
			switch (Data_Code)
			{
				case (BMB_Music_UP_Code): {Music_UP_Action(); break;}
				case (BMB_Music_DN_Code): {Music_DN_Action(); break;}
				case (BMB_Mic_UP_Code): {Mic_UP_Action(); break;}
				case (BMB_Mic_DN_Code): {Mic_DN_Action(); break;}
				default:{break;}
			}
		}
	}
	if((RemKey_Action_Flag == 2) && (gc_Task == 2) && (Remote_Key_Flag))
    {
		if((Custom_Code == TDF_R3_Custom_Code) && (Custom_Code_BAR == TDF_R3_Custom_Code_BAR))//ZSound R3
		{
			switch (Data_Code)
			{
				case (TDF_R3_Chennal_UP_Code): {INPUT_Function--; INPUT_Action(); break;}
				case (TDF_R3_Chennal_DN_Code): {INPUT_Function++; INPUT_Action(); break;}
				case (TDF_R3_Mute_Code):{Mute_Function++; Mute_Action(); break;}
				case (TDF_R3_STECHO_Code):{STECHO_Function++, STECHO_Action(); break;}
				case (TDF_R3_MONOECHO_Code):{ECHO_Function++, ECHO_Action(); break;}
				case (TDF_R3_Rev_Code): {REV_Function++; REV_Action(); break;}
				case (TDF_R3_SP_Code): {SP_Function++; SP_Action(); break;}

				case (TDF_R3_Loud_Code): {LOUD_Function++; Loud_Action(); break;}
				case (TDF_R3_AUTO_Code): {ASONG_Function++; ASONG_Action(); break;}
				case (TDF_R3_WUTA_Code): {WUTA_Function++; WUTA_Action(); break;}
	
				case (TDF_R3_HICUT_Code): {HICUT_Function++; HICUT_Action(); break;}
				case (TDF_R3_LOWCUT_Code): {LOWCUT_Function++; LOWCUT_Action(); break;}
				case (TDF_R3_ANTI_Code): {ANTI_Function++; ANTI_Action(); break;}
				
				//:Dennis,A
				///////////////////////////////////////////////////////////////////////////////////
				case (TDF_R3_KTV_Channel):	{INPUT_Function = KTV; INPUT_Action(); break;}
				case (TDF_R3_DVD_Channel):	{INPUT_Function = DVD; INPUT_Action(); break;}
				case (TDF_R3_AUX_Channel):	{INPUT_Function = AUX; INPUT_Action(); break;}	
				case (TDF_R3_BT_Channel):		{INPUT_Function = BT; INPUT_Action(); break;}	
				case (TDF_R3_COAX_Channel):	{INPUT_Function = COAX; INPUT_Action(); break;}	
				case (TDF_R3_OPT1_Channel):	{INPUT_Function = OPT1; INPUT_Action(); break;}	
				case (TDF_R3_OPT2_Channel):	{INPUT_Function = OPT2; INPUT_Action(); break;}					
				///////////////////////////////////////////////////////////////////////////////////
				
				//
				default:{break;}
			}
		}
	}
}//End of Remote_Key(void)



void Remote_Repeat(void)
{
	if((RemKey_Action_Flag == 2) && (gc_Task == 2) && (Mute_Function == 0))
    {
		if(((Custom_Code==GVO_Custom_Code)&&(Custom_Code_BAR == GVO_Custom_Code_BAR))||((Custom_Code==GVN_Custom_Code)&&(Custom_Code_BAR==GVN_Custom_Code_BAR)))
		{
			switch(Data_Code)
			{
				case (GV_Music_UP_Code): {Music_UP_Action(); break;}
				case (GV_Music_DN_Code): {Music_DN_Action(); break;}
				case (GV_Mic_UP_Code): {Mic_UP_Action(); break;}
				case (GV_Mic_DN_Code): {Mic_DN_Action(); break;}
				default:{break;}
			}
		}
		if((Custom_Code == TDF_R3_Custom_Code) && (Custom_Code_BAR == TDF_R3_Custom_Code_BAR))//ZSound R3
		{
			switch (Data_Code)
			{
				case (TDF_R3_Music_UP_Code): {Music_UP_Action(); break;}
				case (TDF_R3_Music_DN_Code): {Music_DN_Action(); break;}
				case (TDF_R3_Mic_UP_Code): {Mic_UP_Action(); break;}
				case (TDF_R3_Mic_DN_Code): {Mic_DN_Action(); break;}
				default:{break;}
			}
		}
		if((Custom_Code == BMB_Custom_Code) && (Custom_Code_BAR == BMB_Custom_Code_BAR))//BMB
		{
			switch (Data_Code)
			{
				case (BMB_Music_UP_Code): {Music_UP_Action(); break;}
				case (BMB_Music_DN_Code): {Music_DN_Action(); break;}
				case (BMB_Mic_UP_Code): {Mic_UP_Action(); break;}
				case (BMB_Mic_DN_Code): {Mic_DN_Action(); break;}
				default:{break;}
			}
		}
	}
}//End of Remote_Repeat(void)



void RS232_Read_Data_Action(void)
{
	U8 temp_data;
	if((RS232_RW_Code == RS232_Read) && (RS232_Data_Code == 0x00))
	{
		switch(RS232_Addr_Code)
		{
			case(ZSound_RS232_Power_Addr):{temp_data = PowerON_Data; break;}
			case(ZSound_RS232_Music_Addr):{temp_data = Music_Volume_Counter; break;}
			case(ZSound_RS232_Mic_Addr):{temp_data = Mic_Volume_Counter; break;}
			case(ZSound_RS232_Channel_Addr):{temp_data = INPUT_Function; break;}				//:Dennis,	Input Sources:KTV,DVD,...OPT1,OPT2
			case(ZSound_RS232_SPA_Addr):
				{
					if(SP_Function == 0) temp_data = 0;
					if(SP_Function == 1) temp_data = 1;
					if(SP_Function == 2) temp_data = 1;
					if(SP_Function == 3) temp_data = 0;
					break;
				}
			case(ZSound_RS232_SPB_Addr):
				{
					if(SP_Function == 0) temp_data = 0;
					if(SP_Function == 1) temp_data = 0;
					if(SP_Function == 2) temp_data = 1;
					if(SP_Function == 3) temp_data = 1;
					break;
				}
			case(ZSound_RS232_Mute_Addr):{temp_data = Mute_Function; break;}
			case(ZSound_RS232_ANTI_Addr):{temp_data = ANTI_Function; break;}
			case(ZSound_RS232_Loud_Addr):{temp_data = LOUD_Function; break;}
			case(ZSound_RS232_Reverb_Addr):{temp_data = REV_Function; break;}

			case(ZSound_RS232_HiCut_Addr):{temp_data = HICUT_Function; break;}
			case(ZSound_RS232_LowCut_Addr):{temp_data = LOWCUT_Function; break;}
			case(ZSound_RS232_WUTA_Addr):{temp_data = WUTA_Function; break;}
			case(ZSound_RS232_AUTO_Addr):{temp_data = ASONG_Function; break;}
			case(ZSound_RS232_STEcho_Addr):{temp_data = STECHO_Function; break;}
			case(ZSound_RS232_MonoEcho_Addr):{temp_data = ECHO_Function; break;}
			
			default:{RS232_Addr_Code = 0xff; temp_data = 0xff; break;}
		}
		UART_Send(RS232_First_Code);
		UART_Send(RS232_Write);
		UART_Send(RS232_Addr_Code);
		UART_Send(temp_data);
		UART_Send(RS232_Last_Code);
	}
}//End of RS232_Read_Data_Action(void)

void Set_EEPROM_ToZero(void)
{
	U8 i;
	for(i = 0; i < 255; i++)
	{
		TDF24c04WriteOneByte(i, 0x00);
	}
}//End of Set_EEPROM_ToZero(void)

void EEPROM_Write_Defaul(void)
{
	Set_EEPROM_ToZero();
	TDF24c04WriteOneByte(Machine_code1_addr, Machine_code1_Data);
	TDF24c04WriteOneByte(Machine_code2_addr, Machine_code2_Data);
	TDF24c04WriteOneByte(Machine_code3_addr, Machine_code3_Data);
	TDF24c04WriteOneByte(Machine_code4_addr, Machine_code4_Data);

	TDF24c04WriteOneByte(SP_Function_addr, SP_Function_Defaul);
	TDF24c04WriteOneByte(ECHO_Function_addr, ECHO_Function_Defaul);
	TDF24c04WriteOneByte(STECHO_Function_addr, STECHO_Function_Defaul);
	TDF24c04WriteOneByte(LOUD_Function_addr, LOUD_Function_Defaul);
	TDF24c04WriteOneByte(HICUT_Function_addr, HICUT_Function_Defaul);
	TDF24c04WriteOneByte(LOWCUT_Function_addr, LOWCUT_Function_Defaul);
	TDF24c04WriteOneByte(REV_Function_addr, REV_Function_Defaul);
	TDF24c04WriteOneByte(ANTI_Function_addr, ANTI_Function_Defaul);

	TDF24c04WriteOneByte(Music_Limit_addr, Music_Limit_Defaul);
	TDF24c04WriteOneByte(Mic_Limit_addr, Mic_Limit_Defaul);
	TDF24c04WriteOneByte(Preset_Music_addr, Preset_Music_Defaul);
	TDF24c04WriteOneByte(Preset_Mic_addr, Preset_Mic_Defaul);
	TDF24c04WriteOneByte(Limit_Flag_addr, Limit_Flag_Defaul);
	TDF24c04WriteOneByte(Remote_Key_Flag_addr, Remote_Key_Flag_Defaul);
	TDF24c04WriteOneByte(VR_Auto_Flag_addr, VR_Auto_Flag_Defaul);
	
	//:Dennis,A
	TDF24c04WriteOneByte(DIRECTPWR_Function_addr, DIRECTPWR_Function_Defaul);
	TDF24c04WriteOneByte(INPUT_Function_addr, INPUT_Function_Defaul);	

	gw_TimerBase = 2000;
	while(gw_TimerBase) //wait
	{	   
		if(PowerOff_Set())
		return;
	}
}//End of Write_Defaul_EEPROM(void)

void Check_EEPROM(void)
{
	U8 Machine_code;
	Machine_code = TDF24c04ReadOneByte(Machine_code1_addr);
	if (Machine_code ==	Machine_code1_Data)
	{
		Machine_code = TDF24c04ReadOneByte(Machine_code2_addr);
		if (Machine_code ==	Machine_code2_Data)
		{
			Machine_code = TDF24c04ReadOneByte(Machine_code3_addr);
			if (Machine_code ==	Machine_code3_Data)
			{
				Machine_code = TDF24c04ReadOneByte(Machine_code4_addr);
				if (Machine_code ==	Machine_code4_Data)
				{
					return;
				}
				else {EEPROM_Write_Defaul(); return;}
			}
			else {EEPROM_Write_Defaul(); return;}
		}
		else {EEPROM_Write_Defaul(); return;}
	}
	else {EEPROM_Write_Defaul(); return;}
}//End of Check_EEPROM(void)


//:Dennis, check to add for TX2: read EEPROM items
void Call_EEPROM_Data(void)
{
	SP_Function      = TDF24c04ReadOneByte(SP_Function_addr);
		if (SP_Function > 3) {SP_Function = AB_ON;}									//:Dennis, invalid default
	ECHO_Function    = TDF24c04ReadOneByte(ECHO_Function_addr);
	STECHO_Function  = TDF24c04ReadOneByte(STECHO_Function_addr);
	LOUD_Function    = TDF24c04ReadOneByte(LOUD_Function_addr);
	HICUT_Function   = TDF24c04ReadOneByte(HICUT_Function_addr);
	LOWCUT_Function   = TDF24c04ReadOneByte(LOWCUT_Function_addr);
	REV_Function     = TDF24c04ReadOneByte(REV_Function_addr);
	ANTI_Function    = TDF24c04ReadOneByte(ANTI_Function_addr);
	Music_Limit      = TDF24c04ReadOneByte(Music_Limit_addr);
	Mic_Limit        = TDF24c04ReadOneByte(Mic_Limit_addr);
	Preset_Music     = TDF24c04ReadOneByte(Preset_Music_addr);
	Preset_Mic       = TDF24c04ReadOneByte(Preset_Mic_addr);
	Limit_Flag       = TDF24c04ReadOneByte(Limit_Flag_addr);
	Remote_Key_Flag  = TDF24c04ReadOneByte(Remote_Key_Flag_addr);
	VR_Auto_Flag     = TDF24c04ReadOneByte(VR_Auto_Flag_addr);

  //:Dennis,A
	preSet_DIRECTPWR = TDF24c04ReadOneByte(DIRECTPWR_Function_addr);
	//valid
	if (preSet_DIRECTPWR < 0 || preSet_DIRECTPWR > 1)
		preSet_DIRECTPWR = 0;
	preSet_INPUT_Function = TDF24c04ReadOneByte(INPUT_Function_addr);
	if (preSet_INPUT_Function < 0 || preSet_INPUT_Function > 6)
		preSet_INPUT_Function = KTV;
		
}//End of Function_Init(void)

void Set_Power_ON_VR_Preset_Action(void)
{
	NJW1159Initial();//先將NJW1159靜音
	ClearAllLED();
	Power_GreenLED_Flash_Flag = 1;

	gw_TimerBase = 1000;
	while(gw_TimerBase) //wait
	{	   
		if(PowerOff_Set())
		return;
	}
	Preset_Music = Music_Volume_Counter;
	Preset_Mic = Mic_Volume_Counter;
	
//***~DA,orig	if (Preset_Music > 0)		//-- 221215
	if (Preset_Music >= 0)		
	{
		TDF24c04WriteOneByte(Preset_Music_addr, Preset_Music);
		VR_Auto_Flag = 1;
		TDF24c04WriteOneByte(VR_Auto_Flag_addr, VR_Auto_Flag);
	}
//***~DA,orig	if (Preset_Mic > 0)			//-- 221215
	if (Preset_Mic >= 0)		
	{
		TDF24c04WriteOneByte(Preset_Mic_addr, Preset_Mic);
		VR_Auto_Flag = 1;
		TDF24c04WriteOneByte(VR_Auto_Flag_addr, VR_Auto_Flag);
	}

//~DR, orig	-- 221215
#if 0
	
	if ((Preset_Music == 0) && (Preset_Mic == 0))
	{
		TDF24c04ReadOneByte(VR_Auto_Flag_addr);											
		if (VR_Auto_Flag != 0) {VR_Auto_Flag = 0;}
		else {VR_Auto_Flag = 1;}
		TDF24c04WriteOneByte(VR_Auto_Flag_addr, VR_Auto_Flag);
	}
#endif
	
	gw_TimerBase = 2000;
	while(gw_TimerBase) //wait
	{	   
		if(PowerOff_Set())
		return;
	}	
	INPUT_Function = KTV;
	Power_GreenLED_Flash_Flag = 0;
	POWER_RedLED_ON;
	//*** PCLK1_Action();
	PCLK_Action(IOEXP_SRC);		//~DA,240808

	gw_TimerBase = 500;
	while(gw_TimerBase) //wait
	{	   
		if(PowerOff_Set())
		return;
	}
	gc_Task = 1;
	Main_Function_Loop();
//	ISPCR = ISPCR | 0x20;//	SWRST = 1;軟體重置
}//End of Set_Power_ON_VR_Preset_Action(void)


void Limit_VR_Set_Action(void)	
{
	NJW1159Initial();//先將NJW1159靜音

	ClearAllLED();
	Power_GreenLED_Flash_Flag = 1;

	gw_TimerBase = 1000;
	while(gw_TimerBase) //wait
	{	   
		if(PowerOff_Set())
		return;
	}
	Limit_Flag = Last_Limit_Flag;
	Music_Limit = Music_Volume_Counter;
	Mic_Limit = Mic_Volume_Counter;
	Preset_Music = TDF24c04ReadOneByte(Preset_Music_addr);
	Preset_Mic = TDF24c04ReadOneByte(Preset_Mic_addr);
    if (Music_Limit > 0)	
	{
		TDF24c04WriteOneByte(Music_Limit_addr, Music_Limit);
		Limit_Flag = 1;
		TDF24c04WriteOneByte(Limit_Flag_addr, Limit_Flag);
	}
	if (Mic_Limit > 0)	
	{
		TDF24c04WriteOneByte(Mic_Limit_addr, Mic_Limit);
		Limit_Flag = 1;
		TDF24c04WriteOneByte(Limit_Flag_addr, Limit_Flag);
	}
	if ((Music_Limit == 0) && (Mic_Limit == 0))
	{
		Limit_Flag = TDF24c04ReadOneByte(Limit_Flag_addr);
		if (Limit_Flag != 0) {Limit_Flag = 0;}
		else {Limit_Flag = 1;}
		TDF24c04WriteOneByte(Limit_Flag_addr, Limit_Flag);
	}
	gw_TimerBase = 2000;
	while(gw_TimerBase) //wait
	{	   
		if(PowerOff_Set())
		return;
	}	
	INPUT_Function = KTV;

	Power_GreenLED_Flash_Flag = 0;
	POWER_RedLED_ON;
	//*** PCLK1_Action();
	PCLK_Action(IOEXP_SRC);		//~DA,240808

	gw_TimerBase = 500;
	while(gw_TimerBase) //wait
	{	   
		if(PowerOff_Set())
		return;
	}
	gc_Task = 1;
	Main_Function_Loop();
//	ISPCR = ISPCR | 0x20;//	SWRST = 1;軟體重置
}//End of Limit_VR_Set_Action(void)


/***
//Orig...
void Limit_VR_Set(void)	//設定 VR 限制時的副程式
{
		
	Power_GreenLED_Flash_Flag = 1;
	Last_Limit_Flag = Limit_Flag;
	Limit_Flag = 0;

#if 0	
//~DR, orig...
	Music_Limit = 0;
	Mic_Limit = 0;
#endif
	
  //~DA, show limit set 1st	--221215
	//////////////////////////////////
	Music_Limit      = TDF24c04ReadOneByte(Music_Limit_addr);
	Mic_Limit        = TDF24c04ReadOneByte(Mic_Limit_addr);	
	//
	gw_TimerBase = 5000;
	while(gw_TimerBase) //wait
	{	   
		if(PowerOff_Set())
		return;
	}
//******* 設定調整時預設的狀態 *****************
	Call_default_Init();
//***********************************************
//~DR, 221215,
#if 0
//orig...	
	Music_Volume_Counter = 0;
	Mic_Volume_Counter = 0;
#endif
	Music_Volume_Counter = Music_Limit;
	Mic_Volume_Counter = Mic_Limit;	
	
	Music_Volume_Action();
	Mic_Volume_Action();

	Power_GreenLED_Flash_Flag = 0;
	POWER_GreenLED_ON;
	PCLK1_Action();
	Mute_Function = Mute_OFF_Function;
	SP_Function = AB_ON;
	SP_Action();

	GetFirstRotateKeyData();
	while(1)
	{
		Scan_Volume();
		SKey_Limit_OK_Switch();
	}
}//End of Limit_VR_Set(void)
***/



//~DA,
///////////////////////////////////////////////////////////////////
void Limit_VR_Set(void)
{	
	//~DA
	unsigned char cc;
	unsigned char maxdispVal;	
	unsigned char micScanBK, mainScanBK;	
	
	Power_GreenLED_Flash_Flag = 1;	
	Last_Limit_Flag = Limit_Flag;
	Limit_Flag = 0;
//~DA
/////////////////////////////////////////	

	//~DR, orig...
	//***orig, Music_Limit = 0;
	//***orig, Mic_Limit = 0;
  //
  //~DA, show limit set 1st	--221215
	//////////////////////////////////
	//*** Music_Limit      = TDF24c04ReadOneByte(Music_Limit_addr);
	//*** Mic_Limit        = TDF24c04ReadOneByte(Mic_Limit_addr);	
	//Use preset default
	Music_Limit = Preset_Music_Defaul;
	Mic_Limit = Preset_Mic_Defaul;
		

	//
	gw_TimerBase = 5000;								//wait
	while(gw_TimerBase) //wait
	{	   
		if(PowerOff_Set())
		return;
	}
	///////////////////////////////////////
	//recalls
	Call_EEPROM_Data();			//recall last setting	
	
	//init,added!
	Music_Limit = Preset_Music;
	Mic_Limit = Preset_Mic;	
	
	//validate
	if ( Music_Limit > Max_Volume)
		Music_Limit = Preset_Music_Defaul;			//valid
	if ( Mic_Limit > Max_Volume)
		Mic_Limit = Preset_Mic_Defaul;			//valid	
	
	//init
	NJW1159Initial();
	CT7302_Init();	
	//
	Limit_Flag = 0;														//clear for new limmiter setting
	MUTE_ON;																	//Muted
	STB_CTL_ON;
	//*** PCLK1_Action();
	PCLK_Action(IOEXP_FUNC);		//~DA,240808
	//VR_Auto_Flag = 1;
	
	//init 0
	Music_Volume_Counter = 0;
	Mic_Volume_Counter = 0;
	Music_Volume_Action();
	Mic_Volume_Action();	
	///////////////////////////////////////	
	
	//******* set to last setting *****************
	//active last setting, init them to
	//:do
	Call_default_Init();
	//*********************************************
	
	//~DA
	/////////////////////////////////////////
	//orig,....
	//***orig, Music_Volume_Counter = 0;
	//***orig, Mic_Volume_Counter = 0;
	Music_Volume_Counter = Music_Limit;
	Mic_Volume_Counter = Mic_Limit;
	
	
	//~DM, Moved,up here
	Power_GreenLED_Flash_Flag = 0;
	POWER_GreenLED_ON;	
	//*** PCLK1_Action();
	PCLK_Action(IOEXP_SRC);		//~DA,240808
		
	
	//---
	Mute_Function = Mute_OFF_Function;
	SP_Function = AB_ON;
	SP_Action();	

/***
	//---
	VU_ON;
	STB_CTL_ON;	
	MUTE_OFF;																	//Mute Release
	PCLK1_Action();	
***/

	gw_TimerBase = 4000;									
	while(gw_TimerBase) ; 										//wait
	
	//~DA --moved
	VU_ON;
	STB_CTL_ON;	
	MUTE_OFF;																	//Mute Release
	//*** PCLK1_Action();	
	PCLK_Action(IOEXP_SRC);		//~DA,240808
	PCLK_Action(IOEXP_FUNC);		//~DA,240808	
	
	//~DA, Ramp ups, add real audio output to SPK for Calibr.
	//////////////////////////////////////////////////
	//Simu. Scroll 7 seg#
	maxdispVal = Music_Limit;
	if ( Music_Limit >= Mic_Limit)
		maxdispVal = Music_Limit;
	else
		maxdispVal = Mic_Limit;
	
	//------
	cc = 0;
	Music_Volume_Counter = 0;
	Mic_Volume_Counter = 0;
	//
	gc_MainRotateOldKey = mainScanBK = 0;
	gc_MicRotateOldKey = micScanBK = 0;
	while ( cc < maxdispVal )
	{
		if (cc < Music_Limit)
			Music_Volume_UP_Action();
		if (cc < Mic_Limit)
			Mic_Volume_UP_Action();
					
		//
		gw_TimerBase = 50;						//25->30(v)->75->50
		while(gw_TimerBase) ; //wait

		//detect
		gc_MainRotateOldKey = P0 & 0x03;		//Music
		if (gc_MainRotateOldKey != 0)
			mainScanBK = gc_MainRotateOldKey;
		
		//
		gc_MicRotateOldKey = P0 & 0x0c;			//Mic
		if (gc_MicRotateOldKey != 0)
			micScanBK = gc_MicRotateOldKey;		
		
		//
		if((gc_MainRotateOldKey != mainScanBK) || (gc_MicRotateOldKey != micScanBK)) 	 //<-- check VR touch to brk
			break;			
		cc++;
	}
	
	//Music touched
	if(gc_MainRotateOldKey != mainScanBK)
	{
		Music_Limit = cc;
		Music_Volume_Counter = Music_Limit;	 //new start
	}
	else
	{
		Music_Volume_Counter = Music_Limit;	//last set, no update
	}
	
	//Mic touched
	if(gc_MicRotateOldKey != micScanBK)
	{
		Mic_Limit = cc;
		Mic_Volume_Counter = Mic_Limit;			//new start
	}
	else
	{
		Mic_Volume_Counter = Mic_Limit;		//last set, no update
	}
	//////////////////////////////////////////////////
	
	//reflesh
	Music_Volume_Action();
	Mic_Volume_Action();	
	//
	GetFirstRotateKeyData();
	while(1)
	{
		Scan_Volume();
		SKey_Limit_OK_Switch();
	}	
} //void Limit_VR_Set(void)
//////////////////////////////////////////////////////////////////


void Set_Power_ON_VR_Preset(void)	//設定 VR 開機時的按鍵掃描副程式
{
	unsigned char cc;
	unsigned char maxdispVal;	
	unsigned char micScanBK, mainScanBK;

	
	Power_GreenLED_Flash_Flag = 1;	
//~DA
/////////////////////////////////////////	
#if 0	
//orig,....
	Preset_Music = 0;
	Preset_Mic = 0;
#else	
	Preset_Music     = TDF24c04ReadOneByte(Preset_Music_addr);
	Preset_Mic       = TDF24c04ReadOneByte(Preset_Mic_addr);
#endif
	

    //	
	gw_TimerBase = 5000;											//wait
	while(gw_TimerBase) //wait
	{	   
		if(PowerOff_Set())
		return;
	}
	
	//recalls
	Call_EEPROM_Data();			//recall last setting		
	//validate
	if ( Preset_Music > Max_Volume)
		Preset_Music = Preset_Music_Defaul;			//valid
	if ( Preset_Mic > Max_Volume)
		Preset_Mic = Preset_Mic_Defaul;			//valid		
	
	//~DA, check limitter, 221220
	//validate with limitter of setting
	if ( Preset_Music > Music_Limit)
		Preset_Music = Music_Limit;			//valid
	if ( Preset_Mic > Mic_Limit)
		Preset_Mic = Mic_Limit;			//valid	
	
	//init
	NJW1159Initial();
	CT7302_Init();	
	
	///////////////////////////////////////
	MUTE_ON;																	//Muted
	STB_CTL_ON;
	//*** PCLK1_Action();
	PCLK_Action(IOEXP_FUNC);		//~DA,240808
	//VR_Auto_Flag = 1;
	
	//init 0
	Music_Volume_Counter = 0;
	Mic_Volume_Counter = 0;
	Music_Volume_Action();
	Mic_Volume_Action();	
	///////////////////////////////////////			
	
	//active last setting, init them to.
	//******* 設定調整時預設的狀態 *****************	
	//:do
    Call_default_Init();
	

    //***********************************************
    //~DA
    /////////////////////////////////////////
	Music_Volume_Counter = Preset_Music;
	Mic_Volume_Counter = Preset_Mic;
	
	
	//~DM, Moved,up here
	Power_GreenLED_Flash_Flag = 0;
	POWER_GreenLED_ON;	
	//*** PCLK1_Action();
	PCLK_Action(IOEXP_SRC);		//~DA,240808
		
	
	//---
	Mute_Function = Mute_OFF_Function;
	SP_Function = AB_ON;
	SP_Action();	

    /***
	//---
	VU_ON;
	STB_CTL_ON;	
	MUTE_OFF;																	//Mute Release
    PCLK1_Action();	
    ***/

	gw_TimerBase = 4000;									
	while(gw_TimerBase) ; 										//wait
	
	//~DA --moved
	VU_ON;
	STB_CTL_ON;	
	MUTE_OFF;																	//Mute Release
  	//*** PCLK1_Action();	
	PCLK_Action(IOEXP_SRC);		//~DA,240808
	PCLK_Action(IOEXP_FUNC);		//~DA,240808
	
	
	//~DA, Ramp ups, add real audio output to SPK for Calibr.
	//////////////////////////////////////////////////
	//Simu. Scroll 7 seg#
	maxdispVal = Preset_Music;
	if ( Preset_Music >= Preset_Mic)
		maxdispVal = Preset_Music;
	else
		maxdispVal = Preset_Mic;
		
	
	//------
	cc = 0;
	Music_Volume_Counter = 0;
	Mic_Volume_Counter = 0;
	//
	gc_MainRotateOldKey = mainScanBK = 0;
	gc_MicRotateOldKey = micScanBK = 0;
	while ( cc < maxdispVal )
	{
		if (cc < Preset_Music)
			Music_Volume_UP_Action();
		if (cc < Preset_Mic)
			Mic_Volume_UP_Action();
					
		//
		gw_TimerBase = 50;						//25->30(v)->75->50
		while(gw_TimerBase) ; //wait

		//detect
		gc_MainRotateOldKey = P0 & 0x03;		//Music
		if (gc_MainRotateOldKey != 0)
			mainScanBK = gc_MainRotateOldKey;
		
		//
		gc_MicRotateOldKey = P0 & 0x0c;			//Mic
		if (gc_MicRotateOldKey != 0)
			micScanBK = gc_MicRotateOldKey;		
		
		//
		if((gc_MainRotateOldKey != mainScanBK) || (gc_MicRotateOldKey != micScanBK)) 	 //<-- check VR touch to brk
			break;			
		cc++;
	}
	
	//Music touched
	if(gc_MainRotateOldKey != mainScanBK)
	{
		Preset_Music = cc;
		Music_Volume_Counter = Preset_Music;	 //new start
	}
	else
		Music_Volume_Counter = Preset_Music;	//last set
	
	//Mic touched
	if(gc_MicRotateOldKey != micScanBK)
	{
		Preset_Mic = cc;
		Mic_Volume_Counter = Preset_Mic;			//new start
	}
	else
		Mic_Volume_Counter = Preset_Mic;			//last set
	//////////////////////////////////////////////////
	
	//reflesh
	Music_Volume_Action();
	Mic_Volume_Action();

    /***		
	//orig, Moved up........................
	Power_GreenLED_Flash_Flag = 0;
	POWER_GreenLED_ON;
	PCLK1_Action();
	Mute_Function = Mute_OFF_Function;
	SP_Function = AB_ON;
	SP_Action();
    ***/		

	GetFirstRotateKeyData();
	while(1)
	{
		Scan_Volume();
		SKey_Preset_OK_Switch();
		//*** ScanKeySwitch();				//***added? -->Not necessary!
	}
}//End of Set_Power_ON_VR_Preset(void)

void Restore_Preset_Function(void)
{
	NJW1159Initial();//先將NJW1159靜音

	Power_GreenLED_Flash_Flag = 1;

	Set_EEPROM_ToZero();
	EEPROM_Write_Defaul();

	gw_TimerBase = 3000;
	while(gw_TimerBase) //wait
	{	   
		if(PowerOff_Set())
		return;
	}
	Power_GreenLED_Flash_Flag = 0;
	POWER_RedLED_ON;
	//*** PCLK1_Action();
	PCLK_Action(IOEXP_SRC);		//~DA,240808

	INPUT_Function = 0;
	gw_TimerBase = 500;
	while(gw_TimerBase) //wait
	{	   
		if(PowerOff_Set())
		return;
	}
	gc_Task = 1;
	Main_Function_Loop();
//	ISPCR = ISPCR | 0x20;//	SWRST = 1;
//	main();
}//End of Restore_Preset_Function(void)


//:Dennis,A
void preSetPOWER_LED(char ONOFF)
{
    //~DA,240808,
    if (ONOFF== 1)
		{ LOUD_ON ; PCLK_Action(IOEXP_SRC); PCLK_Action(IOEXP_FUNC); /*PCLK2_Action(); PCLK3_Action();*/} // LOUD ON		 
    else
		{ LOUD_OFF; PCLK_Action(IOEXP_SRC); PCLK_Action(IOEXP_FUNC); /*PCLK2_Action(); PCLK3_Action();*/} // LOUD OFF	
}


//:Dennis,A
//////////////////////////////////////////////////////////////
void DIRECTPower_Preset_Function(void)
{
	NJW1159Initial();//先將NJW1159靜音

	Power_GreenLED_Flash_Flag = 1;
	
	//retrieve setting
	preSet_DIRECTPWR = TDF24c04ReadOneByte(DIRECTPWR_Function_addr);
	//valid
	if (preSet_DIRECTPWR < 0 || preSet_DIRECTPWR > 1)
	{	//Error rewrite: keep Standby
		preSet_DIRECTPWR = 0;
		TDF24c04WriteOneByte(DIRECTPWR_Function_addr, preSet_DIRECTPWR);
		preSetPOWER_LED(preSet_DIRECTPWR);	
	}
    else
	{ //Normal
		preSetPOWER_LED(preSet_DIRECTPWR);				//prev:Status
		//Flips
		if (preSet_DIRECTPWR == 1)
			preSet_DIRECTPWR = 0;
		else
			preSet_DIRECTPWR = 1;
		//Set it
		TDF24c04WriteOneByte(DIRECTPWR_Function_addr, preSet_DIRECTPWR);
    }

	gw_TimerBase = 1000;
	while(gw_TimerBase) //wait
	{	   
		if(PowerOff_Set())
		return;
	}

	
	//according to Setting result, now
	if (preSet_DIRECTPWR == 1)
		preSetPOWER_LED(1);	
	else
		preSetPOWER_LED(0);
  
	//:
	Power_GreenLED_Flash_Flag = 0;
	//:Dennis	Green LED ON
	POWER_GreenLED_ON;						//:Dennis,M
	//*** PCLK1_Action();
	PCLK_Action(IOEXP_SRC);		//~DA,240808
	
	//wait LED
	gw_TimerBase = 1500;
	while(gw_TimerBase) //wait
	{	   
		if(PowerOff_Set())
		return;
	}	

	//check LOUD key is released
	ROW0 = ROW2 = ROW3 = 1; ROW1 = 0;   	//僅掃描輸出ROW1=0
	while(!COL0)						//key released
		NJWDelay(150);
	ROW0 = ROW1 = ROW2 = ROW3 = 1;   		//Clear	
	
	//go
	INPUT_Function = preSet_INPUT_Function;
	gc_Task = 1;
	gc_IrPowerOnOff = 0;							//:Dennis,A
	coldBoot = 0;											//:Dennis,fix incident Power OFF then ON
	Main_Function_Loop();
}//End of DIRECTPower_Preset_Function(void)


//:Dennis
void preSetInput_LED(char isrc, char ONOFF)
{
	Volume_Action_Flag = 0;	
	
	//:Dennis
	if(isrc < 0 || isrc > 6) 
		INPUT_Function = KTV;
	else
		INPUT_Function = isrc;
	
	//light up LED accordingly
	switch(INPUT_Function)
	{
		case(KTV):
			{
				CH_KTV; 
				if (ONOFF == 0)
					CH_LED |=0x01;
				//*** PCLK1_Action(); 
				//*** PCLK3_Action(); 
				PCLK_Action(IOEXP_SRC);		//~DA,240808
				PCLK_Action(IOEXP_FUNC);	//~DA,240808
				break;
			}			//KTV
		case(DVD):
			{
				CH_DVD;
				if (ONOFF == 0)
					CH_LED |=0x02;				
				//*** PCLK1_Action(); 
				//*** PCLK3_Action(); 
				PCLK_Action(IOEXP_SRC);		//~DA,240808
				PCLK_Action(IOEXP_FUNC);	//~DA,240808				
				break;
			}			//DVD
		case(AUX):
			{
				CH_AUX;
				if (ONOFF == 0)
					CH_LED |=0x04;				
				//*** PCLK1_Action(); 
				//*** PCLK3_Action(); 
				PCLK_Action(IOEXP_SRC);		//~DA,240808
				PCLK_Action(IOEXP_FUNC);	//~DA,240808					
				break;
			}			//AUX(VCR)
		case(BT): 
			{
				CH_BT;
				if (ONOFF == 0)
					CH_LED |=0x08;				
				//*** PCLK1_Action(); 
				//*** PCLK3_Action(); 
				PCLK_Action(IOEXP_SRC);		//~DA,240808
				PCLK_Action(IOEXP_FUNC);	//~DA,240808					
				break;
			}				//BT   
		case (COAX): 
			{
				CH_COAX;
				if (ONOFF == 0)
					CH_LED |=0x10;				
				//*** PCLK1_Action(); 
				//*** PCLK3_Action(); 
				PCLK_Action(IOEXP_SRC);		//~DA,240808
				PCLK_Action(IOEXP_FUNC);	//~DA,240808					
				break;
			}	//COAX
		case (OPT1): 
			{
				CH_OPT1; 
				if (ONOFF == 0)
					CH_LED |=0x20;				
				//*** PCLK1_Action(); 
				//*** PCLK3_Action(); 
				PCLK_Action(IOEXP_SRC);		//~DA,240808
				PCLK_Action(IOEXP_FUNC);	//~DA,240808					
				break;
			}	//OPT1, CT7302	
		case (OPT2): 
			{
				CH_OPT2;
				if (ONOFF == 0)
					CH_LED |=0x40;				
				//*** PCLK1_Action(); 
				//*** PCLK3_Action(); 
				PCLK_Action(IOEXP_SRC);		//~DA,240808
				PCLK_Action(IOEXP_FUNC);	//~DA,240808					
				break;
			}	//OPT2, CT7302
		default:{break;}
	}	
	//:---
	Music_Scan_Display_counter = Scan_Display_counter_Preset;
	Volume_Action_Flag = 1;
}//End of INPUT_SEL_Action(void)


//:Dennis,A
void INPUT_Preset_Function(char isrc)
{
	NJW1159Initial();//先將NJW1159靜音

	Power_GreenLED_Flash_Flag = 1;
	gw_TimerBase = 1000;
	while(gw_TimerBase) //wait
	{	   
		if(PowerOff_Set())
		return;
	}
	//valid
	if (isrc > 6 || isrc < 0)
		preSet_INPUT_Function = KTV;
	else
		preSet_INPUT_Function = isrc;
	//Set it
	TDF24c04WriteOneByte(INPUT_Function_addr, preSet_INPUT_Function);
	
  //Light the LED	
	preSetInput_LED(preSet_INPUT_Function, 1);
		
	//go
	Power_GreenLED_Flash_Flag = 0;
	//:Dennis,M	Green LED ON
	POWER_GreenLED_ON;						//:Dennis,M	
	//*** PCLK1_Action();
	PCLK_Action(IOEXP_SRC);		//~DA,240808

	//wait LED	
	gw_TimerBase = 1500;
	while(gw_TimerBase) //wait
	{	   
		if(PowerOff_Set())
		return;
	}

	INPUT_Function = preSet_INPUT_Function;
    while(!(COL0 & COL1 & COL2 & COL3))//若COL0~3≠1111未放開按鍵
	  NJWDelay(150);
	//:key released
	//go
	gc_Task = 1;
	gc_IrPowerOnOff = 0;							//:Dennis,A	
	coldBoot = 0;											//:Dennis,A
	Main_Function_Loop();
}//End of INPUT_Preset_Function(void)

//////////////////////////////////////////////////////////////



void Cancel_Remote_Key(void)
{
	NJW1159Initial();//先將NJW1159靜音

	Power_GreenLED_Flash_Flag = 1;

	Remote_Key_Flag++;
	if (Remote_Key_Flag > 1) Remote_Key_Flag = 0;
	TDF24c04WriteOneByte(Remote_Key_Flag_addr, Remote_Key_Flag);

	gw_TimerBase = 3000;
	while(gw_TimerBase) //wait
	{	   
		if(PowerOff_Set())
		return;
	}
	Power_GreenLED_Flash_Flag = 0;
	POWER_RedLED_ON;
	//*** PCLK1_Action();
	PCLK_Action(IOEXP_SRC);		//~DA,240808

	INPUT_Function = 0;
	gw_TimerBase = 500;
	while(gw_TimerBase) //wait
	{	   
		if(PowerOff_Set())
		return;
	}
	gc_Task = 1;
	Main_Function_Loop();
//	ISPCR = ISPCR | 0x20;//	SWRST = 1;
//	main();
}//End of Cancel_Remote_Key(void)

void Remote_Scan(void)
{
	if (NewKeyFlag)
	{
		NewKeyFlag = 0;
		Remote_Key();
	}
	if (RepeatFlag)
	{	          
		while (RepeatFlag)
		{
			RepeatFlag = 0;
			Remote_Repeat(); 
		}	  
	}
	else
	{
		if(Remote_Flag)
		{
			Remote_Flag = 0;
			Custom_Code = 0xff;
			Custom_Code_BAR = 0xff;
			Data_Code = 0xff;
		}
	}
	if (Ticks_error(Ticks))//這個不能拿掉，因為如果有解碼或訊號錯誤,要靠這個回復原始狀態。
	{
		Clear_State();
		VR_Stop();
	}
}//End of Remote_Scan(void)

void Call_default_Init(void)
{
	//:Dennis, INPUT_Function = 0;
	INPUT_Function = preSet_INPUT_Function;			//:Dennis,M
	INPUT_Action();
	if(ECHO_Function == 0) STECHO_Action();
	if(STECHO_Function == 0) ECHO_Action();
	LOUD_Action();
	HICUT_Action();
	LOWCUT_Action();
	REV_Action();
	ANTI_Action();

	ASONG_Function = 0;
	WUTA_Function = 0;
	ASONG_Action();
	WUTA_Action();
}//End of Call_default_Init(void) 


void Power_ON_Init(void)
{
    #if SEG7_SIMU
        U8 maxdispVal, cc;			//:Dennis
    #else
        U8 i;	
    #endif	

	VU_OFF;
	PWM_Dimmer_10;
	RGB_PWM_Dimmer_50;	
	//***~DD,240809,	PWM_Main(PWM_Dimmer,RGB_PWM_Dimmer);
	POWER_BlueLED_ON;
	init_taskTPreset();		//~DA,240812

	//~DA,240807
	gw_TimerBase = 100;	
	while(gw_TimerBase); //wait	

	//~DA,240808, init 7SEG
	Display_7SEG_init();
	
	//~DA,240807, init ioEXPs
	//init,exp#1, IC#0,L
	IOPORTS_DATBK[IOEXP_FUNC][0]=IOPORTS_DATBK[IOEXP_FUNC][1]=IOPORTS_DATBK[IOEXP_FUNC][2]=0x00;		//exp#1, clear BK	
    //***AC plug inited,~DD,240906,	initIOPorts(IOEXP_FUNC);	//init,exp#1
	//init,exp#2, IC#1,R
	IOPORTS_DATBK[IOEXP_SRC][0]=IOPORTS_DATBK[IOEXP_SRC][1]=IOPORTS_DATBK[IOEXP_SRC][2]=0x00;			//exp#2, clear BK
    //***AC plug inited,~DD,240906,	initIOPorts(IOEXP_SRC);		//init,exp#2	

	//:Dennis,	fixed cold start no sound
	BT_SPDIF_CTL = 0x00;				//:Dennis, 220301, cold start
	
    //	VU_ON;
	STB_ON_Action();
	
	gw_TimerBase = 500;	
	while(gw_TimerBase) //wait
	{	   
		//***		if(PowerOff_Set())			//***221220, avoid power disrupt
		//***		return;
	}
	//
	
	//:Dennis,move to below,	Call_EEPROM_Data();
	Get_SKey();
    
    //221220, Move here. change 500->1500. next loop dec 1000
    
	Power_BlueLED_Flash_Flag = 1;
	POWER_BlueLED_ON;
    
    //--- Add Power On Capacitor Charge ---//
    
    gw_TimerBase = 2800;	
	while(gw_TimerBase)
    {
        if(PowerOff_Set())		
		return;
    }
    
    STB_OFF_Action();
    
    gw_TimerBase = 1400;	
	while(gw_TimerBase)
    {
        
        if(PowerOff_Set())		
		return;
        //if(STB_SW == 0)
        //if(STB_SW == 1)
        //{
            //gc_poweroff = 0;
			//Power_BlueLED_Flash_Flag = 0;
			//ClearAllLED();
			//STB_OFF_Action();
			//gc_IrPowerOnOff = 0;
            //PowerOff_Set();		
            //return;
            //break;
            //return;
            //f(PowerOff_Set())	
            //return;
            //break;
        //}
        //else
        //{
            //gc_poweroff = 0;
			//Power_BlueLED_Flash_Flag = 0;
			//ClearAllLED();
			//STB_OFF_Action();
            //STB_ON_Action();
			//gc_IrPowerOnOff = 0;
            //PowerOff_Set();
            //if(PowerOff_Set())//return;	
            //return;
            //break;
        //}
        
    }
    
    STB_ON_Action();
    
    MUTE_ON;
	STB_CTL_ON;
    
    //--- Code End ---//  
	
  
	gw_TimerBase = 1500;		//added!, 1000 ->1500
	while(gw_TimerBase) //wait
	{	   
		//if(PowerOff_Set())					//***221220, avoid power disrupt
		//return;
	}
	//

	//***221220	Power_BlueLED_Flash_Flag = 1;				//moved upward
	//***221220	POWER_BlueLED_ON;

	gw_TimerBase = 1500;			//2000;		//orig: gw_TimerBase = 3000;			//2000->3000->2000->1500<+above1500>
	while(gw_TimerBase) //wait
	{	   
		if(PowerOff_Set())				//***221220
		return;
	}
	//:Dennis, Move here ; wait till power is steady
	Call_EEPROM_Data(); 
	
  	//	Call_default_Init();
	NJW1159Initial();//先將NJW1159靜音

	//:Dennis,A
	//init CT7302
  	CT7302_Init();
	//***debug***Dennis,	CT7302_ChSel_Action(SPDIF_COAX_IN);

	Music_Volume_Counter = 0;
	Mic_Volume_Counter = 0;
	Music_Volume_Action();
	Mic_Volume_Action();

	Call_default_Init();			//init all to preset

	//:Dennis,A before long wait, Added!
	///////////////////////////////////////
	MUTE_ON;
	STB_CTL_ON;
	//*** PCLK1_Action();
	PCLK_Action(IOEXP_FUNC);	//~DA,240808
	//*** PCLK_Action(IOEXP_SRC);		//~DA,240812,added!, //~DD,240813
	/////////////////

	gw_TimerBase = 5000;			//6000->5000
	while(gw_TimerBase) //wait
	{	   
		if(PowerOff_Set())
		return;
	}
    
	Power_BlueLED_Flash_Flag = 0;
	POWER_BlueLED_ON;
	STB_CTL_ON;
	//	VU_ON;
	//	PCLK1_Action();
    PCLK_Action(IOEXP_FUNC);	//~DA,240808
	PCLK_Action(IOEXP_SRC);		//~DA,240812,added!

	gw_TimerBase = 500;
	while(gw_TimerBase) //wait
	{	   
		if(PowerOff_Set())
		return;
	}
	Mute_Function = Mute_OFF_Function;
	//	VU_ON;
	MUTE_OFF;
	STB_CTL_ON;
	//*** PCLK1_Action();
	PCLK_Action(IOEXP_FUNC);		//~DA,240808
    //*** PCLK_Action(IOEXP_SRC);		    //~DA,240812,added! //~DD,240813
	gw_TimerBase = 1000;
	while(gw_TimerBase) //wait
	{	   
		if(PowerOff_Set())
		return;
	}
	VU_ON;
	MUTE_OFF;
	STB_CTL_ON;

	SP_Action();

    #if 0	
    //***debug
    //Add in case of Music no sound at booting...
    //extra delay 
    /////////////////////////////////////////////////////
    gw_TimerBase = 3000;			//***3000
    while(gw_TimerBase) //wait
    {	   
        if(PowerOff_Set())
        return;
    }
    /////////////////////////////////////////////////////	
    #endif	

    //=====================================================================
	
	Music_Volume_Counter = 0;
	Mic_Volume_Counter = 0;

	if(VR_Auto_Flag)
	{
		gw_TimerBase = 500;
		while(gw_TimerBase) //wait
		{
			if(PowerOff_Set())
			return;
		}
		
    //:Dennis		
    #if (SEG7_SIMU == 0)
        //orig Start right --> left 7seg#		
		for(i = 0; i < Preset_Music; i++)
		{
			Music_Volume_UP_Action();

			gw_TimerBase = 25;
			while(gw_TimerBase) //wait
			{	   
				if(PowerOff_Set())
				return;
			}
		}
		gw_TimerBase = 500;
		while(gw_TimerBase) //wait
		{
			if(PowerOff_Set())
			return;
		}
		for(i = 0; i < Preset_Mic; i++)
		{
			Mic_Volume_UP_Action();

			gw_TimerBase = 25;
			while(gw_TimerBase) //wait
			{	   
				if(PowerOff_Set())
				return;
			}
		}
    #else
		//:Dennis,M. Simu. 7 seg#
		//which would be followed
		if ( Preset_Music >= Preset_Mic)
			maxdispVal = Preset_Music;
		else
			maxdispVal = Preset_Mic;
		//:
		cc = 0;
		while ( cc < maxdispVal)	{
			if (cc < Preset_Music)
				Music_Volume_UP_Action();
			if (cc < Preset_Mic)
				Mic_Volume_UP_Action();
			//
			gw_TimerBase = 25;
			while(gw_TimerBase) //wait
			{	   
				if(PowerOff_Set())
				return;
			}	
			cc++;
		}
		//---
		gw_TimerBase = 500;
		while(gw_TimerBase) //wait
		{
			if(PowerOff_Set())
			return;
		}		
    #endif	
	}
	else//if(VR_Auto_Flag == 0)
	{
		Music_Volume_Counter = Preset_Music;
		Mic_Volume_Counter = Preset_Mic;
		Music_Volume_Action();
		Mic_Volume_Action();
	}
	Power_ON_Flag = 1;
	gc_Task = 2;
	isBoot = 0;			//clear
	//
	UART_Send(RS232_Leader_Code);
	UART_Send(RS232_Write);
	UART_Send(ZSound_RS232_Power_Addr);
	UART_Send(Power_Completed);
	UART_Send(RS232_End_Code);


}//End of Power_ON_Init(void)

//-----------------------------------------------------------------------------------
    
void PowerKeyDetect(void)
{
	U8 x_temp,x_Count,x_Success,x_fail;
	STB_SW = 1;
    
	if(~STB_SW)
	{
		x_Count = 0;
		x_Success = 1;
		x_fail = 0;
        
		while(x_Success)
		{
			NJWDelay(200);
            NJWDelay(250);		//:Dennis
			x_temp = ~STB_SW;
			if(x_temp)
			{
				x_Count++;
				if(x_Count > 15)
				{
					x_Count = 15;
				}
			}
			else
			{
				if(x_Count >= 15)
				{
					x_fail = 0;
				}
				else
				{
					x_fail = 1;
				}
				x_Success = 0;
			}
		}
		if(x_fail)
		{
			return;		//***debug, orig: none
		}
        
		//:Dennis,A avoid incident OFF
		///////////////////////////////////////////////////
		//while(STB_SW == 0) ;		
		gw_TimerBase = 1500;			//221220, 1000->1500
		while(gw_TimerBase) //wait
		{	   
			if (STB_SW == 1) 
            {
				NJWDelay(200);
				if (STB_SW == 1)
					break;						//CFM
			}
		}	
		///////////////////////////////////////////////////		
		//go
		gc_poweroff = 1;//OFF
		gc_IrPowerOnOff = 1;
	}
	else
	{
		if(gc_IrPowerOnOff)
		{
			gc_poweroff = 0;
			gc_IrPowerOnOff = 0;
		}       
	}
	
	if(UART_STATUS == 2) // Command Ready
	{
		if((RS232_Addr_Code == ZSound_RS232_Power_Addr) && (RS232_Data_Code == ((PowerON_Data) || (PowerOFF_Data))))
	    {
			RS232_Write_Data_Action();
			UART_Variable_Reset();
            
			if(gc_IrPowerOnOff)
			{
				gc_poweroff = 0;
				gc_IrPowerOnOff = 0;
				UART_Variable_Reset();
			}
		}
	}
}//End of PowerKeyDetect(void)


/*
U8 PowerOff(void)
{

   //STB_SW = 1;  
      
    if(STB_SW == 1)
	{
		return 0;
    }        
    else
	{
            STB_SW = 0;
            gc_poweroff = 0;
			Power_BlueLED_Flash_Flag = 0;
			//ClearAllLED();
			//STB_OFF_Action();
			//gc_IrPowerOnOff = 0;
			return 1;
    }
}
*/

//---------------------------------------------------------------------------------------------
U8 PowerOff_Set(void)
{
    
	U8 x_temp,x_Count,x_Success,x_fail;	
    STB_SW = 1;  
      
    if(~STB_SW)
	{
        x_Count = 0;
		x_Success = 1;
		x_fail = 0;
        
		while(x_Success)
		{
			NJWDelay(200);
            NJWDelay(250);		//:Dennis
                        
			x_temp = ~STB_SW;
			
            if(x_temp)
			{
				x_Count++;
				if(x_Count > 15)
                //if(x_Count > 2)
                {
					x_Count = 15;
                    //x_Count = 2;
                }
			}
			else
			{
				if(x_Count >= 15)
                //if(x_Count >= 2)
				{
					x_fail = 0;
				}
				else
				{
					x_fail = 1;
				}
				x_Success = 0;
			}
		}
		if(x_fail)
		{
			return 0;
		}
        
		Power_BlueLED_Flash_Flag = 0;
		gc_poweroff = 0;
		ClearAllLED();
		STB_OFF_Action();
		gc_IrPowerOnOff = 0;
		return 1;
	}
	else
	{
		if(gc_IrPowerOnOff)
		{
			NewKeyFlag = 0;  
			RepeatFlag = 0;
			Remote_Flag = 0;
			Custom_Code = 0xff;
			Custom_Code_BAR = 0xff;
			Data_Code = 0xff;
			Data_Code_BAR = 0xff;
			gc_poweroff = 0;
			Power_BlueLED_Flash_Flag = 0;
			ClearAllLED();
			STB_OFF_Action();
			gc_IrPowerOnOff = 0;
			return 1;
		}       
	}
   
	if(UART_STATUS == 2) // Command Ready
	{
		UART_main();
		if((RS232_Addr_Code == ZSound_RS232_Power_Addr) && (RS232_Data_Code == PowerOFF_Data))
	    {
			gc_poweroff = 0;
			RS232_Write_Data_Action();

			ClearAllLED();
			STB_OFF_Action();
			gc_IrPowerOnOff = 0;
			return 1;
		}
		else
		{
			UART_Variable_Reset();
		}
    }
	if(gc_IrPowerOnOff)
	{
		NewKeyFlag = 0;  
		RepeatFlag = 0;
		Remote_Flag = 0;
		Custom_Code = 0xff;
		Custom_Code_BAR = 0xff;
		Data_Code = 0xff;
		Data_Code_BAR = 0xff;
        gc_poweroff = 0;
		Power_BlueLED_Flash_Flag = 0;
		ClearAllLED();
		STB_OFF_Action();
		gc_IrPowerOnOff = 0;
		return 1;
	}
	return 0;	 
}//End of U8 PowerOff_Set(void)

//-----------------------------------------------------------------------------------
void Scan_Volume(void)
{
	MainVolumeGetRotateKey();
	MicVolumeGetRotateKey();
	MainMicRotateKeyProcess();
}//End of Scan_Volume(void)

void Main_Loop(void)
{
	GetFirstRotateKeyData();
    
    while(1)
    {
		if(UART_STATUS == 2) // Command Ready
		{
			UART_main();
			if(RS232_RW_Code == RS232_Write)
			{
				RS232_Write_Data_Action();
			}
			else if(RS232_RW_Code == RS232_Read)
			{
				RS232_Read_Data_Action();
			}
			UART_Variable_Reset();
		}
		if(gc_poweroff)
		{
			gc_poweroff = 0;
			gc_IrPowerOnOff = 0;
			ClearAllLED();
			STB_OFF_Action();
			break;
		}
		PowerKeyDetect();
		ScanKeySwitch();
		Scan_Volume();
		Remote_Scan();
		Remote_PowerKey_Flag = 2;
		RS232_PowerKey_Flag = 2;
		RemKey_Action_Flag = 2;
		RS232_Action_Flag = 2;
	}
}//End of Main_Loop(void)

void PowerONDetect(void)
{
	STB_SW = 1;
	NJWDelay(200);
	NJWDelay(200);
	
	//:Dennis,A
	preSet_DIRECTPWR = 0;
    
	if (coldBoot == 1)
	{
		//read setting: direct power ON 
		//:Dennis,A
		preSet_DIRECTPWR = TDF24c04ReadOneByte(DIRECTPWR_Function_addr);
		//valid
		if (preSet_DIRECTPWR < 0 || preSet_DIRECTPWR > 1)
			preSet_DIRECTPWR = 0;
	}
	
	//
	while(1)
	{
        //--------REMOTE-----------------------------------------------
		Remote_Scan();
        
		if(Remote_PowerKey_Flag)// || (RS232_PowerKey_Flag))
		{
			Remote_PowerKey_Flag = 2;
			break;
		}
        //--------RS-232-----------------------------------------------
		if(UART_STATUS == 2) // Command Ready
		{
			UART_main();
            
			if((RS232_Addr_Code == ZSound_RS232_Power_Addr) && (RS232_Data_Code == PowerON_Data))
		    {
				RS232_PowerKey_Flag = 2;
				RS232_Write_Data_Action();
				UART_Variable_Reset();
				break;
			}
			else
			{
				UART_Send(RS232_Leader_Code);
				UART_Send(RS232_Write);
				UART_Send(ZSound_RS232_Power_Addr);
				UART_Send(PowerOFF_Data);
				UART_Send(RS232_End_Code);
				UART_Variable_Reset();
			}
		}
        //--------Power Key-----------------------------------------------
		if(~STB_SW)
		{
			NJWDelay(200);
			NJWDelay(250);		//:Dennis		
			while(STB_SW == 0) ;
			//
			fromSTB_SW = 1;		//:Dennis, from PowerKeyPress ON. preSet is allowed
			break;
		}
		STB_OFF_Action();
        
        //:D--------PreSet Direct Power ON--------------------------------
        if(preSet_DIRECTPWR == 1 && coldBoot == 1)
		{
			//NJWDelay(250);		//:Dennis
            NJWDelay(100);
			coldBoot = 0;
			break;
		}

        //----------------------------------------------------------------
		
	}
	gc_IrPowerOnOff = 0;
	gc_Task = 1;

	//:Dennis, in case of power key detect
	NJWDelay(200);
	NJWDelay(250);		//:Dennis		
	
}//End of PowerONDetect(void)


//-----------------------------------------------------------------------------------

void STB_ON_Action(void)
{
	STB_CTL_ON;	
	//*** PCLK1_Action();
	FAN_HiSpeed;
	//*** PCLK2_Action();
	PCLK_Action(IOEXP_FUNC);		//~DA,240808
}//End of STB_ON_Action(void)


void STB_OFF_Action(void)
{
	STB_CTL_OFF;
	//*** PCLK1_Action();
	FAN_LowSpeed;
	//*** PCLK2_Action();
	PCLK_Action(IOEXP_FUNC);		//~DA,240808     
}//End of STB_OFF_Action(void)


/*
void STB_OFF(void)
{
	STB_CTL_OFF;
	//*** PCLK1_Action();
	FAN_LowSpeed;
	//*** PCLK2_Action();
	//PCLK_Action(IOEXP_FUNC);		//~DA,240808     
}
*/


//:Dennis, Migrated for SPDIF switches
//-----------------------------------------------------------------------------------

void CT7302_ChSel_Action(U8 CT7302_ChSel)
{
	
	//:Dennis, Mute it
	CT_7302_WriteOneByte(0x06,0xC8);
    
	//
	if(CT7302_ChSel > 3) CT7302_ChSel = 0;
    if(CT7302_ChSel < 0) CT7302_ChSel = 3;
    
    CT_7302_WriteOneByte(0x04, (CT7302_ChSel |= 0x30)); 
	
	//:Dennis, Mute Released
	CT_7302_WriteOneByte(0x06,0x48);
	
    //***debug	
    CT7302_ChSel = CT7302_ChSel;				//***debug,Dennis
}//End of CT7301_ChSel_Action(U8 CT7302_ChSel)


//Migrated
void CT7302_Init(void)
{
    CT_7302_WriteOneByte(0x11, 0x00);//Crystal free run
    I2CSS_Wait(5);
    CT_7302_WriteOneByte(0x13, 0x00);//Output port de-jitter set to normal
    I2CSS_Wait(5);
    CT_7302_WriteOneByte(0x14, 0x40);//Fading mode setting
    I2CSS_Wait(5);
    CT_7302_WriteOneByte(0x30, 0x23);//0x57);//0x23);//SPDIF CDR Frequency detector
    I2CSS_Wait(5);
    CT_7302_WriteOneByte(0x31, 0x19);//0x19);//0x19);//SPDIF CDR phase detector gain
    I2CSS_Wait(5);
    CT_7302_WriteOneByte(0x32, 0x1E);//0x3F);//0x1E);//SPDIF CDR_PHASE_VC_GAIN_FINE
    I2CSS_Wait(5);
    CT_7302_WriteOneByte(0x39, 0xF3);//In/Out PLL bandwidth
    I2CSS_Wait(5);
    CT_7302_WriteOneByte(0x3B, 0xFF);//In/Out PLL offset
    I2CSS_Wait(5);
    CT_7302_WriteOneByte(0x40, 0x02);//Input PLL switch to high bandwidth mode
    I2CSS_Wait(5);
    CT_7302_WriteOneByte(0x45, 0x00);//Output PLL gain setting to min
    I2CSS_Wait(5);
    CT_7302_WriteOneByte(0x47, 0xA4);//Bypass check SPDIF CDR fine lock
    I2CSS_Wait(5);
    CT_7302_WriteOneByte(0x4D, 0x37);//SPDIF new coarse offset mode enable
    I2CSS_Wait(5);
    CT_7302_WriteOneByte(0x4E, 0x72);//In/Out PLL band width
    I2CSS_Wait(5);
    CT_7302_WriteOneByte(0x61, 0x08);//Digital OSC frequency fine tune to 50MHz
    I2CSS_Wait(5);
    CT_7302_WriteOneByte(0x62, 0x01);//select to use digital OSC
    I2CSS_Wait(5);
    CT_7302_WriteOneByte(0x06, 0xC8);//Enable output mute function
    I2CSS_Wait(5);
    CT_7302_WriteOneByte(0x04, 0x30);//SRC mode 
    I2CSS_Wait(5);
    CT_7302_WriteOneByte(0x05, 0x3c);//SRC mode and Output frequency selection,3=8x(256k/352k/384k);Software SRC frequency selector,C=384kHz
    I2CSS_Wait(5);
	SPDIF_SOURCE = 0;			//init
    
}//End of CT7302_Init(void)


//:Dennis,M Migrated
void SPDIF_Select_Action(U8 SPDIF_SRC)
{

	//LED vars for indicator could be put in each case
    switch(SPDIF_SRC)
    {   
        case(SPDIF_AUX_IN):		//AUX_SPDIF_IN
            CT7302_ChSel_Action(SPDIF_AUX_IN); //SPDIF_IN_0;
            break;
        case(SPDIF_COAX_IN):	//COAX_IN
            CT7302_ChSel_Action(SPDIF_IN_1); 		//SPDIF_IN_1;           
            break;
        case(SPDIF_OPT1_IN):	//OPT1_IN
            CT7302_ChSel_Action(SPDIF_IN_2);		//SPDIF_IN_2;            
            break;
        case(SPDIF_OPT2_IN):	//OPT2_IN
            CT7302_ChSel_Action(SPDIF_IN_3);   	//SPDIF_IN_3;       
            break;		
        default:{break;}
    }
}//End of SPDIF_Select_Action(void)




void ClearAllLED(void)
{
	SPA_OFF;
	SPB_OFF;
	MUTE_ON;
	VU_OFF;
	POWER_RedLED_ON;
	//*** PCLK1_Action();
	PCLK_Action(IOEXP_SRC);		//~DA,240808
	PCLK_Action(IOEXP_FUNC);	//~DA,240808
	Power_ON_Flag = 0;
	Music_Display_Flag = 0;
	Mic_Display_Flag = 0;
    
	//
	DP1A1 = 1;		//OFF
	DP1A2 = 1;
	DP2A1 = 1;
	DP2A2 = 1;
    //
    
	RemKey_Action_Flag = 0;
	RS232_Action_Flag = 0;
	Power_RedLED_Flash_Flag = 0;
	Power_BlueLED_Flash_Flag = 0;
	Power_GreenLED_Flash_Flag = 0;

	PWM_Dimmer_0;
	RGB_PWM_Dimmer_25;
	//***~DA,240809, PWM_Main(PWM_Dimmer,RGB_PWM_Dimmer);

	ASONG_OFF;
	WUTA_OFF;
    
	/*** ~Orig,
	ECHO_CTL = 0x04;
	ECHO_LED = 0x08;
	STECHO_CTL = 0x08;
	STECHO_LED = 0x04;
	***/
    
	ECHO_OFF;
	STECHO_OFF;
	
	REV_OFF;
	HICUT_OFF;
	LOWCUT_OFF;
	ANTI_OFF;
	
	CH_LED_OFF;
	LOUD_OFF;

	CH_SEL = 0x00;

	VU_OFF;
	BT_SPDIF_CTL = 0x00;		//~DA,240814
	POWER_RedLED_ON;
	MUTE_LED = 0x10;
    
	//
	//*** ECHO_LED = 0x00;		//~DA,240910
	//*** STECHO_LED = 0x10;		//~DA,240910
    
	ECHO_OFF;				//~DA,240910
	STECHO_OFF;				//~DA,240910

	//*** PCLK1_Action();
	//*** PCLK2_Action();
	//*** PCLK3_Action();
	PCLK_Action(IOEXP_SRC);		//~DA,240808
	PCLK_Action(IOEXP_FUNC);	//~DA,240808
	//	
	UART_Variable_Reset();
	gw_TimerBase = 200;
	while(gw_TimerBase) ; //wait
    //
	gc_Task = 0;
	Remote_PowerKey_Flag = 0;
	RS232_PowerKey_Flag = 0;
}//End of ClearAllLED(void)

void Main_Function_Loop()
{
	while(1)
	{
		switch(gc_Task)
		{
			case 0:{PowerONDetect(); gc_IrPowerOnOff = 0; break;}
			case 1:{ClearAllLED(); STB_ON_Action(); Power_ON_Init(); break;}
			case 2:{Main_Loop(); break;}
			default:{break;}
		}
	}
}//End of Main_Function_Loop()


#if 0
//~DD,240912
//~DA,240813,240814
///////////////////////////
void resetIOEXP(void)
{
	//ioEXP
	//*** ioRESET = Low;
	//*** delay10us(100);				//1ms
	ioRESET = High;
}
///////////////////////////
#endif


//~DA,240808, added
/////////////////////////////////////
void PowerPlugMCU_init(void) 
{
    //MCU init
    DP1A2 = High;
    DP1A1 = High;
    DP2A2 = High;
    DP2A1 =	High;
    EN2 = High;
    EN1 = High;
    EN4 = High;
    EN3 = High;
    EVIC_ST = High;
    STB_SW = High;
    RST_MCU = High;		//ioEXP reset
    //***~DD,240912, resetIOEXP();	//ioEXP

}

////////////////////////////////////

void main(void)
{
	P0 = 0xff;
	P1 = 0xff;
	P2 = 0xff;
	P3 = 0xff;

	Power_ON_Flag = 0;

    //********************************************************************************************************
	//***Orig, IE = 0xab;      //    致能外部中斷 INT0 & INT1 , Timer 0 & 1
	IE = 0xab;      //    致能外部中斷 INT0 & INT1 , Timer 0 & 1		//~DA,240813
					//     IE7  IE6  IE5  IE4 | IE3  IE2  IE1  IE0
					//     EA   ES1  ET2  ES0 | ET1  EX1  ET0  EX0
					//      1    0    1    0  |  1    0    1    1
					//				  ^== 為 8052 才有
	
  	TMOD = 0x21;    //0x11;    //    設定 Timer 0 & 1 為工作模式 1
					//             Timer 1   |      Timer 0
					//     GATE  C/T  M1  M0 | GATE  C/T  M1  M0  <== C/T=0 當Timer , C/T=1 當Counter
					//      0     0   0   1  |  0     0   0   1
					//      M1  M0 == 00 => Mode 0 ==> 當 13 位元 計時 / 計數器
					//    * M1  M0 == 01 => Mode 1 ==> 當 16 位元 計時 / 計數器
					//      M1  M0 == 10 => Mode 2 ==> 當  8 位元 計時 / 計數器 , 自動載入
					//      M1  M0 == 11 => Mode 3 ==> 當  2 個 8 位元計時 / 計數器

	TCON = 0x07;	//    設定 INT0 , INT1 為負緣觸發
					//     TCON7  TCON6  TCON5  TCON4 | TCON3  TCON2  TCON1  TCON0
					//       X      X      X      X   |  IE1    IT1    IE0    IT0
					//       0      1      0      1   |   0      1      1      1

	T2CON = 0x00;	//    設定 INT0 , INT1 為負緣觸發
					//     T2CON7  T2CON6  T2CON5  T2CON4 | T2CON3  T2CON2  T2CON1  T2CON0
					//      TF2     EXF2    RCLK    TCLK  |  EXEN2   TR2     C/T2   CP/RL2
					//       0       0       0       0    |   0       1       0       0

 	RCAP2 = T2R = 65536 - Frequency2;

    //RCAP2H = 65536;
    //RCAP2L = 65536-Frequency2;

	IP = 0x03;		//    設定 INT0 優先於 INT1 , Timer 0 優先於 Timer 1 .
					//     IP7  IP6  IP5  IP4 | IP3  IP2  IP1  IP0
					//      x    x   PT2  PS  | PT1  PX1  PT0  PX0
					//      0    0    0    0  |  0    0    1    1

    //*********************************************************************************************************
    //UART_init();
    //UART_Variable_Reset();

	EA = High;            //致能所有中斷

	EX0 = High;
	ET0 = High;
	ET1 = High;
	ET2 = High;
	ES = High;
	PT0 = High;
	TR0 = High;
	TR1 = High;
	TR2 = High;

	//***240808, OE = 1;

	//***240808, SCLK_High;
	//***240808, PCLK1_Low;
	//***240808, PCLK2_Low;
	//***240808, PCLK3_Low;

	Clear_State();
    
	/***Orig,
	x55ms_counter = x55ms_counter_Preset;
	x220ms_counter = x220ms_counter_Preset;
	x10ms_counter = x10ms_counter_Preset;	//~DA,240808
	x5ms_counter = x5ms_counter_Preset;		//~DA,240808
	***/
	init_taskTPreset();			//~DA,240812
	
    //
	Power_RedLED_Flash_Flag = 0;
	Power_BlueLED_Flash_Flag = 0;
	Power_GreenLED_Flash_Flag = 0;
	Turbo_Volume_Counter = 0;
	Volume_Action_Flag = 0;
	Music_Display_Flag = 0;
	Mic_Display_Flag = 0;
    gc_Task = 0;
	isBoot = 1;			//~DA,240812
	toInit = 1;			//~DA,240918
	bRelMute = 0;		//~DA,240919
	FAN_LowSpeed;
    //FAN_CTL = 1;
 	STB_CTL_OFF;
	
    //***~DD,240912,	ioRESET = High;		//~DA,240906

	//*** ClearAllLED();   //~DA,240808, XIO, move downward

	UART_init();
	UART_Variable_Reset();
    //	STB_OFF_Action();
    /*
    *Orig,
	PWM_Dimmer_0;
	RGB_PWM_Dimmer_25;
	PWM_Main(PWM_Dimmer,RGB_PWM_Dimmer);

	Mute_Function = Mute_ON_Function;
	MUTE_ON;
	ASONG_OFF;
	WUTA_OFF;
	ECHO_CTL = 0x04;
	ECHO_LED = 0x08;
	STECHO_CTL = 0x08;
	STECHO_LED = 0x04;
	ASONG_Function = 0;
	WUTA_Function = 0;
	
	REV_OFF;
	HICUT_OFF;
	LOWCUT_OFF;
	ANTI_OFF;
	
	CH_LED_OFF;
	SPA_OFF;
	SPB_OFF;
	LOUD_OFF;

	CH_SEL = 0x00;

	VU_OFF;
	POWER_RedLED_ON;

 	STB_CTL_OFF;	
	
	MUTE_LED = 0x10;

	PCLK1_Action();
	PCLK2_Action();
	PCLK3_Action();
    */

    //~DA,240812, 
	gw_TimerBase = 50;		//*250	
	while(gw_TimerBase) ; 	//wait, added!
    //ioEXPs    
	//Power Plug MCU init.
	PowerPlugMCU_init();
	delay10us(200);				//5->2ms

    //init,exp#1
	IOPORTS_DATBK[IOEXP_FUNC][0]=IOPORTS_DATBK[IOEXP_FUNC][1]=IOPORTS_DATBK[IOEXP_FUNC][2]=0x00;			//exp#1, clear BK	
	initIOPorts(IOEXP_FUNC);		//init,exp#1
	//init,exp#2
	IOPORTS_DATBK[IOEXP_SRC][0]=IOPORTS_DATBK[IOEXP_SRC][1]=IOPORTS_DATBK[IOEXP_SRC][2]=0x00;			//exp#2, clear BK
	initIOPorts(IOEXP_SRC);		//init,exp#2


	//:Dennis,A assert a delay to read after power plug
	////////////////////////////////////////////////////////
	gw_TimerBase = 1000;	//*1000
	while(gw_TimerBase) ; //wait, added!
	/////////////////////////////////////////////////////////
	//read EEPROM
	Check_EEPROM();

	//~DA,240809, init 7SEG
	Display_7SEG_init();	

	//~DA,240808, inti LEDs, put here
	gw_TimerBase = 250;
	while(gw_TimerBase) ; //wait, added	
	ClearAllLED();

    //
    //NJW1159Initial();//先將NJW1159靜音
	Music_Volume_Counter = 0;
	Mic_Volume_Counter = 0;
	//***240808, OE = Low;
	//RGB_Dimmer = 0;
	//Dimmer = 0;
	fromSTB_SW = 0;
	toInit = 0;			//~DA,240918, clear

	//
	Main_Function_Loop();

}//End of main(void)




