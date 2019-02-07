import paho.mqtt.client as mqtt
import time

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("IC.embedded/ALphawolfSquadron/#")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
	print(msg.topic+" "+str(msg.payload))
	
	print("The message was recieved and you got the payload\n" +str(msg.payload))
	

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
		command = input('Enter a number: ')
		
		if command == '1':
			message=client.publish("IC.embedded/ALphawolfSquadron","get_data_light")
			print(mqtt.error_string(message.rc))
			print("I think I published a message")
		if command == '2':
			message=client.publish("IC.embedded/ALphawolfSquadron","get_data_sonic")
			print(mqtt.error_string(message.rc))
			print("I think I published a message")
		if command == 'q':
			client.loop_stop()
			exit()
			
		time.sleep(10)