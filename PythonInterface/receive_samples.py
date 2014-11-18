'''
Assumed mode AP=2
If AP=1, put escaped = False in the Zigbee constructor
The received packet is assumed to have 12 bytes of data with 4 bytes each for Voltage, Current and Power
The source_address_long is data[0], source_address_short is data[1] and data is data[3].
data[2] is just the mode which says rx/tx. It's redundant for now.

'''
import serial
import time
from xbee import ZigBee


PORT = 'COM4'
BAUD_RATE = 9600

# Open serial port
ser = serial.Serial(PORT, BAUD_RATE)

zb = ZigBee(ser, escaped = True)

while True:
    try:
        data = zb.wait_read_frame()
        x = data[3];
        v = (int(x[0:2],16))+(int(x[2:4],16))/100.0;
        i = (int(x[4:6],16))/100.0+(int(x[6:8],16))/10000.0;
        p = (int(x[8:10],16))+(int(x[10:12],16))/100.0;
        print 'ID:'+ str(1)+' Voltage:' + str(v) + 'V  '+ 'Current:'+str(i)+'A  '+'Power:'+str(p)+'W';
        
    except KeyboardInterrupt:
        break
