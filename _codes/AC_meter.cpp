# VIRTUAL METER FOR SOYO SOURCE BRAND POWER INVERTERS
# V2.0 - targetting the main feed to zero
#
import bs4
import requests #pip install requests
import time
import datetime
import serial #pip install pyserial
import threading

## CREATE GLOBALS
byte0 = 36
byte1 = 86
byte2 = 0
byte3 = 33
byte4 = 0 ##(2 byte watts as short integer xaxb)
byte5 = 0 ##(2 byte watts as short integer xaxb)
byte6 = 128
byte7 = 8 ## checksum

serialWrite = serial.Serial('/dev/ttyAMA0', 4800, timeout=1) # define serial port on which to output RS485 data
currentOutput = 1
json_use = "http://192.168.86.248/feed/get.json?id=1&field=value&apikey=65631ea6b203f5dfdd36783f13e89e9e"

def jsonSignal(url):
    r = requests.get(url)
    json_data = r.json()
    return json_data

def computeDemand(sourceValue):
    global currentOutput
    equilibrium = currentOutput+use
    if equilibrium >= 2000:
        equilibrium = 2000
    currentOutput = equilibrium
    print"currentOutput =", currentOutput
    return currentOutput
    
def createPacket(demand):
    byte4 = int(demand/256) ## (2 byte watts as short integer xaxb)
    if byte4 <= 0 or byte4 >= 256:
        byte4 = 0
    byte5 = int(demand)-(byte4 * 256) ## (2 byte watts as short integer xaxb)
    if byte5 <= 0 or byte5 >= 256:
        byte5 = 0
    byte7 = (264 - byte4 - byte5) #checksum calculation
    if byte7 >= 256:
        byte7 = 8
    return byte4, byte5, byte7

def writeToSerial(packet):
        bytes = [byte0,byte1,byte2,byte3,packet[0],packet[1],byte6,packet[2]]
        serialWrite.write(bytearray(bytes))
        #log.info("complete decimal packet: %s", bytes)
        #log.info("raw bytearray packet being sent to serial: %s", bytearray(bytes))
    #except ValueError:
        #log.error("Error writing to serial port, check port settings are correct in /dev/ ")
    #return packet

while True:
    use = int(jsonSignal(json_use))
    print"use =", use
    demand = computeDemand(use)
    outputValue = demand/2
    print"demand =", demand
    simulatedPacket = createPacket(outputValue)
    writeToSerial(simulatedPacket)
    time.sleep(2)