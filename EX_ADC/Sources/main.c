// filename ******** Main.C ************** 

//***********************************************************************
// Simple ADC example for the Technological Arts EsduinoXtreme board
// by Carl Barnes, 12/03/2014
//***********************************************************************

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"  /* derivative information */
#include "SCI.h"
#include "stdlib.h"
#include "stdio.h"
//#include "math.h"
//#include "LUT.h"

char string[4];
unsigned short val;
short theta; 
long g;
//short LUT[1001];
int LUT_i;
int index_m;
unsigned short val1;
unsigned short val2;
unsigned short val3;
unsigned short val4;
short sign_theta=0;
unsigned long x;

int max=0;
int min=0;
short z_dir=0;

short x_sign=0;

int loop_cnt=0;

short press_cnt=0;
short dir_en=0;
short serial_en=1;

short temp_atdctl;


void QuartermsDelay(unsigned long time);
short getLUTdata(int i);
char atan(int n, long* x_p);
short getSign(int n);
void busCLK(void);
void setLEDx(int n, int v);
void displayBinary(short s,unsigned short n);
void displayBCD(short s,unsigned short n);


//---------------------OutCRLF---------------------
// Output a CR,LF to SCI to move cursor to a new line
// Input: none
// Output: none
// Toggle LED each time through the loop

void OutCRLF(void){
  SCI_OutChar(CR);
  SCI_OutChar(LF);
  PTJ ^= 0x20;          // toggle LED D2
}

void main(void) {
const char LUT[]={39,39,39,39,39,40,40,40,40,40,41,41,41,41,41,41,42,42,42,42,42,43,43,43,43,43,44,44,44,44,44,45,45,45,45,45,46,46,46,46,46,47,47,47,47,48,48,48,48,48,49,49,49,49,49,50,50,50,50,51,51,51,51,51,52,52,52,52,53,53,53,53,54,54,54,54,55,55,55,55,56,56,56,56,57,57,57,57,58,58,58,58,59,59,59,60,60,60,60,61,61,61,62,62,62,63,63,63,64,64,64,64,65,65,66,66,66,67,67,67,68,68,68,69,69,70,70,70,71,71,72,72,73,73,74,74,75,75,76,77,77,78,79,79,80,81,82,83,84,86,90};
  
  
  int g_index=0;
  
   press_cnt=0;
   dir_en=0;
   serial_en=0;
   loop_cnt=0;
   z_dir=0;
busCLK(); 

DDRP=0xFF;
DDRT=0xFC;
ATDDIEN=0xFFC0;
DDR1AD=0xC0; 
DDR0AD=0xFF;  
    
// Setup and enable ADC channel 0
// Refer to Chapter 14 in S12G Reference Manual for ADC subsystem details
        
    ATDCTL1 = 0x4F;     // set for 12-bit resolution
    ATDCTL3 = 0x88;     // right justified, one sample per sequence
    ATDCTL4 = 0x00;     // prescaler = 2; ATD clock = 8MHz / (2 * (0 + 1)) == 4MHz
    ATDCTL5 = 0x25;     // continuous conversion on channel 0
    

// Setup LED and SCI
  DDRJ |= 0x01;     // PortJ bit 0 is output to LED D2 on DIG13
  
  
  //LUT=getLUT();
  
  
  //enable timer interrupts
  
  TSCR1 = 0x90;    //Timer System Control Register 1
                    // TSCR1[7] = TEN:  Timer Enable (0-disable, 1-enable)
                    // TSCR1[6] = TSWAI:  Timer runs during WAI (0-enable, 1-disable)
                    // TSCR1[5] = TSFRZ:  Timer runs during WAI (0-enable, 1-disable)
                    // TSCR1[4] = TFFCA:  Timer Fast Flag Clear All (0-normal 1-read/write clears interrupt flags)
                    // TSCR1[3] = PRT:  Precision Timer (0-legacy, 1-precision)
                    // TSCR1[2:0] not used

  TSCR2 = 0x00;    //Timer System Control Register 2
                    // TSCR2[7] = TOI: Timer Overflow Interrupt Enable (0-inhibited, 1-hardware irq when TOF=1)
                    // TSCR2[6:3] not used
                    // TSCR2[2:0] = Timer Prescaler Select: See Table22-12 of MC9S12G Family Reference Manual r1.25 (set for bus/1)
  
                    
  TIOS = 0xEE;     //Timer Input Capture or Output capture
                    //set TIC[0] and input (similar to DDR)
  PERT = 0x11;     //Enable Pull-Up resistor on TIC[0]

  TCTL3 = 0x02;    //TCTL3 & TCTL4 configure which edge(s) to capture
  TCTL4 = 0x02;    //Configured for falling edge on TIC[0]

/*
 * The next one assignment statement configures the Timer Interrupt Enable                                                   
 */           
   
  TIE = 0x11;      //Timer Interrupt Enable
  EnableInterrupts;
  PTP=0x02;
  for(;;) {
    //PTJ ^= 0x01;          // toggle LED
    
    if(serial_en==1){
    if(loop_cnt==0){
     SCI_Init(38400); 
     loop_cnt=1; 
    }
    temp_atdctl=ATDCTL5;
    
    val1=ATDDR0;
    
    QuartermsDelay(1);
    val2=ATDDR0;
    
    QuartermsDelay(1);
    val3=ATDDR0;
    
    QuartermsDelay(1);
    val4=ATDDR0;
    ATDCTL5=0x23;
    QuartermsDelay(1);
    
    val=(val1+val2+val3+val4)/4;
    
    z_dir=ATDDR0;
    if(z_dir>2100){
      
      z_dir=0;
    }else{
      z_dir=1;
    }
    
    
    
    ATDCTL5=temp_atdctl;
    
    if(dir_en==0){
      
    
    g_index=abs(val-(1675+400));
    x_sign=getSign(val-(1675+400));
    if(g_index<250){
    theta=atan(g_index,&x);  
    }else if(g_index<400){
     theta=LUT[g_index-250]; 
    }else{
     theta=LUT[150]; 
    }
    }else{
     g_index=abs(val-(1620+400));
    x_sign=getSign(val-(1620+400));
    if(g_index<250){
    theta=atan(g_index,&x);  
    }else if(g_index<400){
     theta=LUT[g_index-250]; 
    }else{
     theta=LUT[150]; 
    } 
      
    }
    
    if(z_dir==1){
     theta=theta+2*(90-theta); 
      
    }
    
    
    //PT1AD=0xC0;
    //PT0AD=0xFF;
    //PTT=0xFF;
    ////press_cnt=0;
    //while(1){
      ////press_cnt=press_cnt+1;
      //if(press_cnt>255)press_cnt=0;
      //displayBinary(0,press_cnt);
     // QuartermsDelay(750);
      
   // }
    
    
    
    displayBCD(x_sign,theta);
    sign_theta=theta*x_sign;
    sprintf(&string, "%d",sign_theta);
    SCI_OutString(string); OutCRLF();
    //SCI_OutUDec(val);OutCRLF();
    }
    
   }
        // 1 sec delay, you can validate using method outlined in Lecture W8-2_W8-3
  
}

