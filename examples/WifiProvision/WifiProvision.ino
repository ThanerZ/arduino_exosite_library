//*****************************************************************************
//
// wifi_read_write_string - A Simple read/write to the Exosite Cloud API
//                          for the Arduino WiFi shield using the
//                          String library
//
// Copyright (c) 2013 Exosite LLC.  All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:

//  * Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright 
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of Exosite LLC nor the names of its contributors may
//    be used to endorse or promote products derived from this software 
//    without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
//*****************************************************************************

#include <SPI.h>
#include <WiFi.h>
#include <Exosite.h>
#include <EEPROM.h>

/*==============================================================================
* Configuration Variables
*
* Change these variables to your own settings.
*=============================================================================*/
char ssid[] = ""; // <-- Fill in your network SSID (name)
char pass[] = ""; // <-- Fill in your network password

// Use these variables to customize what datasources are read and written to.
String readString = "command&uptime";
String writeString = "uptime=";
String returnString;

// Number of Errors before we try a reprovision.
const unsigned char reprovisionAfter = 3;

/*==============================================================================
* End of Configuration Variables
*=============================================================================*/
unsigned char errorCount = reprovisionAfter;  // Force Provision On First Loop
char macString[18];  // Used to store a formatted version of the MAC Address
byte macData[WL_MAC_ADDR_LENGTH];

int status = WL_IDLE_STATUS;
WiFiClient client;
Exosite exosite(&client);

/*==============================================================================
* setup
*
* Arduino setup function.
*=============================================================================*/
void setup(){  
  Serial.begin(115200);
  Serial.println("Boot");
  
  
  // Create MAC Address String in the format FF:FF:FF:FF:FF:FF
  WiFi.macAddress(macData);
  snprintf(macString, 18, "%02X:%02X:%02X:%02X:%02X:%02X",
           macData[5], macData[4], macData[3], macData[2], macData[1], macData[0]);
  
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while(true);
  } 
  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    status = WiFi.begin(ssid, pass);
  }
  Serial.println("Connected to Access Point");
  
  // Print Some Useful Info
  Serial.print(F("MAC Address: "));
  Serial.println(macString);
  
  Serial.print(F("IP Address: "));
  Serial.println(WiFi.localIP());
}

/*==============================================================================
* loop 
*
* Arduino loop function.
*=============================================================================*/
void loop(){
  // Check if we should reprovision.
  if(errorCount >= reprovisionAfter){
    if(exosite.provision("exosite", "ard-generic", macString)){
      errorCount = 0;
    }
  }
  
  //Write to "uptime" and read from "uptime" and "command" datasources.
  if ( exosite.writeRead(writeString+String(millis()), readString, returnString)){
    Serial.println("OK");
    Serial.println(returnString);
    errorCount = 0;
  }else{
    Serial.println("Error");
    errorCount++;
  }

  delay(5000);
}

