void toggleAlexa(int device,const char *topic,bool state, int address)
{ 

    
    (state==true)?digitalWrite(device,ON):digitalWrite(device,OFF);
    client.publish(topic, state==true?"ON":"OFF", true); // considering after complementing
    EEPROM.write(address,state==true?1:0);
    EEPROM.commit();
}


void toggleBluetooth(int device,const char* topic,int address)
{
     int val = EEPROM.read(address);
     // Reverse Logic toggling the previous value.
    (val==1)?digitalWrite(device,OFF):digitalWrite(device,ON);
    
    client.publish(topic, val==1?"OFF":"ON" , true); // considering after complementing
  
    EEPROM.write(address,val==1?0:1);
    EEPROM.commit();
}


void toggleMQTT(int device,const char* topic,int address, int state)
{
    Serial.println("Pin " + String(device) + "State" + String(state) );
    digitalWrite(device, state==ON?ON:OFF);
    client.publish(topic, state==ON?"ON":"OFF", true); // considering after complementing
    EEPROM.write(address,state==ON?1:0);
    EEPROM.commit();
  
}
