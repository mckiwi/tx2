#include "REG_MPC82G516.H"
#include "userdefine.h"

//========Main Volume Process=================================
void    MainVolumeGetRotateKey(void);
void    MainRotateKeyTrigger(U8 x_MaingetValue);
void    MainRotateKeyDebounceEnd(U8 x_MainKeydata);
//========Mic Volume Process==================================
void    MicVolumeGetRotateKey(void);
void    MicRotateKeyTrigger(U8 x_MicgetValue);
void    MicRotateKeyDebounceEnd(U8 x_MicKeydata);
//===========================================================
void    MainMicRotateKeyProcess(void);
void    GetFirstRotateKeyData(void);
void    LeftRotary(U8 x_Main_Mic_Sel_L);
void    RightRotary(U8 x_Main_Mic_Sel_R);
//void    PowerOn_VU_Mode(void);
//===========================================================
//====================Key Define Flage========================
U8  gc_Main_Mic_Flag;
//===================Main Key Define==========================
U8  gc_MainRotateOldKey;
U8  gc_MainRotateNewKey;
U8  gc_MainRotateXorTemp;
U8  gc_MainRotateDebounceCount;
//===================Main Key Define==========================
U8  gc_MicRotateOldKey;
U8  gc_MicRotateNewKey;
U8  gc_MicRotateXorTemp;
U8  gc_MicRotateDebounceCount;
//===========================================================
//=========Main Volume========================================
U8 data  gc_ComVolume;
U8 data  gc_MicOverAllVolume;
//===========================================================
void MainVolumeGetRotateKey(void);
void MainRotateKeyTrigger(U8 x_MaingetValue);
void MainRotateKeyDebounceEnd(U8 x_MainKeydata);
void MicVolumeGetRotateKey(void);
void MicRotateKeyTrigger(U8 x_MicgetValue);
void MicRotateKeyDebounceEnd(U8 x_MicKeydata);
void MainMicRotateKeyProcess(void);
void RightRotary(U8 x_Main_Mic_Sel_R);//右旋
void LeftRotary(U8 x_Main_Mic_Sel_L);//左旋
void GetFirstRotateKeyData(void);
void Music_UP_Action(void);
void Music_DN_Action(void);
//===========================================================
//如果有變更Port及接腳,須修改有標註"需修改"的參數.
void MainVolumeGetRotateKey(void)
{
    U8 x_MainGetKeyTemp;

     
    if(gc_Main_Mic_Flag & D_MainKeyDebounce)
    {
        if(gc_MainRotateDebounceCount != 0)
        {
            gc_MainRotateDebounceCount--;
            return;
        }
    }
    x_MainGetKeyTemp = P0 & 0x03;//需修改 原0x03
    x_MainGetKeyTemp = P0 & 0x03;//需修改 原0x03
    x_MainGetKeyTemp = P0 & 0x03;//需修改 原0x03
    if(gc_MainRotateOldKey == x_MainGetKeyTemp)
    {
        gc_Main_Mic_Flag &= 0xf0;//不需修改
    }else{
           MainRotateKeyTrigger(x_MainGetKeyTemp);
         }
}//End of MainVolumeGetRotateKey(void)

