import smbus
import time

from gpiozero import LED

led = LED(17)

bus = smbus.SMBus(1)

#Address = 0x48 # Port ADC

#Sets the address pointer to config register to set up the ADC
#To the spec that we want
bus.write_i2c_block_data(0x13,0x80,[0x07])


while True:
	led.off()
	#Send the value and read x amount of bytes
	
	print ("Trying to fetch the values")
	
	#This is the method for using the proximity sensor, it's quite bad and going to be pretty much useless 
	# data = bus.read_i2c_block_data(0x13,0x87,1)
	# data2 = bus.read_i2c_block_data(0x13,0x88,1)
	
	# print ("Got the values")
	
	## Use for light sensor, which is way better - much better readings
	
	data = bus.read_i2c_block_data(0x13,0x85,1)
	data2 = bus.read_i2c_block_data(0x13,0x86,1)
	
	value = int.from_bytes(data, 'big')
	value2 = int.from_bytes(data2, 'big')
	
	value = (value*256)+value2
	
	print (value)
	led.on()
	
	time.sleep(2)