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

char string[20];
unsigned short val;
short theta; 
long g;
//short LUT[1001];
int LUT_i;
int index_m;

unsigned long x;


void msDelay(unsigned int);
short getLUTdata(int i);
char atan(int n, long v, long* x_p);
char getSign(int n);


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
const char LUT[]={50,51,51,51,52,52,52,53,53,53,54,54,55,55,55,56,56,57,57,57,58,58,59,59,59,60,60,61,61,62,62,63,63,64,64,65,65,66,66,67,67,68,69,69,70,70,71,72,73,73,74,75,76,77,78,79,80,81,83,85,90};

  int g_index=0;
char x_sign=0;    
    
// Setup and enable ADC channel 0
// Refer to Chapter 14 in S12G Reference Manual for ADC subsystem details
        
    ATDCTL1 = 0x4F;     // set for 12-bit resolution
    ATDCTL3 = 0x88;     // right justified, one sample per sequence
    ATDCTL4 = 0x02;     // prescaler = 2; ATD clock = 6.25MHz / (2 * (2 + 1)) == 1.04MHz
    ATDCTL5 = 0x25;     // continuous conversion on channel 0
    
// Configure Timer for PWM on channel 0 (uncomment below for milestone 2)
// PWMCTL= 0x??
// PWMCLK= 0x??
// PWMCLKAB= 0x??
// PWMPRCLK= 0x??
// PWME= 0x??

// Configure PWM preoperties (uncomment below for milestone 2)
// PWMPOL= 0x??
// PWMPER0= ?
// PWMCAE= 0x??
// PWMDTY0= ?


// Setup LED and SCI
  DDRJ |= 0x01;     // PortJ bit 0 is output to LED D2 on DIG13
  SCI_Init(9600);
  
  //LUT=getLUT();
 
  
  for(;;) {
    PTJ ^= 0x01;          // toggle LED
    
    
    val=ATDDR0;
    
    g_index=abs(val-(1050+260));
    x_sign=getSign(val-(1050+260));
    if(g_index<192){
    theta=atan(g_index,6,&x);  
    }else if(g_index<200){
    theta=atan(g_index,6,&x)+1;  
    }else if(g_index<261){
     theta=LUT[g_index-200]; 
    }else{
     theta=LUT[60]; 
    }
    
    
   
    
    
   
    
    
    
    
    //sprintf(&string, "%d",theta);
    //SCI_OutString(string);OutCRLF();
    SCI_OutUDec(val);OutCRLF();
    
    
    msDelay(250);    // 1 sec delay, you can validate using method outlined in Lecture W8-2_W8-3
  }
}

void msDelay(unsigned int time)
{
    unsigned int j,k;
    for(j=0;j<time;j++)
        for(k=0;k<1033;k++);
}

char atan(int n,long v,long* x_p){
 unsigned long cubic=0;
 unsigned long linear=0;
 unsigned long denom=0;
 unsigned long z=(long)(n+4);
 
 
 *x_p=((v*20*v*20)*(v*(z)/13));
 linear=*x_p;
 *x_p= (( (v*(z))*(v*(z))*(v*(z)) )/(6*13*13*13));
 cubic=*x_p;
 *x_p=(v*20*v*20*v*20);
 denom=*x_p;
 *x_p=(57*(linear +cubic))/denom;
 
 
 
 
 
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

char getSign(int n){
  if(n>=0){
  return 1;
 } else{
  return -1;
 }
}


