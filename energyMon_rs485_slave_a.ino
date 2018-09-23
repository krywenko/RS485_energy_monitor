// Receiver
#include <RS485esp.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
//#include <Arduino.h>
#include <TFT_ILI9341_ESP.h> // Hardware-specific library
#include <SPI.h>

#define TFT_GREY 0x5AEB
#define triac 16  // pin for pulsing triac
int analogF = 13;  // analog feq write on PWM
int pulse;
int start;
TFT_ILI9341_ESP tft = TFT_ILI9341_ESP();  

// Meter colour schemes
#define RED2RED 0
#define GREEN2GREEN 1
#define BLUE2BLUE 2
#define BLUE2RED 3
#define GREEN2RED 4
#define RED2GREEN 5
#define LTBLUE    0xB6DF
#define LTTEAL    0xBF5F
#define LTGREEN         0xBFF7
#define LTCYAN    0xC7FF
#define LTRED           0xFD34
#define LTMAGENTA       0xFD5F
#define LTYELLOW        0xFFF8
#define LTORANGE        0xFE73
#define LTPINK          0xFDDF
#define LTPURPLE  0xCCFF
#define LTGREY          0xE71C
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define DKBLUE        0x000D
#define DKTEAL    0x020C
#define DKGREEN       0x03E0
#define DKCYAN        0x03EF
#define DKRED         0x6000
#define DKMAGENTA       0x8008
#define DKYELLOW        0x8400
#define DKORANGE        0x8200
#define DKPINK          0x9009
#define DKPURPLE      0x4010
#define DKGREY        0x4A49
#define ORANGE        0xFD20
#define PINK          0xF81F
#define PURPLE    0x801F
//#define TFT_DC 2
//#define TFT_CS 15

#define TFT_GREY 0x5AEB
#define ILI9341_GREY 0x2104 // Dark grey 16 bit colour
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
//XPT2046 touch(/*cs=*/ 4, /*irq=*/ 5);
//Adafruit_GFX_Button button;

uint32_t runTime = -99999;       // time for next update
uint32_t runTime1 = -99999;

boolean graph_1 = true;
boolean graph_2 = true;
boolean graph_3 = true;
boolean graph_4 = true;
boolean graph_5 = true;
boolean graph_6 = true;
boolean graph_7 = true;
boolean redraw1 = true;
double ox , oy ;
double ox2 , oy2 ;
double x, y;
double a1, b1, c1, d1, r2, r1, vo, tempC, tempF, tempK;
byte p_len =4 ;

int reading = 0; // Value to be displayed - ring meter
int p_reading =0; // previous value -ring meter
int d = 0; // Variable used for the sinewave test waveform


String inString1 = "1"; 
String inString1b = ""; 
String inString2 = "1";
String inString2b = ""; 
String inString3 = "1"; 
String inString3b = ""; 
String inString4 = "1";
String inString4b = ""; 
String inString5 = "1"; 
String inString5b = ""; 


int scan =0;
int scan1;
int scan2;
int scan3;
int scan4;
int scan5;
int scan6 = 0;

char message[maxMsgLen+3+1];

////////////Graphing /////////////////////////
//////////////////////////////////////////////


String Format(double val, int dec, int dig ) {
  int addpad = 0;
  char sbuf[20];
  String condata = (dtostrf(val, dec, dig, sbuf));


  int slen = condata.length();
  for ( addpad = 1; addpad <= dec + dig - slen; addpad++) {
    condata = " " + condata;
  }
  return (condata);

}


// #########################################################################
// Return a 16 bit rainbow colour
// #########################################################################
unsigned int rainbow(byte value)
{
  // Value is expected to be in range 0-127
  // The value is converted to a spectrum colour from 0 = blue through to 127 = red

  byte red = 0; // Red is the top 5 bits of a 16 bit colour value
  byte green = 0;// Green is the middle 6 bits
  byte blue = 0; // Blue is the bottom 5 bits

  byte quadrant = value / 32;

  if (quadrant == 0) {
    blue = 31;
    green = 2 * (value % 32);
    red = 0;
  }
  if (quadrant == 1) {
    blue = 31 - (value % 32);
    green = 63;
    red = 0;
  }
  if (quadrant == 2) {
    blue = 0;
    green = 63;
    red = value % 32;
  }
  if (quadrant == 3) {
    blue = 0;
    green = 63 - 2 * (value % 32);
    red = 31;
  }
  return (red << 11) + (green << 5) + blue;
}

