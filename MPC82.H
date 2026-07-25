/*-----------------------------------------------
MPC82G.H
-----------------------------------------------*/
//重新定義變數的型態名稱
typedef unsigned char   uint8;  // 無符號8-bit整數變數       
typedef signed   char  	int8;   // 有符號8-bit整數變數       
typedef unsigned int 	uint16; // 無符號16-bit整數變數      
typedef signed   int 	int16;  // 有符號16-bit整數變數      
typedef unsigned long  	uint32; // 無符號32-bit整數變數      
typedef signed   long  	int32;  // 有符號32-bit整數變數      
typedef float          	fp32;   // 單精度浮點數(32bit)

//重新定義Keil內部<intrins.h>函數
#include  <intrins.h>  //包括邏輯運算函數庫設定
#define RR8(x)  _cror_(x,1); //定義8-bit變數x右旋轉1-bit
#define RR16(x) _iror_(x,1); //定義16-bit變數x右旋轉1-bit
#define RR32(x) _lror_(x,1); //定義32-bit變數x右旋轉1-bit

#define RL8(x)  _crol_(x,1); //定義8-bit變數x左旋轉1-bit
#define RL16(x) _irol_(x,1); //定義16-bit變數x左旋轉1-bit
#define RL32(x) _lrol_(x,1); //定義32-bit變數x左旋轉1-bit

#define NOP() _nop_();  //定義空指令，延時一個機械週期時間
//----------------------------------------------
//----------------------------------------------
//------------------------------------------------
#define IDL    0x01  //1=進入idle省電模式
#define PD     0x02  //1=進入power down省電模式  
#define GF0    0x04  //通用存取位元
#define GF1    0x08  //通用存取位元
#define POF    0x10  //冷開機重置旗標，1=為冷開機所產生的重置(預定)
#define SMOD0  0x40  //1=設定UART1為增強型UART
#define SMOD   0x80  //1=UART1鮑率加倍
//------------------------------------------------
//------------------------------------------------
#define T0_M0   0x01 //Timer0模式位元0
#define T0_M1   0x02 //Timer0模式位元1
#define T0_CT   0x04 //0=Timer0為內部計時，1=外部計數
#define T0_GATE 0x08 //1=Timer0由INT0腳(P32)啟動
#define T1_M0   0x10 //Timer1模式位元0
#define T1_M1   0x20 //Timer1模式位元1
#define T1_CT   0x40 //0=Timer1為內部計時，1=外部計數
#define T1_GATE 0x80 //1=Timer1由INT1腳(P33)啟動
//------------------------------------------------
//------------------------------------------------
//------------------------------------------------

//------------------------------------------------

/* Timer2*暫存器 */
#define DCEN    0x01  //1=Timer2下數計數致能
#define T2CKOE  0x02  //1=Timer2時脈輸出致能

//sfr T2CON = 0xC8;   //Timer2控制暫存器
//sbit CP_RL2= T2CON^0;  //0=重新載入(Reload),1=捕捉(Capture )
//sbit C_T2 =  T2CON^1;   //0=計時(Timer),1=計數(Counter)
//sbit TR2  =  T2CON^2;   //1=開始計時
//sbit EXEN2=  T2CON^3;  	//1=致能T2EX腳輸入負緣觸發
//sbit TCLK =  T2CON^4;   //0=串列埠發射時脈使用Timer1，1=使用Timer2
//sbit RCLK =  T2CON^5;   //0=串列埠接收時脈使用Timer1，1=使用Timer2 
//sbit EXF2 =  T2CON^6;   //T2EX腳輸入旗標
//sbit TF2  =  T2CON^7;   //Timer2溢位旗標

sfr16  RCAP2  = 0xCA; //Timer2重新載入/捕捉暫存器
//sfr RCAP2L  = 0xCA;   //Timer2重新載入/捕捉暫存器低位元組
//sfr RCAP2H  = 0xCB;   //Timer2重新載入/捕捉暫存器高位元組

