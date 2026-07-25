#include "REG_MPC82G516.H"
#include "userdefine.h"

void NJW1159SetData(U8 x_Volume, U8 x_Channel);
void NJW1159VolumeControlSet(U16 x_NjwTemp);
void NJW1159Initial(void);

void NJW1159SetData(U8 x_Volume,U8 x_Channel)//x_Channel = Music_Volume or Mic_Volume
{
    U16 x_NjwDataTemp;
	if(x_Volume > (Max_Volume - 1)) x_Volume = 0xff;//音量小於1時令NJW1159 Mute
    x_Volume <<= 1;
    x_NjwDataTemp = (U16)x_Volume;
    x_NjwDataTemp <<= 8;
    x_NjwDataTemp &= 0xfe00;
    switch(x_Channel)
    {
        case (0x00):
		{
             NJW1159VolumeControlSet((x_NjwDataTemp | 0x0000));//Volume 1	 Music_Volume_RCH
             NJW1159VolumeControlSet((x_NjwDataTemp | 0x0010));//Volume 2	 Music_Volume_LCH
             break;
		}
        case (0x01):
		{
             NJW1159VolumeControlSet((x_NjwDataTemp | 0x0003));//Volume 1	 Mic_Volume_RCH
             NJW1159VolumeControlSet((x_NjwDataTemp | 0x0013));//Volume 2	 Mic_Volume_LCH
             break;
		}
        default:{break;}
    }
}//End of NJW1159SetData(U8 x_Volume,U8 x_Channel)

void NJW1159VolumeControlSet(U16 x_NjwTemp)
{
    U8 i;
	NJW1159_LATCH_LOW;
	NJWDelay(2);
	for(i = 0; i < 16; i++)
	{
	   if(x_NjwTemp & 0x8000)
	    	NJW1159_SDA_HI;
	   else
	    	NJW1159_SDA_LW;
	
	    NJWDelay(1);
	    NJW1159_CLK_LW;
	    NJWDelay(1);
	    NJW1159_CLK_HI;
	    x_NjwTemp <<= 1;
	}

	NJW1159_LATCH_HI;

}//End of NJW1159VolumeControlSet(U16 x_NjwTemp)
//============================================================
// Set Mute
//============================================================
void NJW1159Initial(void)
{
    NJW1159VolumeControlSet(0xbf00);//Music Volume RCH
    NJW1159VolumeControlSet(0xbf10);//Music Volume LCH

    NJW1159VolumeControlSet(0xbf03);//Mic Volume RCH
    NJW1159VolumeControlSet(0xbf13);//Mic Volume LCH
}//End of NJW1159Initial(void)
//============================================================



