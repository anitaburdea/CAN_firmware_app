CAN_firmware

Qt application made to send new firmware via CAN bus, usign PCAN-BASIC API - CAN Interface for USB. 
This program comunicate with controller using frames that have an ID, length and data. The IDs are defined by the embedded developer.
This app have a mainwindow, where the user enter the address where the firmware will be written.
Browse the path where the .bin file is located. 
If the user realise that the .bin file is not the good one,
he could push button Upload firmware again and the application will start again.
When writing in flash finished with success or failer, will apear a message box, with the right information about the app status.
When program is in flash memory of the microcontroller, the program will jump to the new firmware.