interrupt  VectorNumber_Vtimch4 void ISR_Vtimch4(void){
unsigned short temp;
PTJ^=0x01;

  dir_en =(dir_en+1)%2;
  if (dir_en==0)
  ATDCTL5 = 0x25;
  if (dir_en==1)
  ATDCTL5 = 0x24;
  QuartermsDelay(2000);
  temp_atdctl=ATDCTL5;
  
  temp = TC4;       //Refer back to TFFCA, we enabled FastFlagClear, thus by reading the Timer Capture input we automatically clear the flag, allowing another TIC interrupt
}
  interrupt  VectorNumber_Vtimch0 void ISR_Vtimch0(void){
unsigned short temp1;
unsigned short temp2=PTJ;
  serial_en=1-serial_en;
  
  if(serial_en==1){
   PTJ=0x00;
   QuartermsDelay(1000);
   PTJ=0x01;
   QuartermsDelay(1000); 
   PTJ=0x00;
   QuartermsDelay(1000);
   PTJ=0x01;
   QuartermsDelay(1000); 
   PTJ=0x00;  
  }
  
  if(serial_en<0 || serial_en>1)serial_en=0;
  
  PTJ=temp2;
  QuartermsDelay(1000);
  
  temp1 = TC0;       //Refer back to TFFCA, we enabled FastFlagClear, thus by reading the Timer Capture input we automatically clear the flag, allowing another TIC interrupt
}

