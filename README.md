/*-------|---------|---------|---------|---------|---------|---------|---------|
etimer.c	

enlarger timer 

by chaz miller 2013

Controller is a 2-channel so basically two controllers in one.

Each has two knobs, A0 and A1 and B0 and B1 for setting time, button A2
and B2 for starting the timer, and button A3 and B3 for resetting

Additionally there are outputs A4 and A5 for driving panel meters


HARDWARE:
for ATMEGAxx8 set at 1MHz. 
LM335 on PC0 (10mV/K absolute)
3-4V pot on PC1 (temperature setpoint knob)
0-5V pot on PC2 (duty cycle control knob)
SSR and LED connected to PBwhatever

This is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License version 3 or any later
version. This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