// #########################################################################
// Return a value in range -1 to +1 for a given phase angle in degrees
// #########################################################################
float sineWave(int phase) {
  return sin(phase * 0.0174532925);
}

// #########################################################################
//  Draw the meter on the screen, returns x coord of righthand side
// #########################################################################
int ringMeter(int value, int vmin, int vmax, int x, int y, int r, char *units, byte scheme,int sp, int segm, int wid, int p_value)
{
  // Minimum value of r is about 52 before value text intrudes on ring
  // drawing the text first is an option
  
  x += r; y += r;   // Calculate coords of centre of ring

  int w = r / wid;    // Width of outer ring is 1/4 of radius
  sp = sp/2;
  int angle = sp;  // Half the sweep angle of meter (300 degrees)

  int text_colour = 0; // To hold the text colour

  int v = map(value, vmin, vmax, -angle, angle); // Map the value to an angle v

  byte seg = 5; // Segments are 5 degrees wide = 60 segments for 300 degrees
  byte inc = 5*segm; // Draw segments every 5 degrees, increase to 10 for segmented ring

  // Draw colour blocks every inc degrees
  for (int i = -angle; i < angle; i += inc) {

    // Choose colour from scheme
    int colour = 0;
    switch (scheme) {
      case 0: colour = ILI9341_RED; break; // Fixed colour
      case 1: colour = ILI9341_GREEN; break; // Fixed colour
      case 2: colour = ILI9341_BLUE; break; // Fixed colour
      case 3: colour = rainbow(map(i, -angle, angle, 0, 127)); break; // Full spectrum blue to red
      case 4: colour = rainbow(map(i, -angle, angle, 63, 127)); break; // Green to red (high temperature etc)
      case 5: colour = rainbow(map(i, -angle, angle, 127, 63)); break; // Red to green (low battery etc)
      default: colour = ILI9341_BLUE; break; // Fixed colour
    }

    // Calculate pair of coordinates for segment start
    float sx = cos((i - 90) * 0.0174532925);
    float sy = sin((i - 90) * 0.0174532925);
    uint16_t x0 = sx * (r - w) + x;
    uint16_t y0 = sy * (r - w) + y;
    uint16_t x1 = sx * r + x;
    uint16_t y1 = sy * r + y;

    // Calculate pair of coordinates for segment end
    float sx2 = cos((i + seg - 90) * 0.0174532925);
    float sy2 = sin((i + seg - 90) * 0.0174532925);
    int x2 = sx2 * (r - w) + x;
    int y2 = sy2 * (r - w) + y;
    int x3 = sx2 * r + x;
    int y3 = sy2 * r + y;

    if (i < v) { // Fill in coloured segments with 2 triangles
      tft.fillTriangle(x0, y0, x1, y1, x2, y2, colour);
      tft.fillTriangle(x1, y1, x2, y2, x3, y3, colour);
      text_colour = colour; // Save the last colour drawn
    }
    else // Fill in blank segments
    {
      tft.fillTriangle(x0, y0, x1, y1, x2, y2, ILI9341_GREY);
      tft.fillTriangle(x1, y1, x2, y2, x3, y3, ILI9341_GREY);
    }
  }

  // Convert value to a string
  char buf[10];
  char p_buf[10];
  byte len = 4; if (value > 999) len = 5;

  dtostrf(value, len, 0, buf);
  dtostrf(p_value, p_len, 0, p_buf);
   //p_len = len;
  // Set the text colour to default
 // tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  // Uncomment next line to set the text colour to the last segment value!
 //  tft.setTextColor(text_colour, ILI9341_BLACK);
  
  // Print value, if the meter is large then use big font 6, othewise use 4
  if (r > 84){ 
              tft.setTextColor(ILI9341_BLACK, ILI9341_BLACK);
              tft.drawCentreString(p_buf, x - 5, y - 20, 6); 
              tft.setTextColor(text_colour, ILI9341_BLACK);    
              tft.drawCentreString(buf, x - 5, y - 20, 6); }// Value in middle
  else {
    tft.setTextColor(ILI9341_BLACK, ILI9341_BLACK);
    tft.drawCentreString(p_buf, x - 5, y - 20, 4);
    tft.setTextColor(text_colour, ILI9341_BLACK);  
    tft.drawCentreString(buf, x - 5, y - 20, 4);} // Value in middle




  // Print units, if the meter is large then use big font 4, othewise use 2
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  if (r > 84) tft.drawCentreString(units, x, y + 30, 4); // Units display
  else tft.drawCentreString(units, x, y + 5, 2); // Units display

  // Calculate and return right hand side x coordinate
  return x + r;
}



