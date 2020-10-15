import RPi.GPIO as GPIO
import time
import datetime
import serial
import csv

GPIO.setmode(GPIO.BOARD)
GPIO.setup(5,GPIO.IN)
GPIO.setwarnings(False)
counter = 0
previous = 0
seconds = 0
flag = 0
dt=datetime.datetime.now()
outputFile=open('anemos.csv','w',newline='') #i'm not sure if this opens and creates the csv file or only opens it 
outputWriter=csv.writer(outputFile)
outputWriter.writerow(['Year','Month','Day','Hour','Min','Sec','MicrSec','Speed'])
outputFile.close()
# ser = serial.Serial(
#   
#     port='/dev/ttyS0',
#     baudrate = 9600,
#     parity=serial.PARITY_NONE,
#     stopbits=serial.STOPBITS_ONE,
#     bytesize=serial.EIGHTBITS,
#     timeout=1
# )
def falling_edge(channel):
    global flag
    global current
    flag = 1
#     print (seconds)    
    current = time.time()
#     seconds = current - previous
#     previous = current
    outputFile.close()
# when a falling edge is detected on port 5, regardless of whatever   
# else is happening in the program, the function falling_edge will be run  
GPIO.add_event_detect(5, GPIO.FALLING, callback=falling_edge, bouncetime=300) 
# while a < 200:
#     if GPIO.input(5):
#         print("HIGH")outputFile=open('anemos.csv','w',newline='')
#     else:
#         print("LOW")
#     time.sleep(0.025)
#     a=a+1

# counter=0
# clear=b'\xFE\x01'
# ser.write(clear)

while 1:
    outputFile=open('anemos.csv','a',newline='')
    outputWriter=csv.writer(outputFile)
    if flag == 1:
        seconds = seconds + (current - previous)
        previous = current
        counter = counter + 1
        flag = 0
        print (seconds)
    if counter == 10:
        seconds = seconds/counter
        dt=datetime.datetime.now()
        speed = (1/seconds)*2.5*0.44704
        outputWriter.writerow([dt.year,dt.month,dt.day,dt.hour,dt.minute,dt.second, dt.microsecond,speed])
        print("%4.2f m/s" % speed)
        seconds = 0
        counter = 0
    outputFile.close()
#     ser.write(clear)
#     ser.write(('Write counter:  %d'%(counter)).encode('utf-8'))
#     time.sleep(1)
#     counter += 1
#     print('Write counter:', counter)
