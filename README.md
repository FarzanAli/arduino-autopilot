# arduino-autopilot
A PID controller based autopilot system for remote control airplanes.


### Ailerons demo
https://github.com/FarzanAli/arduino-autopilot/assets/56518530/e7d850b8-0ff4-4c8c-9b8b-4313d8a899ef

### Accelerometer readings
https://github.com/FarzanAli/arduino-autopilot/assets/56518530/75dca39f-aa9b-45c0-bc00-e38f6888696c

## Challenge(s)
**Problem**: Turning the motor on causes control surfaces (aileron, elevator) to be jittery.  

**Cause**: Vibrations from the motor are felt throughout the structure of the aircraft causing noisy accelerometer readings. These readings are used to calculate the output values (angle in degrees) to the servos. Therefore, noisy readings are causing jittery servo movements.  

**Solution**: Take an average of the readings from the accelerometer to smooth out any noise allowing for smoother servo movements.  

![noise-vs-average](https://github.com/FarzanAli/arduino-autopilot/assets/56518530/6de74bc4-c7d6-4878-9f45-8e26a048c931)

## More pictures
<img width="424" src="https://github.com/FarzanAli/arduino-autopilot/assets/56518530/759c01ff-f98f-47ec-8939-ee8d6d356ff1">
<img width="200" src="https://github.com/FarzanAli/arduino-autopilot/assets/56518530/804e98e3-8159-4563-ab14-dcc99ce2c8cd">
