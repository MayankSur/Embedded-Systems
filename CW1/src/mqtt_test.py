import paho.mqtt.client as mqtt

client = mqtt.Client()
client.tls_set(ca_certs="mosquitto.org.crt", certfile="client.crt",keyfile="client.key")

error_code = client.connect("test.mosquitto.org",port=8884)
if error_code ==0:
	message=client.publish("IC.embedded/ALphawolfSquadron","hello")
	print(mqtt.error_string(message.rc))
	print("I think I published a message")
else:
	print (mqtt.error_string(error_code))
	