sfr16 T2R   = 0xCC;   //Timer2計時/計數暫存器
//sfr TL2     = 0xCC;   //Timer2計時/計數暫存器低位元組
//sfr TH2     = 0xCD;   //Timer2計時/計數暫存器高位元組
//------------------------------------------------
/*------------------------------------------------
中斷向量位址 = (中斷向量編號 * 8) + 3
------------------------------------------------*/
#define IE0_VECTOR	  0  //INT0外部中斷，中斷向量位址0x03
#define TF0_VECTOR	  1  //Timer0溢位中斷，中斷向量位址0x0B
#define IE1_VECTOR	  2  //INT1外部中斷，中斷向量位址0x13
#define TF1_VECTOR	  3  //Timer1溢位中斷，中斷向量位址0x1B
#define UART_VECTOR   4  //UART1傳輸中斷，中斷向量位址0x23
#define TF2_VECTOR	  5  //Timer2溢位中斷，中斷向量位址0x2B
#define EX2_VECTOR	  5  //Timer2外部中斷，中斷向量位址0x2B
#define IE2_VECTOR    6  //INT2外部中斷，中斷向量位址0x33
#define IE3_VECTOR    7  //INT3外部中斷，中斷向量位址0x3B	
#define SPI_VECTOR    8  //SPI中斷，中斷向量位址0x43
#define ADC_VECTOR    9  //ADC中斷，中斷向量位址0x4B
#define PCA_VECTOR    10 //PCA中斷，中斷向量位址0x53
#define LVI_VECTOR    11 //低電源電壓中斷，中斷向量位址0x5B
#define UART2_VECTOR  12 //UART2傳輸中斷，中斷向量位址0x63
#define KBI_VECTOR    13 //Keypad中斷，中斷向量位址0x6B	

//*******MPC82特殊暫功能存器****************************
/*接腳模式暫存器*/
#define  OPF   0x01  //1=當VDD腳輸入電壓低於3.7V時
#define  CPF   0x02  //1=當V3O腳輸出電壓低於2.4V時
#define  EOPFI 0x04  //1=致能當VDD腳輸入電壓低於3.7V時產生中斷
#define  ECPFI 0x08  //1=致能當V3O腳輸出電壓低於2.4V時產生中斷


/*輔助暫存器*/
#define     EXTRAM 0x02   //1=禁能ERAM，0=致能內部ERAM位址0x000~0FFF
#define     P35ALE  0x10  //1=P35為ALE腳，0=P35為IO腳
#define     P41ALE  0x20  //1=P41為ALE腳，0=P41為IO腳
#define     ADRJ    0x40  //1=數位資料ADCH向右移，0=數位資料ADCH向左移
#define     URTS    0x80  //1=致能S2BRT提供UART1速率時脈

#define     DPS     0x01  //使用的DPTR，0=DPTR0，1=DPTR1 
#define     GF2     0x08  //通用位元旗標 
#define     P4S2    0x10  //1=P4.2及P4.3為UART2，0=P1.2及P1.3為UART2
#define     P4SPI   0x20  //1=P4.7-4.3為SPI，0=P1.7-4為SPI 
#define     P4PCA   0x40  //1=P4為PCA，0=P1為PCA 
#define     P4KB    0x80  //1=P4為 keypad，0=P2為 keypad

#define ESPI  0x01  //SPI中斷致能
#define EADC  0x02  //ADC中斷致能
#define EPCA  0x04  //PCA中斷致能
#define EBD   0x08  //電源偵測中斷致能
#define ES2   0x10  //UART2中斷致能
#define EKBI  0x20  //按鍵中斷致能

#define PSPI  0x01  // SPI中斷優先
#define PADC  0x02  // ADC中斷優先
#define PPCA  0x04  // PCA中斷優先
#define PBD   0x08  // 電源偵測中斷優先
#define PS2   0x10  // UART2中斷優先
#define PKB   0x20  // 按鍵中斷優先

#define PSPIH 0x01  //SPI中斷高優先
#define PADCH 0x02  //ADC中斷高優先
#define PPCAH 0x04  //PCA中斷高優先
#define PBDH  0x08  //電源偵測中斷高優先
#define PS2H  0x10  //UART2中斷高優先
#define PKBH  0x20  //Keypad中斷高優先，1=高優先 

