import smbus
import time

from gpiozero import LED

led = LED(17)

bus = smbus.SMBus(1)

Address = 0x48 # Port ADC

#Sets the address pointer to config register to set up the ADC
#To the spec that we want
bus.write_i2c_block_data(0x48,0x1,[4,128])


while True:
	led.off()
	#Send the value and read x amount of bytes

	data = bus.read_i2c_block_data(0x48,0x0,2)

	value = int.from_bytes(data, 'big')

	print (value)
	led.on()
	
	time.sleep(1)