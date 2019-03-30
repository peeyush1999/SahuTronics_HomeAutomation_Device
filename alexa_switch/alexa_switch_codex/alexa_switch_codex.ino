#include <EEPROM.h>

/*
 Version 0.3 - March 06 2018
*/ 

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h> //  https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <ArduinoJson.h> // https://github.com/kakopappa/sinric/wiki/How-to-add-dependency-libraries
#include <StreamString.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager


ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
WiFiClient client;

#define MyApiKey "762afd75-ab56-4612-815e-87eddaa71f59" // TODO: Change to your sinric API Key. Your API Key is displayed on sinric.com dashboard
#define MySSID "Sahutronics-Desktop" // TODO: Change to your Wifi network SSID
#define MyWifiPassword "sahu@123" // TODO: Change to your Wifi network password

#define HEARTBEAT_INTERVAL 300000 // 5 Minutes 

#define device1 D5
#define device2 D6
#define device3 D7
#define device4 D8

#define device1_id "5c39fe26dde46d6ba5ec7e28"
#define device2_id "5c39fe36dde46d6ba5ec7e2a"
#define device3_id "5c39fe64dde46d6ba5ec7e2f"
#define device4_id "5c39fe72dde46d6ba5ec7e31"
#define device5_id "5c434ebd2069f45a06714bfc"

#define btn1 D1
#define btn2 D2
#define btn3 D3
#define btn4 D4

#define addr1 1
#define addr2 2
#define addr3 3
#define addr4 4



String str ="",data="";


char received;

uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

void setPowerStateOnServer(String deviceId, String value);
void setTargetTemperatureOnServer(String deviceId, String value, String scale);
void toggle(String deviceid,int devicepin,int btn,int addr);
// deviceId is the ID assgined to your smart-home-device in sinric.com dashboard. Copy it from dashboard and paste it here

void turnOn(String deviceId) {
  if (deviceId == device1_id) // Device ID of first device
  {  
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(device1,LOW);
    Serial.println("D5 LOW");
    EEPROM.write(addr1,1);
    EEPROM.commit();
  } 
  else if (deviceId == device2_id) // Device ID of second device
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(device2,LOW);
    EEPROM.write(addr2,1); 
    EEPROM.commit();
  }
   else if (deviceId == device3_id) // Device ID of third device
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(device3,LOW);
    EEPROM.write(addr3,1);
    EEPROM.commit();
  }
  else if (deviceId == device4_id) // Device ID of fourth device
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(device4,LOW);
    EEPROM.write(addr4,1);
    EEPROM.commit();
  }
   else if (deviceId == device5_id) // Device ID of all devices device
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(device1,LOW);
    digitalWrite(device2,LOW);
    digitalWrite(device3,LOW);
    digitalWrite(device4,LOW);
    EEPROM.write(addr1,1);
    EEPROM.write(addr2,1);
    EEPROM.write(addr3,1);
    EEPROM.write(addr4,1);
    EEPROM.commit();
  }
  else {
    Serial.print("Turn on for unknown device id: ");
    Serial.println(deviceId);    
  }     
}

