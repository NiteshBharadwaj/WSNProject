## This can be used to read data received on a Digi Module connected via USB.


#! /usr/bin/python

from xbee.zigbee import ZigBee
import serial

PORT = 'COM4'
BAUD_RATE = 9600

# Open serial port
ser = serial.Serial(PORT, BAUD_RATE)

# Create API object
xbee = ZigBee(ser)

# Continuously read and print packets
while True:
    try:
        response = xbee.wait_read_frame()
        print response
    except KeyboardInterrupt:
        break
        
ser.close()
