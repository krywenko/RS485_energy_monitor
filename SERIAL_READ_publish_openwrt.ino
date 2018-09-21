

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//int blue = 1;
//int red = 2;
//int grn = 4;
// Update these with values suitable for your network.
String command;
float input1b = 0;
float input2b = 0;
float input3b = 0;
float input4b = 0;
int scan1;
int scan2;
int scan3;
int scan4;

const char* ssid = "IOT";
const char* password = "";
const char* mqtt_server = "192.168.168.150";
String clientName;
WiFiClient espClient;
PubSubClient client(mqtt_server,1883, espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
char msg2[50];
char msg3[50];

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  
}
}

String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}


  void MQTT_Connect(){
    if (client.connect((char*) clientName.c_str())) {
    Serial.println("Connected to MQTT broker");

   digitalWrite(BUILTIN_LED, LOW); 

  }
  else {
    Serial.println("MQTT connect failed");
    Serial.println("Will reset and try again...");

    abort();
  }
  client.setCallback(callback);
  }

void setup_wifi() {


  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
WiFi.hostname("PowerMonitor");
  WiFi.begin(ssid, password);
WiFi.mode(WIFI_STA);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  clientName += "esp8266-";
  uint8_t mac[6];
  WiFi.macAddress(mac);
  clientName += macToStr(mac);
  clientName += "-";
  clientName += String(micros() & 0xff, 16);

  Serial.print("Connecting to ");
  Serial.print(mqtt_server);
  Serial.print(" as ");
  Serial.println(clientName);
  
  MQTT_Connect();
  
}

void setup() {
 Serial.begin(9600);
pinMode(BUILTIN_LED, OUTPUT); 
// pinMode(red, OUTPUT);
//pinMode(blue, OUTPUT);

 delay(10);
 digitalWrite(BUILTIN_LED, HIGH); 
   setup_wifi();

 
 
 // put your setup code here, to run once:
Serial.println("started");
}





void parseCommand(String com)
{
   
char test[5];
  String part1;
  String part2;
  
  part1 = com.substring(6, com.indexOf("("));
  
  Serial.println(part1);
  
  part2 = com.substring(com.indexOf("(")+1);
  Serial.println(part2);
  if (part1 == "1")
  {
    
    scan1++;
    Serial.println("recieved 1");
    String raw_CMD;
    
   float input1 = part2.toFloat();
    input1b = input1b+ input1;
    if (scan1==5){
    scan1=0;
    input1 = input1b/5;
    input1b=0;
    raw_CMD=input1;
if (client.connected()){
    Serial.print("Sending payload: ");
    Serial.println(raw_CMD);
     String payload = "-m 'N:" + raw_CMD + "'";
  //  Serial.println(payload);
    if (client.publish("'incoming/OpenWrt/mqtt-Energy/power-grid'", (char*) payload.c_str())) {
     digitalWrite(BUILTIN_LED, LOW); 
      Serial.println("Publish ok");
     // digitalWrite(BUILTIN_LED, HIGH);
    }
    else {
      Serial.println("Publish failed");
    }
}
    }
    
    }
  if (part1== "2")
    {
     scan2++; 
    Serial.println("recieved 2");
    String raw_CMD;
    
    float input2 = part2.toFloat();
        input2b = input2b+ input2;
    if (scan2==5){
    scan2=0;
    input2 = input2b/5;
    input2b=0;
    raw_CMD=input2;
if (client.connected()){
    Serial.print("Sending payload: ");
    Serial.println(raw_CMD);
     String payload = "-m 'N:" + raw_CMD + "'";
   // Serial.println(payload);
    if (client.publish("'incoming/OpenWrt/mqtt-Energy/power-wind'", (char*) payload.c_str())) {
    digitalWrite(BUILTIN_LED, LOW); 
      Serial.println("Publish ok");
     // digitalWrite(BUILTIN_LED, HIGH);
    }
    else {
      Serial.println("Publish failed");
    }
   
    }
    }
    }
 if (part1 == "3")
    {
      scan3++; 
    Serial.println("recieved 3");
    String raw_CMD;
    
    float input3 = part2.toFloat();
           input3b = input3b+ input3;
    if (scan3==5){
    scan3=0;
    input3 = input3b/5;
    input3b=0;
    raw_CMD=input3;
     if (client.connected()){
    Serial.print("Sending payload: ");
    Serial.println(raw_CMD);
     String payload = "-m 'N:" + raw_CMD + "'";
  //  Serial.println(payload);
    if (client.publish("'incoming/OpenWrt/mqtt-Energy/power-solar'", (char*) payload.c_str())) {
    // digitalWrite(BUILTIN_LED, HIGH); 
      Serial.println("Publish ok");
      digitalWrite(BUILTIN_LED, LOW);
    }
    else {
      Serial.println("Publish failed");
    }
     }
    }  
    
  }

  if (part1== "4")
    {
     scan4++; 
    Serial.println("recieved 4");
    String raw_CMD;
    
    float input4 = part2.toFloat();
        input4b = input4b+ input4;
    if (scan4==5){
    scan4=0;
    input4 = input4b/5;
    input4b=0;
    raw_CMD=input4;
if (client.connected()){
    Serial.print("Sending payload: ");
    Serial.println(raw_CMD);
    String payload = "-m 'N:" + raw_CMD + "'";
   // Serial.println(payload);
    if (client.publish("'incoming/OpenWrt/mqtt-Energy/power-divert'", (char*) payload.c_str())) {
    digitalWrite(BUILTIN_LED, LOW); 
      Serial.println("Publish ok");
     // digitalWrite(BUILTIN_LED, HIGH);
    }
    else {
      Serial.println("Publish failed");
    }
   
    }
    }
    }
delay(100);
    }  

    void loop() 
{
  if(WiFi.status() != WL_CONNECTED){setup_wifi();}
  if (!client.connected()) { MQTT_Connect(); }
  client.loop();

   if(Serial.available())
   {
      char c = Serial.read();
    
      if (c == ')')
      {
       
        parseCommand(command);
        
        command="";
      }
      else
      {
       command += c;
       digitalWrite(BUILTIN_LED, HIGH); 
      }
  }
 
}
