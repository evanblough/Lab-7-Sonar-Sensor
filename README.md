# Lab 7 Sonar Sensor
#
#These are the instructions we gave to the TI microcontroller. The system communicated with a sonar sensor. 
#The microcontroller would send a start pulse by setting an output pin to high and using the timer hardware to wait for 5 micro seconds.
#The start pulse would trigger the sonar sensor to send out a ping. The sensor would send a high signal back to the microcontroller for the duration 
#that it was waiting and we used the interrupt hardware of the system to capture the time between the start pulse and the ending edge.
#
 We then used the speed of sound and the clock of the system to determine the distance the ping traveled. This indicates the distance of #the object.
#
#
#
#
#
#
#
#
#
