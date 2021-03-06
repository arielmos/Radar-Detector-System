# Radar Detector System - Digital Computer Structure - Final Project
 •	Design and implementation of an MCU-based (MSP430G2553) radar system for monitoring and detecting objects in space using an ultrasonic odometer and Servo motor.
 The space scan performs in a 180-degree section using Servo motor movement, the measuring range using the odometer ranges from 2M-4.5M.
 The angular motion control of a servo motor is PWM based.
   
 •	As part of the project, a code in the C ++ / C language developed for the implementation of a real-time system based on interrupts (MCU side) for components activation and data reading from the odometer.
 
 •	For display and user interface, a PC is being used on which the radar image displayed.
The MCU connected to the PC using asynchronous serial communication (UART) in RS-232 standard.
 
 •	The user interface on the PC side is a Python based and allow setting parameters, sending files and high-level commands to the MCU and display radar image on PC. 
 
 •	The interface allow the transfer of files that include high-level coded commands for implementation on the controller side.
The files in the controller is being stored in FLASH memory.

# Flowchart of Software Architecture -
![flow_chart](https://user-images.githubusercontent.com/54889635/131258519-26865a66-b3b0-495c-a4a4-4ac360b87452.png)

# Setup -
![image](https://user-images.githubusercontent.com/54889635/131258555-fcb529b9-113d-46a2-8760-2b565132c021.png)

# GUI Example -
![image](https://user-images.githubusercontent.com/54889635/131258559-27c0ecbd-cbef-4d01-9aeb-a06e5979f322.png)
![image](https://user-images.githubusercontent.com/54889635/131258621-de1ee330-d65f-4363-af92-058e1669af2e.png)
