#define FILTERSETTLETIME 5000 //  Time (ms) to allow the filters to settle before sending data


const int CT1 = 1;                                                      //  divert sensor - Set to 0 to disable 
const int CT2 = 1;                                                      // Inverter sensor - Set to 0 to disable 
const int CT3 = 1;                                                      //grid sensor 
const int CT4 = 1;                                                      // windgen  sensor - Set to 0 to disable 

float grid = 0;                                                          //grid   usage
float stepa = 0;   // 
float stepb = 1;
float stepc = 1;
float prestep =1;
float step1 = 0;   // 
float step2 = 1;
float step3 = 1;
float prestep1 =1;
float curinvt = 1; //percentage of power uage comparison over or below grid usage 
float curelem =1;
float kw = 0;
int curgrid = 0;                                                      // current  PMW step
int curgrid2 = 0;                                                     //current triac step
float invert =100;
float wind = 100;
float diverter =100;
float element = 5000; //wattage of  element  use incase the incase no inverter reading ot grid tie outside  inverter reading - -

int pulse = 5;       // pin for pulse
int invstatus = 7;  // pin for led display  showing overproduction 
float per = 0;

int stat ;
int stepbu;
int ios = 8;

float stepa4 = 0;   // 
float stepb4 = 1;
float stepc4 = 1;
float prestep4 =0; //1
int stepbu4;
int stat4 ;
float curelem4 =1;
int curgrid4 = 0;
int ssr=0; // 0= zerocrossing 1 = phase angle 
int sV;
int full;
int type = 0; // 0= casdading -  1 = equal for diverting 

String value;
String EMON; 
int DELAY = 70;  /// delay for consistant decoding os rs485 sends  if to fast  some rs485 sends will be losts
#include <RS485.h>
#include <SoftwareSerial.h>

#include "EmonLib.h"
EnergyMonitor ct1,ct2,ct3,ct4;   // Create  instances for each CT channel

char Message[maxMsgLen+1] ;
char payload[50];


// Note: Please update emonhub configuration guide on OEM wide packet structure change:
// https://github.com/openenergymonitor/emonhub/blob/emon-pi/configuration.md
typedef struct { int power1, power2, power3, power4, Vrms;} PayloadTX;      // create structure - a neat way of packaging data for RF comms
PayloadTX emontx;                                                       

const int LEDpin = 9;                                                   // On-board emonTx LED 

boolean settled = false;

void setup() 
{
      
  Serial.begin(9600);

  RS485_Begin(28800);
   //while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
   
  //Serial.println("emonTX Shield CT123 Voltage example"); 
 // Serial.println("OpenEnergyMonitor.org");


  if (CT1) ct1.current(1, 66.606);                                     // Setup emonTX CT channel (ADC input, calibration)
  if (CT2) ct2.current(2, 60.606);                                     // Calibration factor = CT ratio / burden resistance
  if (CT3) ct3.current(3, 60.606);                                     // emonTx Shield Calibration factor = (100A / 0.05A) / 33 Ohms
  if (CT4) ct4.current(4, 60.606); 
  
  if (CT1) ct1.voltage(0, 146.54, 1.7);                                // ct.voltageTX(ADC input, calibration, phase_shift) - make sure to select correct calibration for AC-AC adapter  http://openenergymonitor.org/emon/modules/emontx/firmware/calibration. Default set for Ideal Power adapter                                         
  if (CT2) ct2.voltage(0, 146.54, 1.7);                                // 268.97 for the UK adapter, 260 for the Euro and 130 for the US.
  if (CT3) ct3.voltage(0, 146.54, 1.7);
  if (CT4) ct4.voltage(0, 146.54, 1.7);
  


  pinMode(LEDpin, OUTPUT);                                              // Setup indicator LED
  digitalWrite(LEDpin, HIGH);
  
                                                                                     
}

