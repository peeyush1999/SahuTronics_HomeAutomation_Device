void readbt()
{
   if(Serial.available()>0)
    {
      //while(Serial.available())
      //{ //If some thing is received by bluetooth
    received=Serial.read();  //Read the serial buffer
    
    str = str + received;
      
    //Print it on the serial monitor of arduino
    Serial.print(str);//Print it on the serial monitor of arduino
    //}
    if(str=="a")
    {
      
      toggleBluetooth(device_1,dev_topic1,address_1);
      Serial.println("Bluetooth : Toggling btn 1");
      
      
    }
    if(str=="b")
    {
      toggleBluetooth(device_2,dev_topic2,address_2);
      Serial.println("Bluetooth : Toggling btn 2");
      
    }
    if(str=="c")
    {
      toggleBluetooth(device_3,dev_topic3,address_3);
      Serial.println("Bluetooth : Toggling btn 3");
    }
    if(str=="d")
    {
      toggleBluetooth(device_4,dev_topic4,address_4);
      Serial.println("Bluetooth : Toggling btn 4");
    }
    str ="";
    
  }
}
