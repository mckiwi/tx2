#include "REG_MPC82G516.H"
#include "userdefine.h"

unsigned char count, Scan_Key_Data;//, ROW;  //基數=0
void dataout(char keyout);
void ScanKeySwitch(void);
//char code TABLE[]={"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};//陣列字元資料 
void ScanSKeySwitch(void);
void Sdataout(char Skeyout);
void UDKeydataout(char UDkeyout);
void ScanUDKeySwitch(void);

//:Dennis,A externs
extern char	preSet_DIRECTPWR, preSet_INPUT_Function;
extern char fromSTB_SW;




void Sdataout(char Skeyout)
{
	U8 Skey_Number;//,i;
	Skey_Number = Skeyout;	  //按鍵資料由LED輸出
	POWER_GreenLED_ON;
	//*** PCLK1_Action();
	PCLK_Action(IOEXP_SRC);	  //~DA,240808
	PCLK_Action(IOEXP_FUNC);	//~DA,240808,added!

	NJWDelay(150);
	if ((Skeyout != 6) && (Skeyout != 7))					//:Dennis,A
			while(!(COL0 & COL1 & COL2 & COL3))//若COL0~3≠1111未放開按鍵
	NJWDelay(150);
	switch(Skey_Number)
	{
		case(0):{Limit_VR_Set(); break;}
		case(1):{Set_Power_ON_VR_Preset(); break;}
		case(2):{Restore_Preset_Function(); break;}
		case(3):{Cancel_Remote_Key(); break;}
		case(4):{Limit_VR_Set_Action(); break;}
		case(5):{Set_Power_ON_VR_Preset_Action(); break;}
		//:Dennis,A
		case(6):
			{ //:Dennis, set Preset directPower, not allow from directPowerON
				if (fromSTB_SW == 1)
					DIRECTPower_Preset_Function(); 
				break;
			}														
		case(7):
			{ //:Dennis, set Preset Input source, not allow from directPowerON
				if (fromSTB_SW == 1)
					INPUT_Preset_Function(preSet_INPUT_Function); 
				break;
			}	
		default:{break;}
	}
}//End of Sdataout(char Skeyout)

void SKey_Limit_OK_Switch(void)
{
//	ROW=0xff;
	ROW0 = ROW1 = ROW2 = ROW3 = 1; ROW0 = 0; //僅掃描輸出ROW0=0
	if(COL3 == 0) Sdataout(4);//若檢查COL3 = 0，按鍵資料輸出 = 4
}//End of ScanSKey_OK_Switch(void)

void SKey_Preset_OK_Switch(void)
{
//	ROW=0xff;
	ROW0 = ROW1 = ROW2 = ROW3 = 1; ROW0 = 0; //僅掃描輸出ROW0=0
	if(COL3 == 0) Sdataout(5);//若檢查COL3 = 0，按鍵資料輸出 = 5
}//End of ScanSKey_OK_Switch(void)

void Get_SKey(void)
{
//	ROW=0xff;

	ROW0 = ROW1 = ROW2 = ROW3 = 1; ROW3 = 0;   //僅掃描輸出ROW3=0
	if((COL0 == 0) && (COL1 == 0)) Sdataout(0);//若檢查COL0=0,COL1=0，按鍵資料輸出 = 0
	if((COL2 == 0) && (COL3 == 0)) Sdataout(1);//若檢查COL2=0,COL3=0，按鍵資料輸出 = 1

	ROW0 = ROW1 = ROW2 = ROW3 = 1; ROW2 = 0;   //僅掃描輸出ROW2=0
	if((COL0 == 0) && (COL1 == 0)) Sdataout(2);//若檢查COL0=0,COL1=0，按鍵資料輸出 = 2
	if((COL2 == 0) && (COL3 == 0)) Sdataout(3);//若檢查COL2=0,COL3=0，按鍵資料輸出 = 3
	
	//:addFunct
	//:Dennis,A -- preSet directPower, preSet Input source
	/////////////////////////////////////////////////////////////////////////////////////////////		
	ROW0 = ROW2 = ROW3 = 1; ROW1 = 0;   			//僅掃描輸出ROW1=0
  if (COL0 == 0) Sdataout(6);	//:Dennis, preSet directPower

	ROW1 = ROW2 = ROW3 = 1; ROW0 = 0;   			//僅掃描輸出ROW0=0 BT,AUX,DVD,KTV
	if (COL3 == 0) { preSet_INPUT_Function = KTV ; Sdataout(7); }
	if (COL2 == 0) { preSet_INPUT_Function = DVD ; Sdataout(7); }	
	if (COL1 == 0) { preSet_INPUT_Function = AUX ; Sdataout(7); }	
	if (COL0 == 0) { preSet_INPUT_Function = BT ; Sdataout(7); }

	ROW0 = ROW2 = ROW3 = 1; ROW1 = 0;   			//僅掃描輸出ROW0=1 COAX,OPT1,OPT2
	if (COL3 == 0) { preSet_INPUT_Function = COAX ; Sdataout(7); }
	if (COL2 == 0) { preSet_INPUT_Function = OPT1 ; Sdataout(7); }	
	if (COL1 == 0) { preSet_INPUT_Function = OPT2 ; Sdataout(7); }	

	//:clear
	ROW0 = ROW1 = ROW2 = ROW3 = 1;   			//Clear
	/////////////////////////////////////////////////////////////////////////////////////////////	
	
}//End of ScanSKeySwitch(void)