void turnOff(String deviceId) {
   if (deviceId == device1_id) // Device ID of first device
   {  
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     digitalWrite(device1,HIGH);
     EEPROM.write(addr1,0);
     EEPROM.commit();
   }
   else if (deviceId == device2_id) // Device ID of second device
   { 
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     digitalWrite(device2,HIGH);
     EEPROM.write(addr2,0);
     EEPROM.commit();
  }
   else if (deviceId == device3_id) // Device ID of third device
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(device3,HIGH);
    EEPROM.write(addr3,0);
    EEPROM.commit();
  }
   else if (deviceId == device4_id) // Device ID of fourth device
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(device4,HIGH);
    EEPROM.write(addr4,0);
    EEPROM.commit();
  }
   else if (deviceId == device5_id) // Device ID of all device 
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(device1,HIGH);
    digitalWrite(device2,HIGH);
    digitalWrite(device3,HIGH);
    digitalWrite(device4,HIGH);
    
    EEPROM.write(addr1,0);
    EEPROM.write(addr2,0);
    EEPROM.write(addr3,0);
    EEPROM.write(addr4,0);
    EEPROM.commit();
  }
  else {
     Serial.print("Turn off for unknown device id: ");
     Serial.println(deviceId);    
  }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      isConnected = false;    
      Serial.printf("[WSc] Webservice disconnected from sinric.com!\n");
      break;
    case WStype_CONNECTED: {
      isConnected = true;
      Serial.printf("[WSc] Service connected to sinric.com at url: %s\n", payload);
      Serial.printf("Waiting for commands from sinric.com ...\n");        
      }
      break;
    case WStype_TEXT: {
        Serial.printf("[WSc] get text: %s\n", payload);
        // Example payloads

        // For Switch or Light device types
        // {"deviceId": xxxx, "action": "setPowerState", value: "ON"} // https://developer.amazon.com/docs/device-apis/alexa-powercontroller.html

        // For Light device type
        // Look at the light example in github
          
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject((char*)payload); 
        String deviceId = json ["deviceId"];     
        String action = json ["action"];
        
        if(action == "setPowerState") { // Switch or Light
            String value = json ["value"];
            if(value == "ON") {
                turnOn(deviceId);
            } else {
                turnOff(deviceId);
            }
        }
        else if (action == "SetTargetTemperature") {
            String deviceId = json ["deviceId"];     
            String action = json ["action"];
            String value = json ["value"];
        }
        else if (action == "test") {
            Serial.println("[WSc] received test command from sinric.com");
        }
      }
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
  }
}

void setup() {
  Serial.begin(9600);
  EEPROM.begin(512);
  pinMode(device1,OUTPUT);
  pinMode(device2,OUTPUT);
  pinMode(device3,OUTPUT);
  pinMode(device4,OUTPUT);
  pinMode(btn1,INPUT_PULLUP);
  pinMode(btn2,INPUT_PULLUP);
  pinMode(btn3,INPUT_PULLUP);
  pinMode(btn4,INPUT_PULLUP);
  digitalWrite(device1,LOW);
  digitalWrite(device2,LOW);
  digitalWrite(device3,LOW);
  digitalWrite(device4,LOW);

/*  EEPROM.write(addr1,1);
  EEPROM.write(addr2,1);
  EEPROM.write(addr3,1);
  EEPROM.write(addr4,1);
  */
  int val1 = EEPROM.read(addr1);
  int val2 = EEPROM.read(addr2);
  int val3 = EEPROM.read(addr3);
  int val4 = EEPROM.read(addr4);

  Serial.print("val1 " + String(val1)+"   val2 " + String(val2)+"     val3 " + String(val3));

  if(val1 == 0)
  {
    digitalWrite(device1,HIGH);
  }
  else
  {
    digitalWrite(device1,LOW);
  }
  if(val2 == 0)
  {
    digitalWrite(device2,HIGH);
  }
  else
  {
    digitalWrite(device2,LOW);
  }

  if(val3 == 0)
  {
    digitalWrite(device3,HIGH);
  }
  else
  {
    digitalWrite(device3,LOW);
  }
  if(val4 == 0)
  {
    digitalWrite(device4,HIGH);
  }
  else
  {
    digitalWrite(device4,LOW);
  }
  

  WiFiManager wifiManager;
  wifiManager.autoConnect("SahuTronics");
 /* WiFiMulti.addAP(MySSID, MyWifiPassword);
  Serial.println();
  Serial.print("Connecting to Wifi: ");
  Serial.println(MySSID);  

  // Waiting for Wifi connect
  while(WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if(WiFiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("WiFi connected. ");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
*/

  // server address, port and URL
  webSocket.begin("iot.sinric.com", 80, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);
  webSocket.setAuthorization("apikey", MyApiKey);
  
  // try again every 5000ms if connection has failed
  webSocket.setReconnectInterval(500);   // If you see 'class WebSocketsClient' has no member named 'setReconnectInterval' error update arduinoWebSockets
}

