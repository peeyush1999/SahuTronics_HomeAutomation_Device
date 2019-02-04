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

ESP8266WebServer server;

#define MyApiKey "762afd75-ab56-4612-815e-87eddaa71f59" // TODO: Change to your sinric API Key. Your API Key is displayed on sinric.com dashboard
#define MySSID "Sahutronics-Desktop" // TODO: Change to your Wifi network SSID
#define MyWifiPassword "sahu@123" // TODO: Change to your Wifi network password


#define deviceOne "5c39fe26dde46d6ba5ec7e28"
#define deviceTwo "5c39fe36dde46d6ba5ec7e2a"
#define deviceThree "5c39fe64dde46d6ba5ec7e2f"
#define deviceFour "5c434ebd2069f45a06714bfc"
 
#define HEARTBEAT_INTERVAL 300000 // 5 Minutes 

#define device1 D5
#define device2 D6
#define device3 D7

uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

void setPowerStateOnServer(String deviceId, String value);
void setTargetTemperatureOnServer(String deviceId, String value, String scale);

// deviceId is the ID assgined to your smart-home-device in sinric.com dashboard. Copy it from dashboard and paste it here

void turnOn(String deviceId) {
  if (deviceId == deviceOne ) // Device ID of first device
  {  
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(device1,LOW);
    setPowerStateOnServer(deviceId,"ON");
  } 
  else if (deviceId == deviceTwo) // Device ID of second device
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(device2,LOW);
    setPowerStateOnServer(deviceId,"ON");
  }
   else if (deviceId == deviceThree) // Device ID of third device
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(device3,LOW);
    setPowerStateOnServer(deviceId,"ON");
  }
   else if (deviceId == deviceFour) // Device ID of fourth device
  { 
    Serial.print("Turn on device id: ");
    Serial.println(deviceId);
    digitalWrite(device1,LOW);
    digitalWrite(device2,LOW);
    digitalWrite(device3,LOW);
    setPowerStateOnServer(deviceId,"ON");
  }
  else {
    Serial.print("Turn on for unknown device id: ");
    Serial.println(deviceId);    
  }     
}

void turnOff(String deviceId) {
   if (deviceId == deviceOne ) // Device ID of first device
   {  
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     digitalWrite(device1,HIGH);
     setPowerStateOnServer(deviceId,"OFF");
   }
   else if (deviceId == deviceTwo ) // Device ID of second device
   { 
     Serial.print("Turn off Device ID: ");
     Serial.println(deviceId);
     digitalWrite(device2,HIGH);
     setPowerStateOnServer(deviceId,"OFF");
  }
   else if (deviceId == deviceThree ) // Device ID of third device
  { 
    Serial.print("Turn off device id: ");
    Serial.println(deviceId);
    digitalWrite(device3,HIGH);
    setPowerStateOnServer(deviceId,"OFF");
  }
   else if (deviceId == deviceFour ) // Device ID of fourth device 
  { 
    Serial.print("Turn off device id: ");
    Serial.println(deviceId);
    digitalWrite(device1,HIGH);
    digitalWrite(device2,HIGH);
    digitalWrite(device3,HIGH);
    setPowerStateOnServer(deviceId,"OFF");
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


//=================================================

const char content[] = "<html><body><h1>SahuTronics</h1><h2>Invent-Inspire-Innovate</h2><form action='/control' method='POST'><fieldset><legend>Control Panel</legend> <br><br>"
  "Device 1 :<button name='device_1' value='OFF' target='myIframe'>OFF</button><button name='device_1' value = 'ON'>ON</button><br><br>"
  "Device 2 :<button name='device_2' value='OFF' target='myIframe'>OFF</button><button name='device_2' value = 'ON'>ON</button><br><br>"
  "Device 3 :<button name='device_3' value='OFF' target='myIframe'>OFF</button><button name='device_3' value = 'ON'>ON</button><br><br>"
  "All Devices :<button name='device_4' value='OFF' target='myIframe'>OFF</button><button name='device_4' value = 'ON'>ON</button><br><br>"
  "</fieldset></form><br>"
  "</body></html>";


void handleRoot() {

/* String content = "<html><body><form action='/control' method='POST'>To log in, please use : admin/admin<br>";
  content += "Device 1 :<button name='device_1' value='OFF' target='myIframe'>OFF</button><button name='device_1' value = 'ON'>ON</button><br>";
  content += "Device 1 :<button name='device_2' value='OFF' target='myIframe'>OFF</button><button name='device_2' value = 'ON'>ON</button><br>";
  content += "<input type='submit' name='SUBMIT' value='Submit'></form><br>";
  content += "You also can go <a href='/inline'>here</a></body></html>";
  */


  
  server.send(200, "text/html",content);
}
void handleControl() {
 if(server.arg("device_1") == "ON")
 {
   turnOn(deviceOne);
   
 }
 else if(server.arg("device_1") == "OFF")
 {
    turnOff(deviceOne);
  
 }
 if(server.arg("device_2") == "ON")
 {
   turnOn(deviceTwo);
   
 }
 else if(server.arg("device_2") == "OFF")
 {
    turnOff(deviceTwo);
  
 }
 
 if(server.arg("device_3") == "ON")
 {
   turnOn(deviceThree);
  
 }
 else if(server.arg("device_3") == "OFF")
 {
    turnOff(deviceThree);
   
 }
 
 if(server.arg("device_4") == "ON")
 {
   turnOn(deviceFour);
 
 }
 else if(server.arg("device_4") == "OFF")
 {
  
    turnOff(deviceFour);
    
 }

  server.sendHeader("Location", String("/"), true);
  server.send(302, "text/html","");
}
void handleNotFound() {
  server.sendHeader("Location", "/",true);   //Redirect to our html web page  
  server.send(302, "text/plane","");
}


void setup() {
  Serial.begin(115200);

  pinMode(device1,OUTPUT);
  pinMode(device2,OUTPUT);
  pinMode(device3,OUTPUT);

  WiFiManager wifiManager;
  wifiManager.autoConnect("SahuTronics");

  
  //-*******************************************************
  
  // server.reset(new ESP8266WebServer(WiFi.localIP(), 80));

  server.on("/", handleRoot);
  server.on("/control", handleControl);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  Serial.println(WiFi.localIP());
  //-**************************************************************

  // server address, port and URL
  webSocket.begin("iot.sinric.com", 80, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);
  webSocket.setAuthorization("apikey", MyApiKey);
  
  // try again every 5000ms if connection has failed
  webSocket.setReconnectInterval(5000);   // If you see 'class WebSocketsClient' has no member named 'setReconnectInterval' error update arduinoWebSockets
}

void loop() {
  webSocket.loop();
  
 /* if(isConnected) {
      uint64_t now = millis();
      
      // Send heartbeat in order to avoid disconnections during ISP resetting IPs over night. Thanks @MacSass
      if((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
          heartbeatTimestamp = now;
          webSocket.sendTXT("H");          
      }
  }*/   
  server.handleClient();
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