void loop() 
{ 
  if (CT1) {

    ct1.calcVI(20,2000);                                                  // Calculate all. No.of crossings, time-out 
    emontx.power1 = ct1.realPower;
    diverter =  emontx.power1;

EMON = String(diverter);
value = "cmd_4(" + EMON + ")" ;

  value.toCharArray (payload,(value.length() + 1));

  strcpy(Message,payload);

  if(RS485_SendMessage(Message,fWrite,ENABLE_PIN))
  {
    //Serial.print("Sending:");
    Serial.println(Message);
  }  
    
   // Serial.print("cmd_4("); Serial.print(diverter); Serial.println(")");                                      
  }
  
  emontx.Vrms = ct1.Vrms*100;                                            // AC Mains rms voltage 
  
  if (CT2) {
   
    ct2.calcVI(20,2000);                                                  // Calculate all. No.of crossings, time-out 
    emontx.power2 = ct2.realPower;
    invert = emontx.power2;
    
EMON = String(invert);
value = "cmd_1(" + EMON + ")" ;

  value.toCharArray (payload,(value.length() + 1));

  strcpy(Message,payload);

  if(RS485_SendMessage(Message,fWrite,ENABLE_PIN))
  {
    //Serial.print("Sending:");
    Serial.println(Message);
  }  


   
   // Serial.print("cmd_1("); Serial.print(invert); Serial.println(")");
    //Serial.print(" "); Serial.print(emontx.power2);
  } 

  if (CT3) {
    ct3.calcVI(20,2000);                                                  // Calculate all. No.of crossings, time-out 
    emontx.power3 = ct3.realPower;
    grid = emontx.power3; 

EMON = String(grid);
value = "cmd_2(" + EMON + ")" ;

  value.toCharArray (payload,(value.length() + 1));

  strcpy(Message,payload);

  if(RS485_SendMessage(Message,fWrite,ENABLE_PIN))
  {
    //Serial.print("Sending:");
    Serial.println(Message);
  }  
    
   //  Serial.print ("cmd_2("); Serial.print(grid); Serial.println(")");
    //Serial.print(" "); Serial.print(emontx.power3);
  } 
  
   if (CT4) {
     ct4.calcVI(20,2000);                                                  // Calculate all. No.of crossings, time-out 
    emontx.power4 = ct4.realPower;
    wind = emontx.power4; 

String EMON = String(wind);
value = "cmd_3(" + EMON + ")" ;

  value.toCharArray (payload,(value.length() + 1));

  strcpy(Message,payload);

  if(RS485_SendMessage(Message,fWrite,ENABLE_PIN))
  {
    //Serial.print("Sending:");
    Serial.println(Message);
  }  
   
  //Serial.print ("cmd_3("); Serial.print(wind); Serial.println(")");
    //Serial.print(" "); Serial.print(emontx.power1);
  } 
  if (invert <0){       // for capture ac adaptor errors is it display consant zero on inverter display -- ct or ac adaptor need to be reversed
    invert = 0;
  }
  if (wind <0){       // for capture ac adaptor errors is it display consant zero on inverter display -- ct or ac adaptor need to be reversed
    wind = 0;
  }
  //Serial.print(" "); Serial.print(ct1.Vrms);


if (grid != 0 ) {  
if (invert >=0) {

  step1 = ( grid / invert);

  prestep1 = (step2);

step2 = (prestep1 + step1);
  if (step2 > 1) {
    step2 =1;
  }
  if (step2 < 0) {
    step2 = 0;
  }
  curinvt = (0  + step2);
  
  curgrid2 = ( 254 * curinvt  );
  curgrid2 =(254-curgrid2);  //inverts the value of curgrid if need be 
  //  Serial.print(" curgrid2 ");
  //Serial.println(curgrid2);

  
}
}

if (CT1){
if (grid !=0) {
 
  
  //curgrid = 0;
  stepc = (grid / element); 

  prestep = (stepb);

stepb = (prestep + stepc);
  if (stepb > 0) {
    stepb =0;
  }
  if (stepb < (0-ios)) {
    stepb = (0-ios);
  }
  curelem = (0  + stepb);
stepbu=curelem;
curelem = (curelem - stepbu);
  curgrid = ( 254 * curelem  );
  curgrid =(0-curgrid);  //inverts the value of curgrid if need be 
 //   Serial.print(" curgrida ----------------");
//  Serial.println(curgrid);
}

if (grid !=0) {
 
  
  //curgrid = 0;
  stepc4 = (grid / element); 

  prestep4 = (stepb4);

stepb4 = (prestep4 + stepc4);
  if (stepb4 > 1) {
    stepb4 =1;
  }
  if (stepb4 < 0) {
    stepb4 = 0;
  }
  curelem4 = (0  + stepb4);


  curgrid4 = ( 255 * curelem4  );
  curgrid4 =(255-curgrid4);  //inverts the value of curgrid if need be 
 //   Serial.print(" curgrida ");
  //Serial.println(curgrid);
}

}

  int statc ;
int ivar;
int statb ;

stat = (0-stepbu);
if (curgrid==256){curgrid=0;}



//analogWrite(pulse,curgrid4); // single pulse signal for SSR off arduino board 

delay(DELAY);
 
if (stat > (ios-1)) {stat=(ios-1);curgrid=255;full=1;}
if (stat ==0) {ivar = 1;}
else  {ivar = 0;}
  if ( type == 0){
   // Serial.println(" solar Diversion - Cascading");
if (stat != statb) {
  statc=(stat+1);
  statb=stat;
  for(int i=ivar;i < stat; i++){
   
     ////////pwmController.setChannelPWM(i, 255 << 4);
    // Serial.print("test _____________________");
    //  Serial.print(i);
     //   Serial.println(" 255");

String DVP =String(i);

value = "cmd_D" + DVP + "(1024)" ;

  value.toCharArray (payload,(value.length() + 1));

  strcpy(Message,payload);

  if(RS485_SendMessage(Message,fWrite,ENABLE_PIN))
  {
    //Serial.print("Sending:");
 //   Serial.print("multi _____________________");
    Serial.println(Message);
delay(DELAY);

   
  }
  }
   
  for(int i=statc;i <ios; i++){
   ///////////  pwmController.setChannelPWM(i, 0 << 4);
   
   // Serial.print("test2 _____________________"); 
     //   Serial.print(i);
       // Serial.println(" O");
   String DVP =String(i);

value = "cmd_D" + DVP + "(0)" ;

  value.toCharArray (payload,(value.length() + 1));

  strcpy(Message,payload);

  if(RS485_SendMessage(Message,fWrite,ENABLE_PIN))
  {
    //Serial.print("Sending:");
  //  Serial.print("multi _____________________");
    Serial.println(Message);
delay(DELAY);
  }
     
     }

     }
      if (ssr==1){
      boolean st=false;                                  //an indicator to exit the while loop

  unsigned long start = millis();    //millis()-start makes sure it doesnt get stuck in the loop if there is an error.

  while(st==false)                                   //the while loop...
  {
    sV = analogRead(0);                    //using the voltage waveform
    if ((sV < (1024*0.55)) && (sV > (1024*0.45))) st=true;  //check its within range
    if ((millis()-start)>2000) st = true;
  }
  sV=map(curgrid,0,255,10000,0);  //delay before pulse  
     delayMicroseconds(sV); 
     
    }
     ////////////pwmController.setChannelPWM(stat, curgrid << 4);
 
    // Serial.print(stat); 
    // Serial.print(" ______"); 
    // Serial.println(curgrid);
String DVP =String(stat);
String DIVERT = String(curgrid <<2);
value = "cmd_D" + DVP + "(" + DIVERT + ")" ;

  value.toCharArray (payload,(value.length() + 1));

  strcpy(Message,payload);

  if(RS485_SendMessage(Message,fWrite,ENABLE_PIN))
  {
    //Serial.print("Sending:");
    //Serial.print("multi _____________________");
    Serial.println(Message);

  }

     
  }

  
  
  //  Serial.print(stat);
  //  Serial.print(" ");
   // Serial.println(curgrid);
    
  if (type == 1){
   // Serial.println(" solar Diversion -  In Unison");
    for(int i=0;i < ios; i++){
    ///////////// pwmController.setChannelPWM(i, curgrid << 4);
     
  //  Serial.print(i);
   // Serial.print(" ");
   // Serial.println(curgrid);
String DVP =String(i);
String DIVERT = String(curgrid <<2);
value = "cmd_D" + DVP + "(" + DIVERT + ")" ;

  value.toCharArray (payload,(value.length() + 1));

  strcpy(Message,payload);

  if(RS485_SendMessage(Message,fWrite,ENABLE_PIN))
  {
    //Serial.print("Sending:");
   // Serial.print("multi _____________________");
    Serial.println(Message);
delay(DELAY);


   
  } 


   
  } 
  }

analogWrite(pulse, curgrid4 );
//Serial.print( "test----"); Serial.println(1024 >>2);
analogWrite(invstatus, curgrid4);  // led display  showing overproduction 
delay(20);
EMON = String(curgrid4 <<2);
value = "cmd_5(" + EMON + ")" ;

  value.toCharArray (payload,(value.length() + 1));

  strcpy(Message,payload);

  if(RS485_SendMessage(Message,fWrite,ENABLE_PIN))
  {
    //Serial.print("Sending:");
    Serial.println(Message);
  }  
//delay(20);

//Serial.println ("");

  // because millis() returns to zero after 50 days ! 
  if (!settled && millis() > FILTERSETTLETIME) settled = true;

  if (settled)                                                            // send data only after filters have settled
  { 
        
 



   
 //   send_rf_data();                                                       // *SEND RF DATA* - see emontx_lib
 //   digitalWrite(LEDpin, HIGH); delay(2); digitalWrite(LEDpin, LOW);      // flash LED
   // delay(500);                                                          // delay between readings in ms
  }
 // DIVERTER;
}