#define T0CKOE  0x01  //1=致能T0溢位時由P34腳反相輸出 
#define S2CKOE  0x02  //1=致能UART2速率時脈由P35腳輸出 
#define S2TX12  0x04  //1=UART2時脈為系統頻率，0=UART2時脈為系統頻率/12  
#define S2SMOD  0x08  //1=UART2速率加倍 
#define S2TR    0x10  //1=UART2啟動位元
#define URM0X6  0x20  //1=系統頻率/2，0=UART mode0時脈為系統頻率/12
#define T1X12   0x40  //1=T1時脈為系統頻率，0=T1時脈為系統頻率/12
#define T0X12   0x80  //1=T0時脈為系統頻率，0=T0時脈為系統頻率/12

/* UART2暫存器 */
/*
#define S2RI    0x01  //UART2接收旗標，接收完成時，RI=1
#define S2TI    0x02  //UART2發射旗標，發射完成時，TI=1
#define S2RB8   0x04  //UART2模式2、3中，存放接收到的第9bit 
#define S2TB8   0x08  //UART2模式2、3中，發射資料的第9bit
#define S2REN   0x10  //UART2接收致能，REN=1允許接收串列資料
#define S2SM2   0x20  //UART2工作模式設定
#define S2SM1   0x40 
#define S2SM0   0x80
*/

#define SCKD1   0  //SCKD3-0=系統除頻控制位元
#define SCKD2   1
#define SCKD4   2
#define SCKD8   3
#define SCKD16  4
#define SCKD32  5
#define SCKD64  6
#define SCKD128 7

/* Keypad暫存器*/
#define KBIF  0x01       //鍵中斷Keypad中斷旗標
#define PATNS  0x02       //Keypad有比較功能


/* WDT暫存器  */
#define PS2_0 0x01  //Fosc預除倍數，000=Fosc/2, 001=Fosc/4, 010=Fosc/8,  011=Fosc/16,
                    //        100=Fosc/32,101=Fosc/64,110=Fosc/128,111=Fosc/256
#define WIDL  0x08  //1=在Idle模式WDT仍計數，0=在Idle模式WDT停止計數
#define CLRW  0x10  //1=清除WDT計數器為0
#define ENW   0x20  //1=致能WDT計數
#define WRF   0x80	//1=WDT溢位重置(reset)旗標

/* ADC暫存器  */
#define ADCON 0x80  // ADC電源，1=將電源送入ADC工作，0=停止ADC工作
#define ADCI  0x10  // ADC中斷旗標，1=ADC轉換完成
#define ADCS  0x08  // ADC啟動，1=啟動ADC開始轉換 


/*  SPI 暫存器 */
	  
sfr SPSTAT = 0x84;    //SPI狀態暫存器
#define WCOL  0x40    //SPI寫入碰撞旗標,0=SPI發射資料緩衝器是空的 
#define SPIF  0x80    //SPI中斷旗標

sfr SPCTL  = 0x85;    //SPI控制暫存器							
#define SPR  0x01     //Fosc預除倍數(SPR1-0)
                      //0=Fosc/4,1=Fosc/16,2=Fosc/64,3=Fosc/128                     
#define CPHA  0x04    //SPI時脈相位選擇
#define CPOL  0x08    //SPI時脈極性選擇,1=SPICLK is high when Idle
#define MSTR  0x10    //Master/Slave模式選擇
#define DORD  0x20    //SPI資料位元順序,1=bit0先傳輸, 0=bit7先傳輸,
#define SPEN  0x40    //1=SPI致能
#define SSIG  0x80    //1=忽略SS腳晶片選擇 
  
/* ISP及IAP暫存器*/ 

#define  READ    0x01  // 01=讀取
#define  WRITE   0x02  // 10=寫入
#define  ERASE   0x03  // 11=清除


#define  IAPEN     0x80   //1=致能IAP/ISP功能
#define  CFAIL     0x10   //0=IAP/ISP命令完成，1=IAP/ISP命令失敗
#define  Fosc_11M  0x03   //寫入時脈

