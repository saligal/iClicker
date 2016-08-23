#include <Servo.h>
#include <EEPROM.h>

const byte VERSION = 15;

/* CODES */
const int CODE_GET_CLICK_POS = 1;
const int CODE_SET_CLICK_POS = 2;
const int CODE_GET_RELEASED_POS = 3;
const int CODE_SET_RELEASED_POS = 4;
const int CODE_GET_CLICK2_POS = 5;
const int CODE_SET_CLICK2_POS = 6;
const int CODE_GET_TEMPERATURE = 7;
const int CODE_CLICK = 8;
const int CODE_CLICK2 = 9;
const int CODE_MOVE_CLICKER = 10;
const int CODE_RESET_CLICKER = 11;

const int CLICK_ADDRESS = 0;
const int RELEASED_ADDRESS = 1;
const int CLICK2_ADDRESS = 2;
const int VERSION_ADDRESS = 3;

Servo clicker;
int interval = 1000;

int pos = 0;

float getTemperature() {
  int value = analogRead(18);
  float celsius = value * 250 / 1024.0;
  return celsius;
}

void moveClicker(int pos, bool attach = true, bool detach = true) {
  if (attach)
    clicker.attach(15);
  clicker.write(pos);
  delay(500);
  if (detach)
    clicker.detach();
}

void click(int pos) {
  moveClicker(pos, true, false);
  moveClicker(EEPROM.read(RELEASED_ADDRESS), false, true);
}

void initEEPROMIfNeeded() {
  if (EEPROM.read(VERSION_ADDRESS) != VERSION) {
    // If the versions don't match, it's possible that the EEPROM has never been initialized. Initialize it here.
    EEPROM.update(CLICK_ADDRESS, 120);
    EEPROM.update(RELEASED_ADDRESS, 90);
    EEPROM.update(CLICK2_ADDRESS, 255);
    EEPROM.update(VERSION_ADDRESS, VERSION);
  }
}

void setup() {
  initEEPROMIfNeeded();
  moveClicker(EEPROM.read(RELEASED_ADDRESS));
  Serial.begin(9600); //This pipes to the serial monitor
  Serial.setTimeout(10000);
  while (!Serial) {
    // Wait for serial connection
  }
}

void loop() {
// put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    int incomingCode = Serial.parseInt();
    switch (incomingCode) {
      case CODE_GET_CLICK_POS:
        Serial.println(EEPROM.read(CLICK_ADDRESS), DEC);
        break;
      case CODE_SET_CLICK_POS: {
        int val = Serial.parseInt();
        if (val >= 0 && val <= 180) {
          EEPROM.update(CLICK_ADDRESS, val);
        }
        break;    
      }
      case CODE_GET_RELEASED_POS:
        Serial.println(EEPROM.read(RELEASED_ADDRESS), DEC);
        break;
      case CODE_SET_RELEASED_POS: {
        int val = Serial.parseInt();
        if (val >= 0 && val <= 180) {
          EEPROM.update(RELEASED_ADDRESS, val);
        }
        break;    
      }
      case CODE_GET_CLICK2_POS:
        Serial.println(EEPROM.read(CLICK2_ADDRESS), DEC);
        break;
      case CODE_SET_CLICK2_POS: {
        int val = Serial.parseInt();
        if (val >= 0 && val <= 180) {
          EEPROM.update(CLICK2_ADDRESS, val);
        }
        break;    
      }
      case CODE_MOVE_CLICKER: {
        while (!Serial.available()) {}
        int val = Serial.parseInt();
        if (val >= 0 && val <= 180) {
            moveClicker(EEPROM.read(val));
        }
        break;
      }
      case CODE_GET_TEMPERATURE:
        Serial.println(getTemperature(), DEC);
        break;  
      case CODE_CLICK:
        click(EEPROM.read(CLICK_ADDRESS));
        break;
      case CODE_CLICK2: {
          int val = EEPROM.read(CLICK2_ADDRESS);
          if (val >= 0 && val <= 180) {
            click(val);
          }
          break; }
      case CODE_RESET_CLICKER:
          moveClicker(EEPROM.read(RELEASED_ADDRESS));
          break;
    }
  }
}
