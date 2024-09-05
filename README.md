# Fan Switching
This project uses an ESP32 DEVKIT to perfom a switching between 3 difernet speed from a celing fan, also it controls the on and off state of the ligth in it


## How does it work
It starts by defining 2 inputs and 4 outputs. One input controls the ligth state, and the other one the fan speed, one output is to turn on and off the ligth and the other 3 are for switching between the three speed levels.

The first input its a simple toggle and the second one uses a case to switch between the off state and the 3 speed levels.

## Considerations
-It is a control that works with interrputs, that is why the "main" only inizializate components and then use an infinite loop to do nothing. 
-Also it uses a timer to avoid bouncing when a buton in pressed but it is non blocking, it just disable the interrpution for a short period of time.
-The next step is to control this via RF like WiFi or Bluetooth.