void ScanKeySwitch(void)
{
//	ROW=0xff;
	ROW0 = ROW1 = ROW2 = ROW3 = 1; ROW0 = 0; //僅掃描輸出ROW0=0
	if(COL3 == 0) dataout(0);//若檢查COL0=0，按鍵資料輸出=0
	if(COL2 == 0) dataout(1);//若檢查COL1=0，按鍵資料輸出=1
	if(COL1 == 0) dataout(2);//若檢查COL2=0，按鍵資料輸出=2
	if(COL0 == 0) dataout(3);//若檢查COL3=0，按鍵資料輸出=3
	   
	ROW0 = ROW1 = ROW2 = ROW3 = 1; ROW1 = 0; //僅掃描輸出ROW1=0
	if(COL3 == 0) dataout(4);//若檢查COL0=0，按鍵資料輸出=4
	if(COL2 == 0) dataout(5);//若檢查COL1=0，按鍵資料輸出=5
	if(COL1 == 0) dataout(6);//若檢查COL2=0，按鍵資料輸出=6
	if(COL0 == 0) dataout(7);//若檢查COL3=0，按鍵資料輸出=7
	   
	ROW0 = ROW1 = ROW2 = ROW3 = 1; ROW2 = 0;   //僅掃描輸出ROW2=0
	if(COL3 == 0) dataout(8);  //若檢查COL0=0，按鍵資料輸出=8
	if(COL2 == 0) dataout(9);  //若檢查COL1=0，按鍵資料輸出=9
	if(COL1 == 0) dataout(0x0a);//若檢查COL2=0，按鍵資料輸出=A
	if(COL0 == 0) dataout(0x0b);//若檢查COL3=0，按鍵資料輸出=B
	  
	ROW0 = ROW1 = ROW2 = ROW3 = 1; ROW3 = 0;  //僅掃描輸出ROW3=0
	if(COL3 == 0) dataout(0x0c);//若檢查COL0=0，按鍵資料輸出=C
	if(COL2 == 0) dataout(0x0d);//若檢查COL1=0，按鍵資料輸出=D
	if(COL1 == 0) dataout(0x0e);//若檢查COL2=0，按鍵資料輸出=E
	if(COL0 == 0) dataout(0x0f);//若檢查COL3=0，按鍵資料輸出=F

	//:clear
	ROW0 = ROW1 = ROW2 = ROW3 = 1;   			//Clear

}//End of ScanKeySwitch(void)


//
//:Dennis,revised for TX-2  //Might need to take care of "SBUF=TABLE[n]"
//
void dataout(char keyout)
{
	U8 key_Number;//,i;
	key_Number = keyout;	  //按鍵資料由LED輸出
	NJWDelay(150);
  while(!(COL0 & COL1 & COL2 & COL3))//若COL0~3≠1111未放開按鍵
	  NJWDelay(150);
	switch(key_Number)
	{
		case(0):{INPUT_Function = KTV; INPUT_Action(); break;}
		case(1):{INPUT_Function = DVD; INPUT_Action(); break;}
		case(2):{INPUT_Function = AUX; INPUT_Action(); break;}
		case(3):{INPUT_Function = BT; INPUT_Action(); break;}
		case(4):{INPUT_Function = COAX; INPUT_Action(); break;}	
		case(5):{INPUT_Function = OPT1; INPUT_Action(); break;}
		case(6):{INPUT_Function = OPT2; INPUT_Action(); break;}				
		case(7):{LOUD_Function++; Loud_Action(); break;}
		case(8):{//SBUF=TABLE[7];  	-->//SPA
				  //SPA
					if(SP_Function == AB_OFF) SP_Function = A_ON;
					else if(SP_Function == A_ON) SP_Function = AB_OFF;
						else if(SP_Function == B_ON) SP_Function = AB_ON;
							else if(SP_Function == AB_ON) SP_Function = B_ON;
					SP_Action();			
					break;
				}
		case(9):{//SBUF=TABLE[6]; 	-->//SPB
					//SPB
					if(SP_Function == AB_OFF) SP_Function = B_ON;
					else if(SP_Function == A_ON) SP_Function = AB_ON;
						else if(SP_Function == B_ON) SP_Function = AB_OFF;
							else if(SP_Function == AB_ON) SP_Function = A_ON;
					SP_Action();			
					break;
				}
		case(10):{STECHO_Function++; STECHO_Action(); break;}
		case(11):{ECHO_Function++; ECHO_Action(); break;}

		case(12):{REV_Function++; REV_Action(); break;}
		case(13):{HICUT_Function++; HICUT_Action(); break;}
		case(14):{LOWCUT_Function++; LOWCUT_Action(); break;}
		case(15):{ANTI_Function++; ANTI_Action(); break;}
		default:{break;}
	}
}//End of dataout(char keyout)




