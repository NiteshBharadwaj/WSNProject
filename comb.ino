/**
* This is modified from the library example to suit our needs. To test communication between a remote arduino connected to digi865 and a central co-ordinator connected to laptop.
* Setup (to be done once):
* 1) Connect the co-ordinator digi to laptop and set AP =1, baudrate=9600 using XCTU. Note its address.
* 2) Connect the test digi to laptop and set AP =2, baudrate=9600 using XCTU.
* Test:
* 1) Run PythonInterface/receive_samples.py on co-ordinator laptop.
* 2) Connect the sensor to analog pin (pin5) on remote/test arduino.
* 3) Run this code on test arduino
* 4) Check if the data is being received on co-ordinator
*/

/**
 * Copyright (c) 2009 Andrew Rapp. All rights reserved.
 *
 * This file is part of XBee-Arduino.
 *
 * XBee-Arduino is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * XBee-Arduino is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XBee-Arduino.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <XBee.h>

/*
This example is for Series 2 XBee
 Sends a ZB TX request with the value of analogRead(pin5) and checks the status response for success
*/

// create the XBee object
XBee xbee = XBee();

uint8_t payload[] = { 0, 0, 0, 0, 0, 0 };

// SH + SL Address of receiving XBee
XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x40bb1689);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();

int pin5 = 0;

int statusLed = 13;
int errorLed = 13;

int vPin = A1;
int iPin = A2;

boolean start = true;
float startT = 0;

int noO = 0;
float vsqr = 0;
float isqr = 0;
float vi = 0;


void flashLed(int pin, int times, int wait) {

  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(wait);
    digitalWrite(pin, LOW);

    if (i + 1 < times) {
      delay(wait);
    }
  }
}

void setup() {
  pinMode(statusLed, OUTPUT);
  pinMode(errorLed, OUTPUT);
  pinMode(vPin,INPUT);
  pinMode(iPin, INPUT);

  Serial.begin(9600);
  xbee.setSerial(Serial);
}

void loop() {  
   
   if (start) {
    start = false;
    startT = millis();
  }
  else {
    if ((millis()-startT)<1000) {
      float v = ((analogRead(vPin)/1023.0)*5-3.3)*100;
      float i = ((analogRead(iPin)/1023.0)*5-3.3)/0.6;
      vsqr = vsqr + v*v;
      isqr = isqr + i*i;
      vi = vi + v*i;
      noO = noO + 1;
    }   
    else {
      start = true;
      startT=0;
      float vRms = sqrt(vsqr/noO);
      float iRms = sqrt(isqr/noO)*100;
      float power = vi/noO; 
      uint8_t v1=(int)(floor(vRms))&0xff;
      uint8_t v2=(int)((vRms-floor(vRms))*1000)>>8 & 0xff;
      uint8_t v3=(int)(floor(iRms))&0xff;
      uint8_t v4=(int)((iRms-floor(iRms))*1000)>>8 & 0xff;
      uint8_t v5=(int)(abs(power))&0xff;
      uint8_t v6=(int)((abs(power)-(int)(abs(power)))*1000)>>8 & 0xff;
      payload[0] = v1;
      payload[1] = v2;
      payload[2] = v3;
      payload[3] = v4;
      payload[4] = v5;
      payload[5] = v6;
      
      noO = 0;
      isqr = 0;
      vsqr = 0;
      vi = 0;   
      xbee.send(zbTx);
  
    // flash TX indicator
    flashLed(statusLed, 1, 100);
  
    // after sending a tx request, we expect a status response
    // wait up to half second for the status response
    if (xbee.readPacket(500)) {
      // got a response!
  
      // should be a znet tx status            	
      if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
        xbee.getResponse().getZBTxStatusResponse(txStatus);
  
        // get the delivery status, the fifth byte
        if (txStatus.getDeliveryStatus() == SUCCESS) {
          // success.  time to celebrate
          digitalWrite(13,HIGH);
        } else {
          // the remote XBee did not receive our packet. is it powered on?
          digitalWrite(13,LOW);
        }
      }
    } else if (xbee.getResponse().isError()) {
      //nss.print("Error reading packet.  Error code: ");  
      //nss.println(xbee.getResponse().getErrorCode());
    } else {
      // local XBee did not provide a timely TX Status Response -- should not happen
      flashLed(errorLed, 2, 50);
    }  
    }
  } 
}
