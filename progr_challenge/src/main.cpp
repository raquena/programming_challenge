// NOTE; Interrupt input pins need external pull down resistors.

#include <Arduino.h>

#include <ESP8266WiFi.h>

#define DEBUG_SERIAL  // debugging flag

// pin definitions //

const unsigned char  PIN_UP_IN = D2;
const unsigned char PIN_DOWN_IN = D1;

const unsigned char  PIN_UP_OUT = D8;
const unsigned char PIN_DOWN_OUT = D5;

const unsigned char WINDOW_MS = 100;    // time in ms in which we can't make more pulses.
const unsigned char PULSE_WIDTH = 50;   // so high-50ms-low-100m-wait

// global variables mailbox //


static unsigned char rolledUp = 0;    // flags of rolling to use in the main loop.(more than 255 pulses necessary?)
static unsigned char rolledDown = 0;  // count the number of times there's a need to roll down

static unsigned long t0 = 0;    // starting time for ensuring 100ms have passed.
unsigned long t = 0;            // current time

static unsigned char step = 0; // flag that determines in which step for the pulse generation or pause we are




// function declaration //

void setupIO(void);

// ISR declaration + implementation //

void rollupISR(void){
  noInterrupts();
  rolledUp = rolledUp + 1;        // this is too simple, doesn't clean the noise
  interrupts();
}
void rolldownISR(void){
  noInterrupts();
  rolledDown = rolledDown + 1;
  interrupts();
}

void setup() {

  WiFi.mode(WIFI_OFF);
    setupIO();            // all pin config related stuff
    Serial.begin(115200); // for debugging purpose

    Serial.print("Setup Done\n");

    rolledUp = 7;

}

void loop() {

  if(rolledUp > 0){   // number of recorded pulses from the roll.

    if(step == 0){

      #ifdef DEBUG_SERIAL
        Serial.print("\nrolled Up ");
        Serial.print(rolledUp);
        Serial.print("\n");
        Serial.print(t0);
        Serial.print('\n');
      #endif

      digitalWrite(PIN_UP_OUT,HIGH);    // we start the pulse
      t0 = millis();                      // start counting time
      step = 1;
    }else if(step == 1){
      if((t = millis()) > t0 + PULSE_WIDTH){
        digitalWrite(PIN_UP_OUT,HIGH);  // finish pulse, start pause
        step = 2;
      }
    }else if(step == 2){
      if((t = millis()) > t0 + PULSE_WIDTH + WINDOW_MS){
        step = 0;                           // finish pause
        rolledUp = rolledUp - 1;
        rolledDown = 0; // NOT IN SPECIFICATION!!! for useability reasons, if we turn up, we clear going down
      }
    }
  }


if(rolledDown > 0){   // number of recorded pulses from the roll.

    if(step == 0){

      #ifdef DEBUG_SERIAL
        Serial.print("\nrolled down ");
        Serial.print(rolledDown);
        Serial.print("\n");
        Serial.print(t0);
        Serial.print('\n');
      #endif

      digitalWrite(PIN_DOWN_OUT,HIGH);    // we start the pulse
      t0 = millis();                      // start counting time
      step = 1;
    }else if(step == 1){
      if((t = millis()) > t0 + PULSE_WIDTH){
        digitalWrite(PIN_DOWN_OUT,HIGH);  // finish pulse, start pause
        step = 2;
      }
    }else if(step == 2){
      if((t = millis()) > t0 + PULSE_WIDTH + WINDOW_MS){
        step = 0;                           // finish pause
        rolledDown = rolledDown - 1;
        rolledUp = 0; // NOT IN SPECIFICATION!!! for useability reasons, if we turn up, we clear going down
      }
    }
  }
}


// function implementation //

void setupIO(void){

  pinMode(PIN_UP_IN, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_UP_IN), rollupISR, RISING);
  pinMode(PIN_DOWN_IN, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_DOWN_IN), rolldownISR, RISING);

  pinMode(PIN_UP_OUT, OUTPUT);
  pinMode(PIN_DOWN_OUT, OUTPUT);
}
