import smbus
import time
import datatime

from gpiozero import LED

led = LED(17)

bus = smbus.SMBus(1)

Address = 0x48 # Port ADC

#Sets the address pointer to config register to set up the ADC
#To the spec that we want
bus.write_i2c_block_data(0x13,0x9,[2])



while True:
	led.off()
	#Send the value and read x amount of bytes

	data = bus.read_i2c_block_data(0x13,0x7,1)
	data2 = bus.read_i2c_block_data(0x13,0x8,1)
	
	data = data + data2

	value = int.from_bytes(data, 'big')
	
	print (value)
	led.on()
	
	time.sleep(5)