void DrawBarChartV(TFT_ILI9341_ESP & d, double x , double y , double w, double h , double loval , double hival , double inc , double curval ,  int dig , int dec, unsigned int barcolor, unsigned int voidcolor, unsigned int bordercolor, unsigned int textcolor, unsigned int backcolor, String label, boolean & redraw)
{

  double stepval, range;
  double my, level;
  double i, data;
  // draw the border, scale, and label once
  // avoid doing this on every update to minimize flicker
  if (redraw == true) {
    redraw = false;

    d.drawRect(x - 1, y - h - 1, w + 2, h + 2, bordercolor);
    d.setTextColor(textcolor, backcolor);
    //d.setTextSize(2);
    d.setCursor(x , y + 10);
    d.println(label);
    // step val basically scales the hival and low val to the height
    // deducting a small value to eliminate round off errors
    // this val may need to be adjusted
    stepval = ( inc) * (double (h) / (double (hival - loval))) - .001;
    for (i = 0; i <= h; i += stepval) {
      my =  y - h + i;
      d.drawFastHLine(x + w + 1, my,  5, textcolor);
      // draw lables
    //  d.setTextSize(1);
      d.setTextColor(textcolor, backcolor);
      d.setCursor(x + w + 12, my - 3 );
      data = hival - ( i * (inc / stepval));
      d.println(Format(data, dig, dec));
    }
  }
  // compute level of bar graph that is scaled to the  height and the hi and low vals
  // this is needed to accompdate for +/- range
  level = (h * (((curval - loval) / (hival - loval))));
  // draw the bar graph
  // write a upper and lower bar to minimize flicker cause by blanking out bar and redraw on update
  d.fillRect(x, y - h, w, h - level,  voidcolor);
  d.fillRect(x, y - level, w,  level, barcolor);
  // write the current value
  d.setTextColor(textcolor, backcolor);
  //d.setTextSize(2);
  d.setCursor(x , y - h - 23);
  d.println(Format(curval, dig, dec));

}

/*
  This method will draw a horizontal bar graph for single input
  it has a rather large arguement list and is as follows

  &d = display object name
  x = position of bar graph (upper left of bar)
  y = position of bar (upper left of bar (add some vale to leave room for label)
  w = width of bar graph (does not need to be the same as the max scale)
  h =  height of bar graph
  loval = lower value of the scale (can be negative)
  hival = upper value of the scale
  inc = scale division between loval and hival
  curval = date to graph (must be between loval and hival)
  dig = format control to set number of digits to display (not includeing the decimal)
  dec = format control to set number of decimals to display (not includeing the decimal)
  barcolor = color of bar graph
  voidcolor = color of bar graph background
  bordercolor = color of the border of the graph
  textcolor = color of the text
  back color = color of the bar graph's background
  label = bottom lable text for the graph
  redraw = flag to redraw display only on first pass (to reduce flickering)
*/

