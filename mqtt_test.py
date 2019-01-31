import paho.mqtt.client as mqtt

client = mqtt.Client()
error_code = client.connect("test.mosquitto.org",port=1883)
if error_code ==0:
	message=client.publish("IC.embedded/ALphawolfSquadron","hello")
	print(mqtt.error_string(message.rc))
	print("I think I published a message")
else:
	print (mqtt.error_string(error_code))
	
