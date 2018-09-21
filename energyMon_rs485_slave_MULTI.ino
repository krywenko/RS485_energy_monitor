// Receiver
#include <RS485esp.h>
#include <SoftwareSerial.h>
#define triac0 16  // pin for pulsing triac
#define triac1 02  // pin for pulsing triac
#define triac2 14  // pin for pulsing triac
#define triac3 12  // pin for pulsing triac
#define triac4 13  // pin for pulsing triac
#define triac5 15  // pin for pulsing triac
#define triac6 03  // pin for pulsing triac
#define triac7 01  // pin for pulsing triac



int analogF = 13;  // analog feq write on PWM
int pulse;
 int scan;

char message[maxMsgLen+3+1];

void setup()
{
  Serial.begin(9600);  /// disable serial prints if using  more then 6 ssr
  Serial.println("System Startup - Receiver");

  RS485_Begin(28800);

analogWrite(triac0, 0);
analogWrite(triac1, 0);
analogWrite(triac2, 0);
analogWrite(triac3, 0);
analogWrite(triac4, 0);
analogWrite(triac5, 0);
//analogWrite(triac6, 0);  .. enable if using more then 6 ssr
//analogWrite(triac7, 0);
  analogWriteFreq(analogF);
  
}

void loop()
{

 
  scan++;

if (scan >1000000){ 
analogWrite(triac0, 0);
analogWrite(triac1, 0);
analogWrite(triac2, 0);
analogWrite(triac3, 0);
analogWrite(triac4, 0);
analogWrite(triac5, 0);
//analogWrite(triac6, 0);  .. enable if using more then 6 ssr
//analogWrite(triac7, 0);
    
  }
  
  //if(RS485_ReadPlainMessage(fAvailable,fRead,message))
  if(RS485_ReadMessage(fAvailable,fRead, message))
  {
   // Serial.print("Receiving:");
   // Serial.println(message);
    String payload(message); 
  
     parseCommand(payload);

  }
}



void parseCommand(String com)
{
   
//char test1[5];
  String part1;
  String part2;
  
  part1 = com.substring(4,com.indexOf("("));
  
  //Serial.println(part1);
  
  part2 = com.substring(com.indexOf("(")+1);
 // Serial.println(part2);

 if (part1 == "D0")
  {
    pulse = (part2.toInt());
   analogWrite(triac0, pulse ); // primary write to 1st triac
   Serial.print("d0--"); Serial.println(pulse);

   scan = 0;
    }
    
    

 
  if (part1 == "D1")
  {
    
     pulse = (part2.toInt());
   analogWrite(triac1, pulse ); // primary write to 1st triac
    
    Serial.print("d1--"); Serial.println(pulse);
    scan = 0;
    }
  if (part1== "D2")
    {
    pulse = (part2.toInt());
   analogWrite(triac2, pulse ); // primary write to 1st triac
Serial.print("d2--"); Serial.println(pulse);
scan = 0;
    }
    
 if (part1 == "D3")
    {
    pulse = (part2.toInt());
   analogWrite(triac3, pulse ); // primary write to 1st triac
    Serial.print("d3--"); Serial.println(pulse);
    scan = 0;
    }  
    
  


  if (part1== "D4")
    {
      pulse = (part2.toInt());
   analogWrite(triac4, pulse ); // primary write to 1st triac
Serial.print("d4--"); Serial.println(pulse);
    scan = 0;
    }
 if (part1 == "D5")
    {
     pulse = (part2.toInt());
     Serial.print("d5--"); Serial.println(pulse);
   analogWrite(triac5, pulse ); // primary write to 1st triac
    
    scan = 0;  
  }
///////////////////   enable if using more then 6 ssr /////////////////////
///////////////////////////////////////////////////////////////////////////
  /*
if (part1 == "D6")
    {
    pulse = (part2.toInt());
   analogWrite(triac6, pulse ); // primary write to 1st triac
    
     scan = 0; 
    
  }

  if (part1 == "D7")
    {
  
    pulse = (part2.toInt());
  analogWrite(triac7, pulse ); // primary write to 1st triac
    //scan6 = 0;
      
    scan = 0;
  }
*/
  
    }  