void DrawBarChartH(TFT_ILI9341_ESP & d, double x , double y , double w, double h , double loval , double hival , double inc , double curval ,  int dig , int dec, unsigned int barcolor, unsigned int voidcolor, unsigned int bordercolor, unsigned int textcolor, unsigned int backcolor, String label, boolean & redraw)
{
  double stepval, range;
  double mx, level;
  double i, data;

  // draw the border, scale, and label once
  // avoid doing this on every update to minimize flicker
  // draw the border and scale
  if (redraw == true) {
    redraw = false;
    d.drawRect(x , y , w, h, bordercolor);
    d.setTextColor(textcolor, backcolor);
   // d.setTextSize(2);
    d.setCursor(x , y - 20);
    d.println(label);
    // step val basically scales the hival and low val to the width
    stepval =  inc * (double (w) / (double (hival - loval))) - .00001;
    // draw the text
    for (i = 0; i <= w; i += stepval) {
      d.drawFastVLine(i + x , y + h + 1,  5, textcolor);
      // draw lables
     // d.setTextSize(1);
      d.setTextColor(textcolor, backcolor);
      d.setCursor(i + x , y + h + 10);
      // addling a small value to eliminate round off errors
      // this val may need to be adjusted
      data =  ( i * (inc / stepval)) + loval + 0.00001;
      d.println(Format(data, dig, dec));
    }
  }
  // compute level of bar graph that is scaled to the width and the hi and low vals
  // this is needed to accompdate for +/- range capability
  // draw the bar graph
  // write a upper and lower bar to minimize flicker cause by blanking out bar and redraw on update
  level = (w * (((curval - loval) / (hival - loval))));
  d.fillRect(x + level + 1, y + 1, w - level - 2, h - 2,  voidcolor);
  d.fillRect(x + 1, y + 1 , level - 1,  h - 2, barcolor);
  // write the current value
  d.setTextColor(textcolor, backcolor);
 // d.setTextSize(2);
  d.setCursor(x + w + 10 , y + 5);
  d.println(Format(curval, dig, dec));
}





