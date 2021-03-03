#include <ESPAsyncWiFiManager.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include "fauxmoESP.h"
#include "ESPAsyncWebServer.h"
#include <EEPROM.h>



// Macro Definitions=======
#define device_1 16  //D0
#define device_2 5  //D1
#define device_3 4   //D2
#define device_4 0  //D3

#define btn_1 D5
#define btn_2 D6
#define btn_3 D7
#define btn_4 D8

#define address_1 101   // State of Device 1
#define address_2 102   // State of Device 2
#define address_3 103   // State of Device 3
#define address_4 104   // State of Device 4
#define address_5 105   // State of Device 5 Not implemented

#define ON LOW
#define OFF HIGH

#define ALEXA 10

#define ALARM1_H 20
#define ALARM1_M 21
#define ALARM1_DEVICE 22


#define ALARM2_H 23
#define ALARM2_M 24
#define ALARM2_DEVICE 25

#define ALARM3_H 26
#define ALARM3_M 27
#define ALARM3_DEVICE 28

#define ALARM4_H 29
#define ALARM4_M 30
#define ALARM4_DEVICE 31
//==================


//Prototypes======================

void mqtt_setup();
void serverSetup();
void freeHeap();
void callback(char* topic, byte* payload, unsigned int length);
void mqttReconnect();
void publishState(String topic, String msg);
void reconnect() ;
void readbt();
void toggle(int devicepin,int btn ,int addr);
//================================
DNSServer dns;
fauxmoESP fauxmo;

WiFiClient espClient;
PubSubClient client(espClient);

//========Bluetooth=============
String str ="",data="";
char received;

//==============================

//Alexa===

AsyncWebServer server(80);
unsigned long last_heap_free=0;


//==MQTT
const char* ssid = "Tarpit's Wifi !";
const char* password = "3213213213";
const char* mqtt_server = "broker.mqtt-dashboard.com";

//topics for deviceto publish
const char* dev_topic1 = "sahutronics/device/device1";
const char* dev_topic2 = "sahutronics/device/device2";
const char* dev_topic3 = "sahutronics/device/device3";
const char* dev_topic4 = "sahutronics/device/device4";
const char* dev_topic5 = "sahutronics/device/device5";

//topics for app to publish and device suscribe it
const char* app_topic1 = "sahutronics/app/device1";
const char* app_topic2 = "sahutronics/app/device2";
const char* app_topic3 = "sahutronics/app/device3";
const char* app_topic4 = "sahutronics/app/device4";
const char* app_topic5 = "sahutronics/app/device5";

const char* topic6 = "sahutronics/alarm";

unsigned long last_mqtt_try = 0;
char msg[50];
int value = 0;

//Try to connect to wifi
   AsyncWiFiManager wifiManager(&server,&dns);


