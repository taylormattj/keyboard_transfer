/*
 * Caps Lock must be off to run and if anything goes wrong setting the caps lock will signal the program to stop
 * This program transfers files that are stored on the root of an microSD card using the keyboard interface
 * The device this is setup for is the Leonardo with microSD card
 * The program assumes to be running on windows and uses notepad and certutil in the command prompt
 * Any interruptions to the PC can cause unintended behavior, so do not try to use the computer when running
 * The transfer rate is about 319 bytes per second
 * The steps used by the program to transfer the files are 
 * 1 - check to see if caps lock is off
 * 2 - open notepad and prompt the user to save the notepad file into the directory that the files are to be transfered into
 * 3 - wait for the user to acknowledge completion using the change of the numbers lock status
 * 4 - open the SD card and begin transferring the data as ASCII hex values
 * 5 - when done transfering the file save the text file and close notepad
 * 6 - open the command prompt and run certutil to convert the text file to a hex file and rename the file correctly
 * 7 - repeat for all the files
 * TODO: use a transfer folder to allow for transfering folder structures too as long as making folders is not hard
 * TODO: ensure all the keyboard commands are defined here incase the libraby changes
 * TODO: flash the led to show that the transfer is taking place
 */

#include "Keyboard.h"
#include <SPI.h>
#include <SD.h>


const int chipSelect = 4;
const int led2Pin = 8;            // output pin for led
const int led1Pin = 13;           // output pin for led
const int conversion_delay = 1000;
const int loop_delay = 1000;
const int flash_delay = 500;
const int write_delay = 3;
const int TRANSFER_FLASH_RATE = 10;
char DIRECTIONS[ ] = "Save this text file into the folder where the contents of the micro-SD card are to be transfered. Press the Num Lock key when done.";
bool num_lock_status = 1;
bool caps_lock_status = 0;
bool led2Pin_status = 0;
bool last_num_lock_status = 1;
int SwitchCase;
byte file_input_byte;
byte byte_count;
char file_input_hex_characters[2];
File root;
File dataFile;

void setup() {
  // make the led pins output:
  pinMode(led1Pin, OUTPUT);
  digitalWrite(led1Pin, LOW); // sets the ledPin off
  pinMode(led2Pin, OUTPUT);
  digitalWrite(led2Pin, LOW); // sets the ledPin off

  // initialize control over the keyboard:
  Keyboard.begin();
  if (!SD.begin(chipSelect)) {
    //Card failed, or not present, don't do anything more
    SwitchCase = 100;
  }  
  // card initialized
  else {
    SwitchCase = 1;
    digitalWrite(led1Pin, HIGH); // sets the ledPin on
  }
  delay(2000);
}

