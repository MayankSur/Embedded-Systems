import time
from gpiozero import LED
led = LED(17)

led.on()

time.sleep(5)

led.off()