void loop() {
  webSocket.loop();
  
  if(isConnected) {
      uint64_t now = millis();
      
      // Send heartbeat in order to avoid disconnections during ISP resetting IPs over night. Thanks @MacSass
      if((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
          heartbeatTimestamp = now;
          webSocket.sendTXT("H");          
      }
  } 

//toggling-------
//void toggle(String deviceid,int devicepin,int addr);
if(digitalRead(btn1) == 0 )
{

  toggle(device1_id,device1,btn1,addr1);  
}
if(digitalRead(btn2) == 0 )
{
 
  toggle(device2_id,device2,btn2,addr2);
}
if(digitalRead(btn3) == 0 )
{
  toggle(device3_id,device3,btn3,addr3);
}
if(digitalRead(btn4) == 0 )
{
   toggle(device4_id,device4,btn4,addr4);
}
//------------

readbt();
    
}

// If you are going to use a push button to on/off the switch manually, use this function to update the status on the server
// so it will reflect on Alexa app.
// eg: setPowerStateOnServer("deviceid", "ON")

// Call ONLY If status changed. DO NOT CALL THIS IN loop() and overload the server. 

void setPowerStateOnServer(String deviceId, String value) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["deviceId"] = deviceId;
  root["action"] = "setPowerState";
  root["value"] = value;
  StreamString databuf;
  root.printTo(databuf);
  
  webSocket.sendTXT(databuf);
}

//eg: setPowerStateOnServer("deviceid", "CELSIUS", "25.0")

// Call ONLY If status changed. DO NOT CALL THIS IN loop() and overload the server. 

void setTargetTemperatureOnServer(String deviceId, String value, String scale) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["action"] = "SetTargetTemperature";
  root["deviceId"] = deviceId;
  
  JsonObject& valueObj = root.createNestedObject("value");
  JsonObject& targetSetpoint = valueObj.createNestedObject("targetSetpoint");
  targetSetpoint["value"] = value;
  targetSetpoint["scale"] = scale;
   
  StreamString databuf;
  root.printTo(databuf);
  
  webSocket.sendTXT(databuf);
}

void readbt()
{
   if(Serial.available()>0)
    {
      //while(Serial.available())
     // { //If some thing is received by bluetooth
    received=Serial.read();  //Read the serial buffer
    
    str = str + received;
    //Print it on the serial monitor of arduino
    Serial.println(str);//Print it on the serial monitor of arduino
    //}
    if( str.indexOf("1") >= 0)
    {
      Serial.println("Toggling btn 1");
      togglebt(device1_id,device1,btn1,addr1);
    }
    if( str.indexOf("2") >= 0)
    {
      Serial.println("Toggling btn 2");
      togglebt(device2_id,device2,btn2,addr2);
    }
    if( str.indexOf("3") >= 0)
    {
      Serial.println("Toggling btn 3");
      togglebt(device3_id,device3,btn3,addr3);
    }
    if( str.indexOf("4") >= 0)
    {
      Serial.println("Toggling btn 4");
     togglebt(device4_id,device4,btn4,addr4); 
    }
    str ="";
    
  }
}

void toggle(String deviceid,int devicepin,int btn ,int addr)
{
 // Serial.println("BTN 1 PRESSED");
  while(digitalRead(btn) == 0);
  if(digitalRead(devicepin) == HIGH)
    {
      digitalWrite(devicepin,LOW);
      EEPROM.write(addr,1);
      EEPROM.commit();
    }
  else
    {
      digitalWrite(devicepin,HIGH);
      EEPROM.write(addr,0);
    EEPROM.commit();
    }
  
}
void togglebt(String deviceid,int devicepin,int btn ,int addr)
{
 // Serial.println("BTN 1 PRESSED");
  
  if(digitalRead(devicepin) == HIGH)
    {
      digitalWrite(devicepin,LOW);
      EEPROM.write(addr,1);
      EEPROM.commit();
    }
  else
    {
      digitalWrite(devicepin,HIGH);
      EEPROM.write(addr,0);
    EEPROM.commit();
    }
  
}
