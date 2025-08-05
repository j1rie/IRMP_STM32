## Macros
A macro allows a series of command sequence to be sent via the IR transmitter diode with the press of a button.
macroslot0 is the trigger, macroslot1 ... macroslot7 are the commands to be sent when the trigger is received.

Example:  
Button A is configured in macro0-macroslot0.  
Button B is configured in macro0-macroslot1.  
Button C is configured in macro0-macroslot2.  
Button D is configured in macro0-macroslot3.  
Button E is configured in macro0-macroslot4.  
If A is pressed and received, B, C, D and E are transmitted in sequence via the IR transmitter diode.

Practical application:  
On “Power” the VDR should come on and the TV and the receiver and ...  
“Power” goes to wakeupslot0 and macro0-macroslot0.  
“TV on” goes to macro0-macroslot1.  
“Receiver on” goes to macro0-macroslot2.  
...

## Enter macros with stm32kbdIRconfig_gui
Select macro and macroslot with “select”. The trigger goes into macroslot0, e.g. with the remote control. The commands to be sent go into macroslot1 and following, also e.g. with the remote control.  
If a trigger is ffffffffffffffff, sending is terminated, and if an IR code to be sent is ffffffffffffffff, the next macro continues.

## Technical
In a new eeprom and an eeprom after reset all bytes are '0xff'. That's for technical reasons, how erasing flash works.  
So '0xff' means yet unwritten.

After IR reception the received IR code will be compared with all macro triggers one after the other. If a trigger is 'ffffffffffff', parsing will stop.  
If the received IR code and the trigger match, the accompanying IR codes will be sent, but again at 'ffffffffffff' transmission will stop.  
So 'ffffffffffff' is the stop mark for both triggers and to be sent IR codes.
This is practical, because the 'ffffffffffff's are there already from the beginning, due to the eeprom properties.

## Special case: last macro
Here, the trigger is an expired alarm timer.
The transmission starts with macro7-macroslot1.