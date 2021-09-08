# keyboard_transfer
Arduino program to transfer files that are stored on the root of an microSD card using the keyboard interface

Caps Lock must be off to run and if anything goes wrong setting the caps lock will signal the program to stop
This program transfers files that are stored on the root of an microSD card using the keyboard interface
The device this is setup for is the Leonardo with microSD card
The program assumes to be running on windows and uses notepad and certutil in the command prompt
Any interruptions to the PC can cause unintended behavior, so do not try to use the computer when running
The transfer rate is about 319 bytes per second
The steps used by the program to transfer the files are 
1 - check to see if caps lock is off
2 - open notepad and prompt the user to save the notepad file into the directory that the files are to be transfered into
3 - wait for the user to acknowledge completion using the change of the numbers lock status
4 - open the SD card and begin transferring the data as ASCII hex values
5 - when done transfering the file save the text file and close notepad
6 - open the command prompt and run certutil to convert the text file to a hex file and rename the file correctly
7 - repeat for all the files
TODO: use a transfer folder to allow for transfering folder structures too as long as making folders is not hard
TODO: ensure all the keyboard commands are defined here incase the libraby changes
