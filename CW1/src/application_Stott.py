import paho.mqtt.client as mqtt
import time
import json
from tkinter import *

from tkinter import messagebox

#Globals for ease of use
bottom_value = 0
curr_ultra_value = 0
curr_light_value = 0
curr_time = ''

class Window(Frame):

	def __init__(self, master=None):
		Frame.__init__(self, master)               
		self.master = master
		#self.init_window()

	
		# changing the title of our master widget      
		self.master.title("LitterRate Desktop Application")

		# allowing the widget to take the full space of the root window
		self.pack(fill=BOTH, expand=1)

		# creating a button instance
		self.quitButton = Button(self, text="Quit", command=self.client_exit)
		
		# creating a button instance
		self.getDistDataButton = Button(self, text="Get New Reading", command=self.Update1)
		
		# creating a button instance
		self.getLightDataButton = Button(self, text="Calibrate", command=self.calibrate)
		
		#creating the text output area
		self.dataSectionHeader=Label(self, text = "Data Reading")
		self.dataSectionHeader.place(x=50,y=100)
		self.timeLabel=Label(self,text="Time: ")
		self.timeLabel.place(x=50,y=120)
		self.readingLabel=Label(self,text="Reading: ")
		self.readingLabel.place(x=50,y=140)
		self.valueLabel=Label(self,text="Value: ")
		self.valueLabel.place(x=50,y=160)
		self.qualitativeCapacity=Label(self,text="Bin is n/a")
		self.qualitativeCapacity.place(x=50,y=180)
		self.lightQualitative=Label(self,text="Bin state is unknown")
		self.lightQualitative.place(x=50,y=200)
		
		
		# placing the button on my window
		self.quitButton.place(x=300, y=250)
		
		self.getDistDataButton.place (x = 0, y =0)
		self.getLightDataButton.place(x = 250, y =0)
	#function to run when quit button pressed		
	def client_exit(self):
			exit()
	#function run when button "Get New Reading" is pressed
	def Update1(self):
		#Globals containing values from data sent from broker
		global curr_light_value
		global curr_ultra_value
		global curr_time
		global bottom_value
		#publish message to broker asking for data
		message=client.publish("IC.embedded/ALphawolfSquadron/send","get_data")
		print('Changed', curr_ultra_value)
		print('Changed', curr_light_value)
		#Update labels
		self.timeLabel.config(text="Time: " + curr_time)
		self.readingLabel.config(text="Reading-Light: " + str(curr_light_value))
		self.valueLabel.config(text="Reading-Ultra: " + str(curr_ultra_value))
		print("Bottom value is ",bottom_value," Curr ultra is ",curr_ultra_value)
		scaled_val = curr_ultra_value/bottom_value
		print("Scaled value is ", scaled_val)
		#If only python had switch statements
		if scaled_val>1.1: #So in theory, we want this as 1.0, however the sensor has fluctuations
			self.qualitativeCapacity.config(text="Bin is so empty it broke the sensor")
		elif (scaled_val<=1.1) and (scaled_val>0.75):
			self.qualitativeCapacity.config(text="Bin is less than 25% full")
		elif (scaled_val<=0.75)and(scaled_val>0.5):
			self.qualitativeCapacity.config(text="Bin is less than 50% full")
		elif (scaled_val<=0.5)and(scaled_val >0.25):
			self.qualitativeCapacity.config(text="Bin is less than 75% full")
		elif (scaled_val<=0.25) and (scaled_val>0.1):
			self.qualitativeCapacity.config(text="Bin is less than 90% full")
		elif (scaled_val<=0.1) and (scaled_val>0):
			self.qualitativeCapacity.config(text="Bin is full")
		else:
			self.qualitativeCapacity.config(text="I have no idea how you would get this error")
		if curr_light_value>100:
			self.lightQualitative.config(text="Bin is open, capacity may be wrong")
		else:
			self.lightQualitative.config(text="Bin is closed")
	def calibrate(self):
			message=client.publish("IC.embedded/ALphawolfSquadron/send","get_empty")
			messagebox.showinfo("tk", "RE-CALIBRATION COMPLETE")
			
		
	


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("IC.embedded/ALphawolfSquadron/recieve")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
	global curr_light_value
	global curr_ultra_value
	global curr_time
	global bottom_value
	topic=msg.topic
	m_decode=str(msg.payload.decode("utf-8","ignore"))
	#print("data Received type",type(m_decode))
	#print("data Received",m_decode)
	#print("Converting from Json to Object")
	m_in=json.loads(m_decode) #decode json data
	print(type(m_in))
	
	if (len(m_in) == 1):
		bottom_value = int (m_in['ultrasonic_value'])
		print('\nCalibrated the system\n')
		print('\nThe new bottom value is ',bottom_value)
	
	print(m_in)
	curr_ultra_value = int(m_in['ultrasonic_value'])
	print('Changed', curr_ultra_value)
	curr_light_value = int(m_in['lightsensor_value'])
	print('Changed', curr_light_value)
	
	curr_time = str(m_in['time'])
	

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("ee-estott-octo.ee.ic.ac.uk", 1883, 60)


# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_start()

root = Tk()
root.geometry("400x300")
app = Window(root)
root.mainloop()