void QuartermsDelay(unsigned long time)
{
    long i;
    TSCR1=0x90;
    TSCR2=0x00;
    TIOS=0x02;
    TC0=TCNT+2000;
    for(i=0;i<time;i++){
     while(!TFLG1_C1F);
     TC1 +=2000; 
      
    }
}

char atan(int n,unsigned long* x_p){
 unsigned long cubic=0;
 unsigned long linear=0;
 
 unsigned long z=(unsigned long)(n+4);
 if(n<=4){
  return 0;
 }
 
 
 *x_p=(z*573)/4000;
 linear=*x_p;
 *x_p= (( 57*z*z*z)/(384000000));
 cubic=*x_p;
 
 *x_p=(linear +cubic);
 
 
 
 
 
 //*x_p=( ((v*20*v*20)*(v*(*x_p)/13)) + (( (v*(*x_p))*(v*(*x_p))*(v*(*x_p)) )/(6*13*13*13)) )/(v*20*v*20*v*20);
 //*x_p=1800000*(*x_p)/31415;
 
 return (char)*x_p;                
}

int abs(int n){
 if(n>=0){
  return n;
 } else{
  return -n;
 }
}

short getSign(int n){
  if(n>=0){
  return 1;
 }else{
  return -1;
 }
}

void busCLK(void){
 
 CPMUCLKS=0x80;
 CPMUOSC=0x00;
 CPMUPROT=0x26;
 CPMUSYNR=0x1F;
 CPMUFLG=0x00; 
  
}

void setLEDx(int n, int v){
 if(n==0){
  if(v==1){
   PT1AD=PT1AD|0b01000000;   
  }
  if(v==0){
   PT1AD=PT1AD&0b10111111;  
  }
 }
 if(n==1){
  if(v==1){
   PT1AD=PT1AD|0b10000000;   
  }
  if(v==0){
   PT1AD=PT1AD&0b01111111;  
  }
 }
 if(n==2){
  if(v==1){
   PTT=PTT|0b00000100;   
  }
  if(v==0){
   PTT=PTT&0b11111011;  
  }
 }
 if(n==3){
  if(v==1){
   PT0AD=PT0AD|0b00000001;   
  }
  if(v==0){
   PT0AD=PT0AD&0b11111110;  
  }
 }
 if(n==4){
  if(v==1){
   PT0AD=PT0AD|0b00000010;   
  }
  if(v==0){
   PT0AD=PT0AD&0b11111101;  
  }
 }
 if(n==5){
  if(v==1){
   PT0AD=PT0AD|0b00000100;   
  }
  if(v==0){
   PT0AD=PT0AD&0b11111011;  
  }
 }
 if(n==6){
  if(v==1){
   PT0AD=PT0AD|0b00001000;   
  }
  if(v==0){
   PT0AD=PT0AD&0b11110111;  
  }
 }
 if(n==7){
  if(v==1){
   PTT=PTT|0b00001000;   
  }
  if(v==0){
   PTT=PTT&0b11110111;  
  }
 }
 if(n==8){
  if(v==1){
   PTP=PTP|0b00000010;   
  }
  if(v==0){
   PTP=PTP&0b11111101;  
  }
 }
  
  
}

void displayBinary(short s,unsigned short n){
  setLEDx(8,n&0b00000001);
  setLEDx(7,n>>1&0b0000001);
  setLEDx(6,n>>2&0b000001);
  setLEDx(5,n>>3&0b00001);
  setLEDx(4,n>>4&0b0001);
  setLEDx(3,n>>5&0b001);
  setLEDx(2,n>>6&0b01);
  setLEDx(1,n>>7&0b1);
  
  if(s==-1){
    setLEDx(0,1);
  }else{
  setLEDx(0,0);
  }
    
 
  
}
void displayBCD(short s,unsigned short n){
  short dig1=n%10;
  short dig2=n/10;
  setLEDx(8,dig1&0b00000001);
  setLEDx(7,dig1>>1&0b0000001);
  setLEDx(6,dig1>>2&0b000001);
  setLEDx(5,dig1>>3&0b00001);
  setLEDx(4,dig2&0b00000001);
  setLEDx(3,dig2>>1&0b0000001);
  setLEDx(2,dig2>>2&0b000001);
  setLEDx(1,dig2>>3&0b00001);
  
  
  if(s==-1){
    setLEDx(0,1);
  }else{
  setLEDx(0,0);
  }
    
 
  
}