/* PCA 暫存器*/ 
#define  ECF    0x01  //1=致能PCA計數溢位產生中斷
#define  CPS0   0x02  //選擇PCA的時脈來源
#define  CPS1   0x04  //00=Fosc/12(預定)，01=Fosc/2，10=Timer0溢位，11=ECI(P34)腳
#define  CIDL   0x80  //0=在IDLE時，PCA不計數(預定)，1=在IDLE時，PCA繼續計數

#define  ECCF   0x01  //1=致能有匹配或捕捉(CCFn=1)時產生中斷
#define  PWM    0x02  //1=致能由CEXn腳輸出PWM波形
#define  TOG    0x04  //1=設定PAC計數溢位及符合模組動作時，令CEXn腳反相輸出
#define  MAT    0x08  //1=設定PAC計數溢位及符合模組動作時，令CCON內的位元CCFn=1
#define  CAPN   0x10  //1=設定捕捉器在CEXn腳輸入負緣時，捕捉PCA計數
#define  CAPP   0x20  //1=設定捕捉器在CEXn腳輸入正緣時，捕捉PCA計數
#define  ECOM   0x40  //1=致能比較器功能，0=禁能比較器功能(預定)


#define  ECAPL   0x01  //PWM脈波(Duty Cycle)時間CAPnL延長的bit-9
#define  ECAPH   0x02  //PWM脈波(Duty Cycle)載入CAPnH延長的bit-9

/*  P1特殊功能暫存器 (90H) */

/* P3特殊功能暫存器 (B0H) */
//sbit S2CK0 = P3^5;
//sbit TOCK0 = P3^4;

//----- P4輸出入埠暫存器---------- (MPC82G516 Only)

#define PX0H  0x01  // INT0外部中斷高優先
#define PT0H  0x02  // TTimer0計時中斷高優先
#define PX1H  0x04  // INT1外部中斷高優先
#define PT1H  0x08  // Timer1計時中斷高優先
#define PSH   0x10  // UART串列傳輸中斷高優先
#define PT2H  0x20  // Timer2計時中斷高優先
#define PX2H  0x40  // INT2外部中斷高優先
#define PX3H  0x80  // INT3外部中斷高優先

/*******************************************
*實習板接腳宣告
*********************************************/

/*******************************************
*函數名稱: Delay_ms
*功能描述: 延時以1mS為單位，石英晶體為22.1184MHz
*********************************************/
/*
void Delay_ms(unsigned int dly) 
{  
  unsigned int dly1; //內循環變數
  while(dly--)         // 外循環，遞減至0
   {dly1=950;  while(dly1--);} //內循環，延時1ms
}
*/
//********宣告函數********************************** 
//extern void Delay_ms(unsigned int dly);//以mS為單位延時
//void Delay_10us(unsigned int dly);//以10uS為單位延時

//extern void UART_init(unsigned int BPS);  //UART啟始程式
//extern void OutputHex(unsigned char Uart_Data);	// 將十六進制碼轉換為字元，由UART發射出去
//extern void Output(unsigned char d);// 將字元，由UART發射出去

//void LCD_init(void);   	//LCD的啟始程式 
//void LCD_Data(unsigned char Data); 	//傳送資料到LCD
//void LCD_Cmd(unsigned char Cmd); 	//傳送命令到LCD
//void LCD_Cmd_Init(unsigned char Cmd); //啟始傳送命令到LCD
//void LCD_wait(void);   //LCD等待忙碌旗標BF
//void LCD_clear(void);  //LCD清除畫面

 //宣告EEPROM 93C46函數
extern void Clock(void);           //送入串列EEPROM時脈信號
extern void SEND(bit flag);       //送入EEPROM串列1-bit位址或資料
extern void SEND8(unsigned char Addr);      //送入8-bit位址或資料
//extern unsigned int  READ_16(unsigned char Addr); //讀取16-bit資料
extern unsigned char READ_8(unsigned char Addr);  //讀取8-bit資料
//extern void WRITE_16(unsigned char Addr,unsigned int ch);//寫入16-bit資料
extern void WRITE_8(unsigned char Addr,unsigned char ch);//寫入8-bit資料
extern void EWEN(void);            //EEPROM寫入及清除致能
extern void EWDS(void);            //EEPROM寫入及清除禁能
extern void ERAL(void);           //EEPROM清除全部記憶體