void Graph(TFT_ILI9341_ESP &d, double x, double y, double gx, double gy, double w, double h, double xlo, double xhi, double xinc, double ylo, double yhi, double yinc, String title, String xlabel, String ylabel, unsigned int gcolor, unsigned int acolor, unsigned int pcolor, unsigned int tcolor, unsigned int bcolor, boolean &redraw) {
//if (redraw1==true){redraw=true;}
  double ydiv, xdiv;
  // initialize old x and old y in order to draw the first point of the graph
  // but save the transformed value
  // note my transform funcition is the same as the map function, except the map uses long and we need doubles
  //static double ox = (x - xlo) * ( w) / (xhi - xlo) + gx;
  //static double oy = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
  double i;
  double temp;
  int rot, newrot;

  if (redraw == true) {

    redraw = false;
    ox = (x - xlo) * ( w) / (xhi - xlo) + gx;
    oy = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
    // draw y scale
    for ( i = ylo; i <= yhi; i += yinc) {
      // compute the transform
      temp =  (i - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

      if (i == 0) {
        d.drawLine(gx, temp, gx + w, temp, acolor);
      }
      else {
        d.drawLine(gx, temp, gx + w, temp, gcolor);
      }

      d.setTextSize(1);
      d.setTextColor(tcolor, bcolor);
      d.setCursor(gx - 40, temp);
      // precision is default Arduino--this could really use some format control
      int strip =i;
      d.println(strip);
    }
    // draw x scale
    for (i = xlo; i <= xhi; i += xinc) {

      // compute the transform

      temp =  (i - xlo) * ( w) / (xhi - xlo) + gx;
      if (i == 0) {
        d.drawLine(temp, gy, temp, gy - h, acolor);
      }
      else {
        d.drawLine(temp, gy, temp, gy - h, gcolor);
      }

      d.setTextSize(1);
      d.setTextColor(tcolor, bcolor);
      d.setCursor(temp, gy + 10);
      // precision is default Arduino--this could really use some format control
     //int strip =(i/.66666666666666666666);
     int strip=(i/2)/3600;
      d.println(strip);
    }

    //now draw the labels
    d.setTextSize(2);
    d.setTextColor(tcolor, bcolor);
    d.setCursor(gx , gy - h - 30);
    d.println(title);

    d.setTextSize(1);
    d.setTextColor(acolor, bcolor);
    d.setCursor(gx , gy + 20);
    d.println(xlabel);

    d.setTextSize(1);
    d.setTextColor(acolor, bcolor);
    d.setCursor(gx - 30, gy - h - 10);
    d.println(ylabel);


  }

  //graph drawn now plot the data
  // the entire plotting code are these few lines...
  // recall that ox and oy are initialized as static above
  x =  (x - xlo) * ( w) / (xhi - xlo) + gx;
  y =  (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
  d.drawLine(ox, oy, x, y, pcolor);
  d.drawLine(ox, oy + 1, x, y + 1, pcolor);
  d.drawLine(ox, oy - 1, x, y - 1, pcolor);
  ox = x;
  oy = y;

}
void Graph2(TFT_ILI9341_ESP &d, double x1, double y, double gx, double gy, double w, double h, double xlo, double xhi, double xinc, double ylo, double yhi, double yinc, String title, String xlabel, String ylabel, unsigned int gcolor, unsigned int acolor, unsigned int pcolor, unsigned int tcolor, unsigned int bcolor, boolean &redraw) {
//if (redraw1==true){redraw=true;}
  double ydiv, xdiv;
  // initialize old x and old y in order to draw the first point of the graph
  // but save the transformed value
  // note my transform funcition is the same as the map function, except the map uses long and we need doubles
  //static double ox = (x - xlo) * ( w) / (xhi - xlo) + gx;
  //static double oy = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
  double i;
  double temp;
  int rot, newrot;

  if (redraw == true) {

    redraw = false;
    ox2 = (x - xlo) * ( w) / (xhi - xlo) + gx;
    oy2 = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
    // draw y scale
    for ( i = ylo; i <= yhi; i += yinc) {
      // compute the transform
      temp =  (i - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

      if (i == 0) {
        d.drawLine(gx, temp, gx + w, temp, acolor);
      }
      else {
        d.drawLine(gx, temp, gx + w, temp, gcolor);
      }

      d.setTextSize(1);
      d.setTextColor(tcolor, bcolor);
      d.setCursor(gx - 40, temp);
      // precision is default Arduino--this could really use some format control
      int strip =i;
      d.println(strip);
    }
    // draw x scale
    for (i = xlo; i <= xhi; i += xinc) {

      // compute the transform

      temp =  (i - xlo) * ( w) / (xhi - xlo) + gx;
      if (i == 0) {
        d.drawLine(temp, gy, temp, gy - h, acolor);
      }
      else {
        d.drawLine(temp, gy, temp, gy - h, gcolor);
      }

      d.setTextSize(1);
      d.setTextColor(tcolor, bcolor);
      d.setCursor(temp, gy + 10);
      // precision is default Arduino--this could really use some format control
     //int strip =(i/.66666666666666666666);
     int strip=(i/2)/3600;
      d.println(strip);
    }

    //now draw the labels
    d.setTextSize(2);
    d.setTextColor(tcolor, bcolor);
    d.setCursor(gx , gy - h - 30);
    d.println(title);

    d.setTextSize(1);
    d.setTextColor(acolor, bcolor);
    d.setCursor(gx , gy + 20);
    d.println(xlabel);

    d.setTextSize(1);
    d.setTextColor(acolor, bcolor);
    d.setCursor(gx - 30, gy - h - 10);
    d.println(ylabel);


  }

  //graph drawn now plot the data
  // the entire plotting code are these few lines...
  // recall that ox and oy are initialized as static above
  x =  (x - xlo) * ( w) / (xhi - xlo) + gx;
  y =  (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
  d.drawLine(ox2, oy2, x, y, pcolor);
  d.drawLine(ox2, oy2 + 1, x, y + 1, pcolor);
  d.drawLine(ox2, oy2 - 1, x, y - 1, pcolor);
  ox2 = x;
  oy2= y;

}

//////////////////////////////////////////////////
///////////////End of graphing ////////////////////

void setup()
{
WiFi.softAPdisconnect(true);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  tft.setTextSize(3);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK); 
  tft.println("DIVERTER MODULE");
  //tft.println("RS485 Offline");
  tft.println("Please wait!");
analogWrite(triac, 0);
  analogWriteFreq(analogF);
  Serial.begin(9600);
  Serial.println("System Startup - Receiver");

  RS485_Begin(28800);
start =0;
// tft.setCursor(0, 0);
  delay(1000);
}

void loop()
{

   if (millis() - runTime1 >=(500)){
  runTime1=millis();
  scan++;
   }
  scan6++;
  
 // Serial.println(scan6);
 // Serial.println(pulse);
  //if(RS485_ReadPlainMessage(fAvailable,fRead,message))
  if(RS485_ReadMessage(fAvailable,fRead, message))
  {
    if (start ==0) tft.fillScreen(TFT_BLACK);
    start =1;
   // Serial.print("Receiving:");
    Serial.println(message);
    String payload(message); 
  
     parseCommand(payload);

  }

if (scan6 >1000000){ 
    pulse =0 ;
    tft.setCursor(0, 0, 1);
    tft.setTextSize(2);
    tft.fillScreen(TFT_BLACK);
    tft.println("DIVERTER MODULE");
  tft.println("RS485 Offline");
  tft.println("Check Connection");
  tft.println("");
  tft.println("");
  tft.println("Diverter Disabled");
    analogWrite(triac, pulse);
    scan6 = 0;
    start = 0;
     graph_1 = true;
 graph_2 = true;
 graph_3 = true;
 graph_4 = true;
 graph_5 = true;
 graph_6 = true;
 graph_7 = true;
  }
  
}



void parseCommand(String com)
{

  
 // scan++; 
//char test1[5];
  String part1;
  String part2;
  
  part1 = com.substring(4,com.indexOf("("));
  
  //Serial.println(part1);
  
  part2 = com.substring(com.indexOf("(")+1);
 // Serial.println(part2);
  if (part1 == "1")
  {
    
  //  scan1++;
    Serial.println("recieved 1");
    String raw_CMD;
    inString1=part2;
   
   
    
    }
  if (part1== "2")
    {
     //scan2++; 
    Serial.println("recieved 2");
    String raw_CMD;
    inString2=part2;
   
   
    }
 if (part1 == "3")
    {
     // scan3++; 
    Serial.println("recieved 3");
    String raw_CMD;
    inString3=part2;
    
      
    
  }


  if (part1== "4")
    {
    // scan4++; 
    Serial.println("recieved 4");
    String raw_CMD;
    inString4=part2;
   
  
    }
 if (part1 == "5")
    {
      //scan5++; 
    Serial.println("recieved 5");
    String raw_CMD;
    inString5=part2;
    pulse = (part2.toInt());
   analogWrite(triac, pulse << 2); // primary write to 1st triac
    scan6 = 0;
  }


x=scan; //stepping function for graphs X axis 

///////////Graphing////////////
   y = (inString2.toFloat()/1000);
    //Serial.println(tempC);
   // y = tempF(tempC * 1.8000) + 32.00;
  //  y = tempC; /// Y axis
 double Tinc = 3600;
 double Tinc2 = 3600;
    Tinc = (Tinc*2);
    Tinc2 = (Tinc2*2);
    //double Time= 28800;
    double Time= 14400;  /// seconds to display  hour format
    
     Time =(Time *2) ;//timedelay compensation 
    Graph(tft, x, y, 50, 75, 260, 60, 0, Time, Tinc, 0, 4, 1, "kw", "hrs", "Solar/Grid", DKBLUE, RED, GREEN, WHITE, BLACK, graph_3);
   y= inString1.toFloat()/1000;
   Graph2(tft, x, y, 50, 75, 260, 60, 0, Time, Tinc2, 0, 4, 1, "kw", "hrs", "", DKBLUE, RED, YELLOW, WHITE, BLACK, graph_4);
   
   
   if (millis() - runTime >=(Time*600)){  ///clear  graphs and redraw from beginning
  runTime=millis();
 //delay(1000);
 tft.fillScreen(BLACK);
  scan=0; ///resets X axis
 /*  a1 = 3.354016E-03 ;
  b1 = 2.569850E-04 ;
  c1 = 2.620131E-06 ;
  d1 = 6.383091E-08 ;*/
  graph_3=true;  /// add graphs affected by resetting screen
  graph_1=true;  // radial meters are un affected 
  graph_2=true;
  graph_4=true;
}
    
//////////Vertical bar/////////////// just repeat line for multiple
reading = (inString3.toInt());
if (reading <1) reading=1;
 DrawBarChartV(tft, 10,  220, 10, 90, -0, 2500 , 500, reading , 4 , 0, DKORANGE, BLACK, BLUE, WHITE, BLACK, "DIVERTER", graph_2); //xpos,ypos, width,height,L_range,H_range,step,reading, font, decimial 
//reading = (inString2.toInt()*2);
// DrawBarChartV(tft, 140,  220, 10, 90, -40, 40 , 20, reading , 4 , 0, DKORANGE, BLACK, BLUE, WHITE, BLACK, "TEMP", graph_2); //xpos,ypos, width,height,L_range,H_range,step,reading, font, decimial 

////////////horizontl bar///////////////
reading = (inString4.toInt());
if (reading <1) reading =1; //if bar has negative value if not set to write neagtive it causes crash
  DrawBarChartH(tft, 100, 200, 180, 15, 0, 2800, 700, reading, 2, 0, GREEN, BLACK,  GREEN, WHITE, BLACK, "Wind", graph_1);//xpos,ypos, width,height,L_range,H_range,step,reading, font, decimial 


///////////////Radial Meters//////////////////    
  

    // Set the the position, gap between meters, and inner radius of the meters
    int xpos = 210, ypos = 5, gap = 4, radius = 40, sweep = 240,segm=2, wid=4;

    // Draw meter and get back x position of next meter

    // Test with Sine wave function, normally reading will be from a Mqtt subscriptions
   // reading = ((inString1.toInt()));
  //  p_reading = (inString1b.toInt());
   // xpos = gap + ringMeter(reading, 0, 2000, xpos, ypos, radius, "HP", GREEN2GREEN,sweep,segm,wid,p_reading); // Draw analogue meter
  //  inString1b=inString1;

   // reading = (inString2.toInt());
   // p_reading = (inString2b.toInt());
  //  xpos = gap + ringMeter(reading, -40, 50, xpos, ypos, radius, "degC", BLUE2RED,sweep,segm,wid,p_reading); // Draw analogue meter
   // inString2=inString2;

   // reading = (inString3.toInt());
    //p_reading = (inString3b.toInt());
    //reading = 50;
   //xpos = gap + ringMeter(reading, 0, 100, xpos, ypos, radius, "%RH", BLUE2BLUE, sweep,segm,wid,reading); // Draw analogue meter
   // inString3b=inString3;

    // Draw two more larger meters
    xpos = 60, ypos = 95, gap = 10, radius = 60, sweep = 240,segm =2, wid =3;

    reading = (inString1.toInt());
    p_reading=(inString1b.toInt());
    p_len = 4; if (p_reading > 999) p_len = 5;
    xpos = gap + ringMeter(reading, -30, 3000, xpos, ypos, radius, "SOLAR", RED2GREEN, sweep,segm,wid,p_reading); // Draw analogue meter
    inString1b=inString1;
    

    reading = (inString2.toInt());
    p_reading = (inString2b.toInt());
    p_len = 4; if (p_reading > 999) p_len = 5;
    xpos = gap + ringMeter(reading, -300, 15000, xpos, ypos, radius, "GRID", GREEN2RED,sweep,segm,wid,p_reading); // Draw analogue meter
    inString2b=inString2;

    // Draw a large meter
   // xpos = 40, ypos = 5, gap = 15, radius = 120, sweep = 75, segm=2,wid=10;
  //  reading = 175;
  //  p_reading = (input5p.toInt());
    // Comment out above meters, then uncomment the next line to show large meter
    //ringMeter(reading,0,200, xpos,ypos,radius," Watts",GREEN2RED,sweep,segm,wid,p_value); // Draw analogue meter
    //inString5b=inString5;
  
    }  