//===============
void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  EEPROM.begin(512);
  
  pinMode(device_1,OUTPUT);
  pinMode(device_2,OUTPUT);
  pinMode(device_3,OUTPUT);
  pinMode(device_4,OUTPUT);
  pinMode(btn_1,INPUT_PULLUP);
  pinMode(btn_2,INPUT_PULLUP);
  pinMode(btn_3,INPUT_PULLUP);
  pinMode(btn_4,INPUT_PULLUP);
  /*digitalWrite(device_1,LOW);
  digitalWrite(device_2,LOW);
  digitalWrite(device_3,LOW);
  digitalWrite(device_4,LOW);
  */
  int val1 = EEPROM.read(address_1);
  int val2 = EEPROM.read(address_2);
  int val3 = EEPROM.read(address_3);
  int val4 = EEPROM.read(address_4);

  Serial.print("val1 " + String(val1)+"   val2 " + String(val2)+"     val3 " + String(val3));

  (val1==1)?digitalWrite(device_1,ON):digitalWrite(device_1,OFF);
  (val2==1)?digitalWrite(device_2,ON):digitalWrite(device_2,OFF);
  (val3==1)?digitalWrite(device_3,ON):digitalWrite(device_3,OFF);
  (val4==1)?digitalWrite(device_4,ON):digitalWrite(device_4,OFF);
  
  
  if(!wifiManager.autoConnect("Sahutronics"))
  {
    Serial.printf("Not able to connect \n When u want to connect press btn1 and btn4 for 3 sec");
  }
  else
  {
    Serial.printf("Connected to wifi");
  }

  //===============Mqtt=================
  mqtt_setup();
  //MQTT ENd============================
  /*========================
   * Alexa Part - Start
   * =======================
   */

  // web server after connecting with wifi
  serverSetup();
  fauxmo.createServer(false);
  fauxmo.setPort(80); // This is required for gen3 devices
  fauxmo.enable(true);
  fauxmo.addDevice("Device One");
  fauxmo.addDevice("Device Two");
  fauxmo.addDevice("Device Three");
  fauxmo.addDevice("Device Four");
  fauxmo.addDevice("All Devices");

  
  
   fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
        
        // Callback when a command from Alexa is received. 
        // You can use device_id or device_name to choose the element to perform an action onto (relay, LED,...)
        // State is a boolean (ON/OFF) and value a number from 0 to 255 (if you say "set kitchen light to 50%" you will receive a 128 here).
        // Just remember not to delay too much here, this is a callback, exit as soon as possible.
        // If you have to do something more involved here set a flag and process it in your main loop.
        
        if (0 == device_id) 
        {
          /*digitalWrite(device_1, state);
          client.publish(topic1, state>=200?"ON":"OFF", true); // considering after complementing
          EEPROM.write(address_1,state>=200?1:0);
          EEPROM.commit();*/

          toggleAlexa(device_1,dev_topic1,state,address_1);
        }
        if (1 == device_id)
          toggleAlexa(device_2,dev_topic2,state,address_2);
        
        if (2 == device_id)
          toggleAlexa(device_3,dev_topic3,state,address_3);
        
        if (3 == device_id) 
        toggleAlexa(device_4,dev_topic4,state,address_4);
        
        if (4 == device_id) 
        {
          toggleAlexa(device_1,dev_topic1,state,address_1);
          toggleAlexa(device_2,dev_topic2,state,address_2);
          toggleAlexa(device_3,dev_topic3,state,address_3);
          toggleAlexa(device_4,dev_topic4,state,address_4);
        }
        
        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id+1, device_name, state ? "ON" : "OFF", value);

        // For the example we are turning the same LED on and off regardless fo the device triggered or the value
       // digitalWrite(LED, !state); // we are nor-ing the state because our LED has inverse logic.

    });
  
  /*========================
   * Alexa Part - End
   * =======================
   */
}

void loop() {

    fauxmo.handle();

    //=======Open Hotsopt=======
    openHotspot(); // if btn1 and btn3 pressed for 3 sec
    //==========================
    
    //=======MQTT=========
    if (!client.connected()) {
        reconnect();
      }
      client.loop();
    //========MQTT End=========


    //=======Button Press===========

    
       

    //==============================


    //===========Read Bluetooth======
      readbt();
    //===============================

    
    
    // Functions Performing 5 second Ccheck
      WifiautoConnect();
      freeHeap();
      


}




//================
unsigned long last_wifi_try=0;
void WifiautoConnect()// Try to connect if device not connected
{
  unsigned long now = millis();
  if(WiFi.status() != WL_CONNECTED)
    {
      if (now - last_mqtt_try > 5000) 
      {
        wifiManager.autoConnect();
        last_wifi_try = now;
      }
    }
}

void openHotspot()
{
  unsigned long now = millis();
  unsigned long duration;
    while(digitalRead(btn_1)==LOW && digitalRead(btn_2)==LOW)
    {
      
      duration = millis();
      
      if (now - duration > 3000) 
      {
        wifiManager.startConfigPortal("Sahutronics",NULL);
       
      }
    }
  
}
//===================
