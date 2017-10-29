#!/usr/bin/python

import threading
import serial
import ast

#ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)


from socketIO_client import SocketIO, LoggingNamespace
socketIO = SocketIO('willclark.io:1642')
exitFlag = 0



def on_camera_message(*args):
    print("Python received camera message!")
    

def on_debug_message(*args):
    print("Python receiced debug message!")



class SocketThread(threading.Thread):
    def __init__(self, name):
        threading.Thread.__init__(self)
        self.name = name
    def run(self):
            socketIO.on("cameraMessage", on_camera_message)           
            socketIO.on("debugMessage", on_debug_message)
            socketIO.wait()
            #socketIO.off("messagePi")
            #socketIO.off("sequencePi")

# Create new thread
socketThread = SocketThread("socketThread")


# Start new Thread
socketThread.start()
print("Socket thread has spawned")


#----------Begining of Main Program-------

while (True):
    pass



#If there is anything to read from the Arduino
    if (ser.inWaiting() != 0):
    
        recString = ser.readline().strip()
        recString = recString.decode('utf-8')
        #recString looks like "3210" or .-.- -..- -|-.- .-. -. 
        #print(seqString)

        if (recString[0] == '-' or recString[0] == '.'):
            recString = str(int(time.time()*1000)) + recString
            print(recString)
            socketIO.emit('messagePi', recString)
            
        else:        
            seqLength = len(recString)
            oString = "["
            for i in range(seqLength - 1):
                oString += (recString[i] + ",")
            oString += (recString[seqLength - 1])
            oString += "]"
            #oString looks like "[3,2,1,0]"
            print(oString)
            socketIO.emit('sequencePi', oString)

        oString = ""
        recString = ""
   
socketThread.join()

print ("Exiting Main Thread")
