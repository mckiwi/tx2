#include "REG_MPC82G516.H"
#include "userdefine.h"

#if 0
//~DA,240809, previous Ver. droppped
void PWM_Main(U8 PWM_Pulse,U8 RGB_PWM_Pulse);

void PWM_Main(U8 PWM_Pulse,U8 RGB_PWM_Pulse)
{
	#if 0	
		//~Dennis
		//~DD,240809, Conflict and impacts to Mic 7SEG display, Same: P1^1,P1^2
		CCAPM0 = CCAPM1 = ECOM + PWM; //致能 CEX0,CEX1 比較器及PWM輸出
    	CMOD = 0x00; //CPS1-0 = 00, Fpwm = Fosc/12/256 => 11.0592MHz/12/256 = 3.6KHz (18.432MHz/12/256 = 6KHz)
		CR = 1;      //開始PCA計數
		CCAP0H = PWM_Pulse; //設定CEX0的PWM脈波時間 0~255
		CCAP1H = RGB_PWM_Pulse; //設定CEX1的PWM脈波時間 0~255
	#endif	
}
#endif




















