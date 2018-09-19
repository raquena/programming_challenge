#include <Arduino.h>


// pin definitions //

const unsigned char  PIN_UP_IN = D4;
const unsigned char PIN_DOWN_IN = D1;

const unsigned char  PIN_UP_OUT = D8;
const unsigned char PIN_DOWN_OUT = D0;

// global variavles mailbox //
static unsigned char rolledUp = 0;    // flags of rolling to use in the main loop.
static unsigned char rolledDown = 0;  // count the number of times there's a need to roll down

// function declaration //

void setupIO(void);

// ISR declaration + implementation //

void rollupISR(void){
  rolledUp = rolledUp + 1;        // this is too simple, doesn't clean the noise
}
void rolldownISR(void){
  rolledDown = rolledDown + 1;
}

void setup() {
    setupIO();            // all pin config related stuff
    Serial.begin(115200); // for debugging purpose

    Serial.print("Setup Done");

}

void loop() {

  if(rolledUp > 0){   // this should
    Serial.print("rolled up");
    rolledUp = rolledUp - 1;
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