void loop() {
  delay(loop_delay);
  caps_lock_status = Keyboard.getLedStatus(LED_CAPS_LOCK); //continously check the caps lock led
  //stop of the caps lock is turned on
  if (caps_lock_status == 1) {
    SwitchCase = 100;
  }

  switch (SwitchCase) { //main switch statement    
    //open the run window
    case 1: 
      Keyboard.press(KEY_RIGHT_GUI);
      Keyboard.write('r');
      Keyboard.releaseAll();
      SwitchCase++;
    break;
    
    //run notepad
    case 2:
      Keyboard.print("notepad");
      Keyboard.write(KEY_RETURN);
      SwitchCase++;
    break;

    //Print user directions
    case 3: 
      num_lock_status = Keyboard.getLedStatus(LED_NUM_LOCK); //continously check the number lock led
      last_num_lock_status = num_lock_status;
      Keyboard.println(DIRECTIONS);
      SwitchCase++;
    break;

    //wait forever for the num_lock_status to change acknowledging that the file has been saved
    case 4: 
        digitalWrite(led1Pin, HIGH); // sets the ledPin on
        digitalWrite(led2Pin, LOW);  // sets the ledPin off
        delay(flash_delay);
        digitalWrite(led2Pin, HIGH); // sets the ledPin on
        digitalWrite(led1Pin, LOW);  // sets the ledPin off
      num_lock_status = Keyboard.getLedStatus(LED_NUM_LOCK); //continously check the number lock status
      if (last_num_lock_status != num_lock_status) {
        SwitchCase++;
      }
    break;

    //Open the save window
    case 5: 
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.write('s'); //open the save as window
      Keyboard.releaseAll();
      SwitchCase++;
    break;
    
    //enter the address field of the save pop up window
    case 6:
      Keyboard.write(KEY_F4); //enter the address field of the save as window
      SwitchCase++;
    break;
    
    //select all of the address field
    case 7:
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.write('a'); //select all of the address
      Keyboard.releaseAll();
      SwitchCase++;
    break;
    
    //copy the address into the copy buffer and exit the address field
    case 8:
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.write('c'); //copy the address into the copy buffer
      Keyboard.releaseAll();
      Keyboard.write(KEY_ESC); //exit the address field
      SwitchCase++;
    break;
    
    //close te save as window
    case 9:
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.write(KEY_F4); //close te save as window
      Keyboard.releaseAll();
      SwitchCase++;
    break;
    
    //open the save as window
    case 10:
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.write('s'); //open the save as window again
      Keyboard.releaseAll();
      SwitchCase++;
    break;
    
    //save as the temp file
    case 11:
      Keyboard.println("temp_ASCII.txt"); //save the notepad as the temp file
      Keyboard.write(KEY_RETURN);
      SwitchCase++;
    break;
    
    //type 'y' to over write if the file already exists
    case 12:
      Keyboard.write('y'); //yes to over write if the file already exists
      SwitchCase++;
    break;
    
    //delete file contents
    case 13:
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.write('a'); //select all of the file contents
      Keyboard.releaseAll();
      Keyboard.write(KEY_DELETE); //delete all of the files contents
      SwitchCase++;
    break;
    
    //open the root of the SD card
    case 14:
      root = SD.open("/"); //open the root of the SD card
      SwitchCase++;
    break;
    
    //Open file from the sd card
    case 15:
      dataFile = root.openNextFile(); 
      if (!dataFile) { //No more files to transfer
        delay(loop_delay);
        Keyboard.press(KEY_LEFT_CTRL); //save file
        Keyboard.write('s');
        Keyboard.releaseAll();
        delay(loop_delay);
        //close notepad
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.write(KEY_F4);
        Keyboard.releaseAll();
        // no more files to open go to clean up
        SwitchCase = 90;
      }
      else { //file sucessfully opened
        if (dataFile.isDirectory()) { //ignore directories
          //we are not copying over directories just files off the root, open the next file
        }
        else {
          SwitchCase++;
        }
      }
    break;
    
    case 16:
      while (dataFile.available()) {
        if (byte_count >= TRANSFER_FLASH_RATE) { //flash led 2 pin slowly to show a transfer is in progress
          byte_count = 0;
          if (led2Pin_status == 0) {
            led2Pin_status = 1;
            digitalWrite(led2Pin, LOW);  // sets the ledPin off
          }
          else {
            led2Pin_status = 0;
            digitalWrite(led2Pin, HIGH); // sets the ledPin on
          }
          caps_lock_status = Keyboard.getLedStatus(LED_CAPS_LOCK); //check the caps lock led status
          //stop of the caps lock is turned on
          if (caps_lock_status == 1) {
            SwitchCase = 100;
            break;
          }
        }
        else {
          byte_count = byte_count + 1;
        }
        //read in one byte at a time to convert to hex and write out two bytes per byte in
        file_input_byte = dataFile.read();
        file_input_hex_characters[0] = (file_input_byte >> 4) + 0x30;
        if (file_input_hex_characters[0] > 0x39) file_input_hex_characters[0] += 7;
        file_input_hex_characters[1] = (file_input_byte & 0x0f) + 0x30;
        if (file_input_hex_characters[1] > 0x39) file_input_hex_characters[1] += 7;
        Keyboard.write(file_input_hex_characters[0]); //write out the upper nibble in ASCII hex
        delay(write_delay); //delay to stop the keyboard buffer from filling and overflowing
        Keyboard.write(file_input_hex_characters[1]); //write out the lower nibble in ASCII hex
        delay(write_delay); //delay to stop the keyboard buffer from filling and overflowing
      }
      dataFile.close();
      SwitchCase++;
    break;

    //Ctrl+S to save the file temp_ASCII.txt
    case 17:
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.write('s');
      Keyboard.releaseAll();
      SwitchCase++;
    break;

    //Alt+F4 to close notepad
    case 18:
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.write(KEY_F4);
      Keyboard.releaseAll();
      SwitchCase++;
    break;

    //WIN+R to run cmd
    case 19:
      Keyboard.press(KEY_RIGHT_GUI);
      Keyboard.write('r');
      Keyboard.releaseAll();
      SwitchCase++;
    break;

    //open cmd prompt
    case 20:
      Keyboard.println("cmd");
      Keyboard.write(KEY_RETURN);
      SwitchCase++;
    break;

    //convert the text file from ascii to hex
    case 21:
      Keyboard.print("certutil -f -decodeHex ");
      Keyboard.write('"');
      Keyboard.press(KEY_LEFT_CTRL); //paste in the folder address from the copy buffer
      Keyboard.write('v');
      Keyboard.releaseAll();
      Keyboard.print("\\temp_ASCII.txt\" "); //input file
      Keyboard.write('"');
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.write('v');
      Keyboard.releaseAll();
      Keyboard.write('\\');
      Keyboard.print(dataFile.name());
      Keyboard.println("\"");
      Keyboard.print(" >nul");
      Keyboard.write(KEY_RETURN);
      SwitchCase++;
    break;

    //conversion delay
    case 22: 
      delay(conversion_delay);
      SwitchCase++;
    break;

    //close cmd window
    case 23:
      Keyboard.println("exit");
      Keyboard.write(KEY_RETURN);
      SwitchCase++;
    break;

    //WIN+R -> NOTEPAD to open notepad
    case 24:
      Keyboard.press(KEY_RIGHT_GUI);
      Keyboard.write('r');
      Keyboard.releaseAll();
      SwitchCase++;
    break;

    //open notepad
    case 25:
      Keyboard.println("notepad");
      delay(loop_delay);
      Keyboard.write(KEY_RETURN);
      SwitchCase++;
    break;

    //Ctrl+O to open the Open pop up window 
    case 26:
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.write('o');
      Keyboard.releaseAll();
      SwitchCase++;
    break;

    //Ctrl+V to paste in the copied path of the folder
    case 27:
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.write('v');
      Keyboard.releaseAll();
      SwitchCase++;
    break;

    //'\temp.ASCII.txt' after the address as the file name
    case 28:
      Keyboard.println("\\temp_ASCII.txt");
      Keyboard.write(KEY_RETURN);
      SwitchCase++;
    break;

    //delete the contents of the temp file
    case 29:
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.write('a');
      Keyboard.releaseAll();
      Keyboard.write(KEY_DELETE);
      SwitchCase = 15; //go to the case to open the next file
    break;

    //WIN+R to run cmd
    case 90: 
      Keyboard.press(KEY_RIGHT_GUI);
      Keyboard.write('r');
      Keyboard.releaseAll();
      SwitchCase++;
    break;

    //open cmd prompt
    case 91: 
      Keyboard.print("cmd");
      delay(loop_delay);
      Keyboard.write(KEY_RETURN);
      SwitchCase++;
    break;

    case 92: //delete the temp file
      Keyboard.print("del ");
      Keyboard.write('"');
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.write('v');
      Keyboard.releaseAll();
      SwitchCase++;
    break;

    //'\temp.ASCII.txt' after the address as the file name
    case 93:
      Keyboard.print("\\temp_ASCII.txt");
      Keyboard.write('"');
      Keyboard.write(KEY_RETURN);
      SwitchCase++;
    break;

    //close cmd window
    case 94:
      Keyboard.print("exit");
      Keyboard.write(KEY_RETURN);
      SwitchCase = 100;
    break; 

    case 100:
      while (1) {
        digitalWrite(led1Pin, HIGH); // sets the ledPin on
        delay(flash_delay);
        digitalWrite(led1Pin, LOW);  // sets the ledPin off
        delay(flash_delay);
      }
    break;
    
    default:
      SwitchCase = 1;
  }
}
void flash_forever() {
  while (1) {
    digitalWrite(led2Pin, HIGH); // sets the ledPin on
    delay(flash_delay);
    digitalWrite(led2Pin, LOW);  // sets the ledPin off
    delay(flash_delay);
  }
}