void MicVolumeGetRotateKey(void)
{
    U8 x_MicGetKeyTemp;
    if(gc_Main_Mic_Flag & D_MicKeyDebounce)
    {
        if(gc_MicRotateDebounceCount != 0)
        {
            gc_MicRotateDebounceCount--;
            return;
        }
    }
    /***Orig,~DD,240809
    x_MicGetKeyTemp = P0 & 0x0c;//需修改 原0x0c
    x_MicGetKeyTemp = P0 & 0x0c;//需修改 原0x0c
    x_MicGetKeyTemp = P0 & 0x0c;//需修改 原0x0c
    ***/
    //~DA,240809,
    x_MicGetKeyTemp = (P1 & 0xc0)>>4;//需修改 0xc0 -->simu:0x0c
    x_MicGetKeyTemp = (P1 & 0xc0)>>4;//需修改 0xc0 -->simu:0x0c
    x_MicGetKeyTemp = (P1 & 0xc0)>>4;//需修改 0xc0 -->simu:0x0c

    if(gc_MicRotateOldKey == x_MicGetKeyTemp)
    {
        gc_Main_Mic_Flag &= 0x0f;//不需修改
    }else{
            MicRotateKeyTrigger(x_MicGetKeyTemp);
         }
}//End of MicVolumeGetRotateKey(void) 
//===========================================================
void MainRotateKeyTrigger(U8 x_MaingetValue)
{
    if(gc_Main_Mic_Flag & D_MainKeyDebounce)
    {
        MainRotateKeyDebounceEnd(x_MaingetValue);
    }else{
            gc_Main_Mic_Flag |= D_MainKeyDebounce;
            gc_MainRotateDebounceCount = 10;
            gc_MainRotateXorTemp = x_MaingetValue & 0x03;//需修改 原0x03
         }
}//End of MainRotateKeyTrigger(U8 x_MaingetValue)

void MicRotateKeyTrigger(U8 x_MicgetValue)
{
    if(gc_Main_Mic_Flag & D_MicKeyDebounce)
    {
        MicRotateKeyDebounceEnd(x_MicgetValue);
    }else{
            gc_Main_Mic_Flag |= D_MicKeyDebounce;
            gc_MicRotateDebounceCount = 10;
            //***Orig,//~DD,240809  gc_MicRotateXorTemp = x_MicgetValue & 0x0c;//需修改 原0x0c
            gc_MicRotateXorTemp = x_MicgetValue & 0x0c;//需修改 0xc0-->simu:0x0c
         }
}//End of MicRotateKeyTrigger(U8 x_MicgetValue)
//===========================================================
void MainRotateKeyDebounceEnd(U8 x_MainKeydata)
{
    U8 x_temp;
    x_temp = x_MainKeydata & 0x03;//需修改 原0x03
    gc_Main_Mic_Flag &= (~D_MainKeyDebounce);
    if(gc_MainRotateXorTemp ^ x_temp)
    {
        gc_Main_Mic_Flag &= 0xf0;//不需修改
        return;
    }
    gc_MainRotateNewKey = x_temp;
    gc_Main_Mic_Flag |= D_MainRotateKeySuccess;
}//End of MainRotateKeyDebounceEnd(U8 x_MainKeydata)

void MicRotateKeyDebounceEnd(U8 x_MicKeydata)
{
    U8 x_tempMic;
    //***Orig,~DD,240809,    x_tempMic = x_MicKeydata & 0x0c;//需修改 原0x0c
    //***Orig,~DM,240809, x_tempMic = x_MicKeydata & 0xc0;//需修改 原0xc0
    x_tempMic = (x_MicKeydata & 0x0c); //需修改 原0xc0 -->sumi:0x0c  //~DA,240809
    gc_Main_Mic_Flag &= (~D_MicKeyDebounce);
    if(gc_MicRotateXorTemp ^ x_tempMic)
    {
        gc_Main_Mic_Flag &= 0x0f;//不需修改
        return;
    }
    gc_MicRotateNewKey = x_tempMic;
    gc_Main_Mic_Flag |= D_MicRotateKeySuccess;
}//End of MicRotateKeyDebounceEnd(U8 x_MicKeydata)

