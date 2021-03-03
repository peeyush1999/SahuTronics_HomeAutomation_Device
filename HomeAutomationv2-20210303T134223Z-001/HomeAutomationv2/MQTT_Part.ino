//#include <ESP8266WiFi.h>
//#include <PubSubClient.h>
// Update these with values suitable for your network.


// Sets the server details server_nmae and Port
void mqtt_setup()
{
   client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  int var=-1;
  if(!strcmp(topic,app_topic1))
  {     Serial.println("Topic one matched");
        if (!strncmp((char *)payload, "ON", length))          
                toggleMQTT(device_1, dev_topic1, address_1, ON);                  
              else  if (!strncmp((char *)payload, "OFF", length))              
                toggleMQTT(device_1, dev_topic1, address_1, OFF);      
  }
  else if(!strcmp(topic,app_topic2))
  { 
     Serial.println("Topic two matched");
     if (!strncmp((char *)payload, "ON", length))              
                toggleMQTT(device_2, dev_topic2, address_2, ON);                  
              else  if (!strncmp((char *)payload, "OFF", length))              
                toggleMQTT(device_2, dev_topic2, address_2, OFF);              
              
  }
  else if(!strcmp(topic,app_topic3))
  {
    Serial.println("Topic three matched");
    if (!strncmp((char *)payload, "ON", length))              
                toggleMQTT(device_3, dev_topic3, address_3, ON);                  
              else  if (!strncmp((char *)payload, "OFF", length))              
                toggleMQTT(device_3, dev_topic3, address_3, OFF); 
  }
  else if(!strcmp(topic,app_topic4))
  {
    Serial.println("Topic three matched");
    if (!strncmp((char *)payload, "ON", length))              
                toggleMQTT(device_4, dev_topic4, address_4, ON);                  
              else  if (!strncmp((char *)payload, "OFF", length))              
                toggleMQTT(device_4, dev_topic4, address_4, OFF); 
  }
  else if(!strcmp(topic,app_topic5))    
    var=5;
//  else if(strcmp(topic,app_topic6))    var=6;
  
 
}
void mqttReconnect()// Try to connect if client not connected
{
  
  unsigned long now = millis();
  if(!client.connected())
    {
      if (now - last_mqtt_try > 5000) 
      {
        reconnect();
        last_mqtt_try = now;
      }
    }
}

void publishState(const char* topic, char* msg)
{ 
  client.publish(topic,msg,true); 
}
void reconnect() {
  // Loop until we're reconnected
  //while (!client.connected()) {
   
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(dev_topic1, (EEPROM.read(address_1)==1)?"ON":"OFF",true);
      client.publish(dev_topic2, (EEPROM.read(address_2)==1)?"ON":"OFF",true);
      client.publish(dev_topic3, (EEPROM.read(address_3)==1)?"ON":"OFF",true);
      client.publish(dev_topic4, (EEPROM.read(address_4)==1)?"ON":"OFF",true);
      
      // ... and resubscribe
      client.subscribe(app_topic1);
      client.subscribe(app_topic2);
      client.subscribe(app_topic3);
      client.subscribe(app_topic4);
      //client.subscribe(app_topic5);
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
    }
 
  //}
}
