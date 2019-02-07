import paho.mqtt.client as mqtt
import time
import json

bottom_value = 0
curr_ultra_value = 0
curr_light_value = 0


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("IC.embedded/ALphawolfSquadron/recieve")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
	topic=msg.topic
	m_decode=str(msg.payload.decode("utf-8","ignore"))
	#print("data Received type",type(m_decode))
	#print("data Received",m_decode)
	#print("Converting from Json to Object")
	m_in=json.loads(m_decode) #decode json data
	print(type(m_in))
	
	if (len(m_in) == 1):
		bottom_value = m_in['ultrasonic_value']
		print('\nCalibrated the system\n')
	
	print(m_in)
	curr_ultra_value = int(m_in['ultrasonic_value'])
	print('entered')
	curr_light_value = int(m_in['lightsensor_value'])
	print('entered')
	
	
	

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("test.mosquitto.org", 1883, 60)


# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_start()

while True:
		time.sleep(4)
		command = input('Enter a number: ')
		
		if command == '1':
			message=client.publish("IC.embedded/ALphawolfSquadron/send","get_data_light")
			#print(mqtt.error_string(message.rc))
			#print("I think I published a message")
		if command == '2':
			message=client.publish("IC.embedded/ALphawolfSquadron/send","get_data_sonic")
			#print(mqtt.error_string(message.rc))
			#print("I think I published a message")
		if command == '3':
			message=client.publish("IC.embedded/ALphawolfSquadron/send","get_empty")
			#print(mqtt.error_string(message.rc))
			#print("I think I published a message")
		if command == '4':
			message=client.publish("IC.embedded/ALphawolfSquadron/send","get_data")
			#print(mqtt.error_string(message.rc))
			#print("I think I published a message")
		if command == 'q':
			client.loop_stop()
			exit()
			