//===========================================================
void MainMicRotateKeyProcess(void)
{
    U8 x_MainMicData,x_main_mic_sel;
    if(gc_Main_Mic_Flag & D_MainRotateKeySuccess)
    {
      gc_Main_Mic_Flag &= (~D_MainRotateKeySuccess);
      x_MainMicData = gc_MainRotateOldKey << 2;
      //~DD,240809,Orig, x_MainMicData &= 0x0c;//需修改 原0x0c
      x_MainMicData &= 0x0c;//需修改 原0xc0 -->simu:0x0c
      x_MainMicData |= gc_MainRotateNewKey;
      gc_MainRotateOldKey = gc_MainRotateNewKey & 0x03;//需修改 原0x03
      x_MainMicData &= 0x0f;//需修改 原0x0f
      x_main_mic_sel = 0;
    }else{
      if(gc_Main_Mic_Flag & D_MicRotateKeySuccess)
      {
        gc_Main_Mic_Flag &= (~D_MicRotateKeySuccess);
        x_MainMicData = gc_MicRotateOldKey;
        gc_MicRotateOldKey = gc_MicRotateNewKey;
        x_MainMicData |= gc_MicRotateNewKey >> 2;
        x_MainMicData &= 0x0f;//需修改 原0x0f
        x_main_mic_sel = 1;
      }else{ return;}
    }
//====================================================================
	switch(x_MainMicData)
	{
		case (0x01)://需修改 原0x01
	   		{
				RightRotary(x_main_mic_sel);
				break;
			}
		case (0x0b)://需修改 原0x0b
			{
				LeftRotary(x_main_mic_sel);
				break;
			}
		default:{break;}
	}
}//End of MainMicRotateKeyProcess(void)
//===========================================================
// Volume Up
//===========================================================
void RightRotary(U8 x_Main_Mic_Sel_R)//右旋
{
	switch(x_Main_Mic_Sel_R)
	{
		//*****************************************************
		// Music Volume Up
		//*****************************************************
		case (0x00)://不需修改
			{
				Volume_Action_Flag = 1;
				Music_Volume_UP_Action();
				break;
            }
		//*****************************************************
		// Mic Volume Up
		//*****************************************************
		case (0x01)://不需修改
			{
				Volume_Action_Flag = 1;
				Mic_Volume_UP_Action();
				break;
			}
		default:{break;}
	}
}//End of RightRotary(U8 x_Main_Mic_Sel_R)//右旋
//===========================================================
// Volume Down
//===========================================================
void LeftRotary(U8 x_Main_Mic_Sel_L)//左旋
{
	switch(x_Main_Mic_Sel_L)
	{
		//*****************************************************
		// Music Volume Down
		//*****************************************************
		case (0x00)://不需修改
            {
				Volume_Action_Flag = 1;
				Music_Volume_DN_Action();
				break;
            }
		//*****************************************************
		// Mic Volume Down
		//*****************************************************
		case (0x01)://不需修改
			{
				Volume_Action_Flag = 1;
				Mic_Volume_DN_Action();
				break;
			}
		default:{break;}
	}
}//End of LeftRotary(U8 x_Main_Mic_Sel_L)//左旋

void GetFirstRotateKeyData(void)
{
    //=======Clear Key=======================================
	gc_Main_Mic_Flag = 0;
	gc_MainRotateOldKey = 0;
	gc_MainRotateXorTemp = 0;
	gc_MainRotateNewKey = 0;
	gc_MainRotateDebounceCount = 0;

	gc_MicRotateOldKey = 0;
	gc_MicRotateXorTemp = 0;
	gc_MicRotateNewKey = 0;
	gc_MicRotateDebounceCount = 0;
//=======================================================
	gc_MainRotateOldKey = P0 & 0x03;//需修改 原0x03
	gc_MainRotateOldKey = P0 & 0x03;//需修改 原0x03
	gc_MainRotateOldKey = P0 & 0x03;//需修改 原0x03

	//~DD,240809,Orig
    /**
    gc_MicRotateOldKey = P0 & 0x0c;//需修改 原0x0c
	gc_MicRotateOldKey = P0 & 0x0c;//需修改 原0x0c
	gc_MicRotateOldKey = P0 & 0x0c;//需修改 原0x0c
    **/
	gc_MicRotateOldKey = (P1 & 0xc0)>>4;//需修改 原0xc0 -->simu:0x0c    //~DA,240809
	gc_MicRotateOldKey = (P1 & 0xc0)>>4;//需修改 原0xc0 -->simu:0x0c    //~DA,240809
	gc_MicRotateOldKey = (P1 & 0xc0)>>4;//需修改 原0xc0 -->simu:0x0c    //~DA,240809
}//End of GetFirstRotateKeyData(void)
//===========================================================

























