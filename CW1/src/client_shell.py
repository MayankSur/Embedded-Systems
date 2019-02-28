#Combined User Interface
## Needed for communication with I2C 

import smbus
## To collect date and time from product
import time
from gpiozero import LED
import paho.mqtt.client as mqtt

import os

# Define location of LED
led = LED(17)

#Define the User Bus
bus = smbus.SMBus(1)

#For memory reasons
#Address = 0x48 # Port ADC


def Read_Value():
	#Sets the address pointer to config register to set up the ADC
	#To the spec that we want
	bus.write_i2c_block_data(0x48,0x1,[4,128])
	
	#Send the value and read x amount of bytes

	data = bus.read_i2c_block_data(0x48,0x0,2)

	value = int.from_bytes(data, 'big')

	print (" Current your bin has this depth: ", value)
	led.on()
	
	time.sleep(1)
	
	led.off()
	
	os.system('clear')
	
	menu()
	
def Read_Value_Proximity():
	#Sets the address pointer to config register to set up the ADC
	#To the spec that we want
	
	## NEED TO CALCULATE THE SENSOR AND HOW IT CONNECTS!!!!!!!!!
	# Very Important---------------- GRRRRR
	##bus.write_i2c_block_data(0x48,0x1,[4,128])
	
	#Send the value and read x amount of bytes

	# data = bus.read_i2c_block_data(0x48,0x0,2)

	# value = int.from_bytes(data, 'big')

	# print (" Current your bin has this depth: ", value)
	# led.on()
	
	# time.sleep(1)
	
	# led.off()
	
	os.system('clear')
	
	# menu()

def menu():
	print("**************************************************\n\r")
	print("*                    LitterRate                  *\n\r")
	print("**************************************************\n\r")
	print("******* Helping clear your litter everyday *******")
	
	print("******* Main Menu *******")
	
	print("1 - Check Bin Level")
	print("2 - Check Bin Utilised")
	print("3 - Message Bin Collection Unit")
	print("q - Quit (don't change resolution)\n\r")
	print("\n\r")
	select_option()
	

def select_option():
	
	choice = input("Please select one of the options avaliable:    ")
	switcher={0:Read_Value, 1:Read_Value_Proximity, 2:Bin_Collected, "q": exit}
	func=switcher.get(choice,lambda : 'Invalid')    
	func()
	
def exit():
	print("**************************************************\n\r")
	print("******* Thank you for using our services!  *******")
	print("**************************************************\n\r")
	os.system('clear')
	print("**************************************************\n\r")
	print("*                    LitterRate                  *\n\r")
	print("**************************************************\n\r")
	print("******* Helping clear your litter everyday *******")
	os._exit()
	
	
menu()

