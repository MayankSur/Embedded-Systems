import smbus
import time
import datetime
	
import paho.mqtt.client as mqtt
from gpiozero import LED

led = LED(17)

bus = smbus.SMBus(1)

# Port for the ultrasonic sensor
Address = 0x48 

# Port for the Light Sensor
Address2 = 0x13

def ultrasonic():
#Sets the address pointer to config register to set up the ADC
#To the spec that we want
	bus.write_i2c_block_data(Address,0x1,[4,128])


	#Send the value and read x amount of bytes

	data = bus.read_i2c_block_data(Address,0x0,2)

	value = int.from_bytes(data, 'big')
	
	led.on()
	led.off()
	
	return value
	
	
def lightsensor():

	#Sets the address pointer to config register to set up the ADC
	#To the spec that we want
	bus.write_i2c_block_data(Address2,0x80,[0x07])
	
	#Send the value and read x amount of bytes
	
	print ("Trying to fetch the values")
	
	#This is the method for using the proximity sensor, it's quite bad and going to be pretty much useless 
	# data = bus.read_i2c_block_data(0x13,0x87,1)
	# data2 = bus.read_i2c_block_data(0x13,0x88,1)
	
	# print ("Got the values")
	
	## Use for light sensor, which is way better - much better readings
	
	data = bus.read_i2c_block_data(Address2,0x85,1)
	data2 = bus.read_i2c_block_data(Address2,0x86,1)
	
	value = int.from_bytes(data, 'big')
	value2 = int.from_bytes(data2, 'big')
	
	value = (value*256)+value2
	
	
	led.on()
	led.off()
	
	return value
	
	
# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("IC.embedded/ALphawolfSquadron/#")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
	print(msg.topic+" "+str(msg.payload))
	
	if (msg.payload) == "get_data_light":
		value = lightsensor()
		time = datetime.datetime.now()
		payload = json.dumps({'lightsensor_value': value, 'time': time.strftime("%c")})	
		message=client.publish("IC.embedded/ALphawolfSquadron",payload)
		print(mqtt.error_string(message.rc))
		print("I think I published a data light message")
		
	if (msg.payload) == "get_data_sonic":
		value = ultrasonic()
		time = datetime.datetime.now()
		payload = json.dumps({'ultrasonic_value': value, 'time': time.strftime("%c")})	
		message=client.publish("IC.embedded/ALphawolfSquadron",payload)
		print("I think I published a sonic message")
		
	if (msg.payload) == "get_data":
		payload = ultrasonic()
		message=client.publish("IC.embedded/ALphawolfSquadron",payload)
		print("I think I published a total message")
	else:
		print (mqtt.error_string(error_code))

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

error_code = client.connect("test.mosquitto.org",port=8884)
client.tls_set(ca_certs="mosquitto.org.crt", certfile="client.crt",keyfile="client.key")

client.loop_forever() 