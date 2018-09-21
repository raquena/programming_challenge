#include <Arduino.h>

#include <ESP8266WiFi.h>


// pin definitions //

const unsigned char  PIN_UP_IN = D3;
const unsigned char PIN_DOWN_IN = D1;

const unsigned char  PIN_UP_OUT = D8;
const unsigned char PIN_DOWN_OUT = D0;

// global variables mailbox //

static unsigned char rolledUpHigh = 0;  // for debouncing pulses if needed
static unsigned char rolledUpLow = 0;

static unsigned char rolledUp = 0;    // flags of rolling to use in the main loop.
static unsigned char rolledDown = 0;  // count the number of times there's a need to roll down

// function declaration //

void setupIO(void);

// ISR declaration + implementation //

void rollupISR(void){
  noInterrupts();
  rolledUp = rolledUp + 1;        // this is too simple, doesn't clean the noise
  Serial.print("aaa");
  interrupts();
}
void rolldownISR(void){
  rolledDown = rolledDown + 1;
}

void setup() {

  WiFi.mode(WIFI_OFF);
    setupIO();            // all pin config related stuff
    Serial.begin(115200); // for debugging purpose

    Serial.print("Setup Done\n");

    rolledUp = 7;

}

void loop() {

  if(rolledUp > 0){   // this should
    Serial.print("rolled up\n");
    // create a pulsed signal during 100ms, to give time to the bt deivce to detect it.
    // this is blocking code, it can be done better.
    // there is still room for uncertainity, if no acknowledge from the bt device exists.

    // ANOTHER GOOD OPTION TO REPLACE THIS WITH ARDUINO CODE:
    // Arduino AnalogWrite, generates PWM signal, (in the esp8266@11KHz)
    // AnalogWrite(PIN_UP_OUT,512);  // 50% duty
    // delay(100);
    for(unsigned char i = 0; i < 50; i++){  //  creating faster edges for the bt to detect.
        digitalWrite(PIN_UP_OUT,HIGH);
        delay(1);
        digitalWrite(PIN_UP_OUT,LOW);
        delay(1);
    }

    rolledUp = rolledUp - 1;
  }
  if(rolledDown > 0){   // this should
    Serial.print("\nrolled down ");
    Serial.print(rolledDown);
    Serial.print("\n");
    rolledDown = rolledDown - 1;

    // this was useful to generate pulses, not needed anymore.
/*
    for(unsigned char i = 0; i < 50; i++){
        Serial.print("\n down_hi_lo");
        digitalWrite(PIN_DOWN_OUT,HIGH);
        delay(1);
        digitalWrite(PIN_DOWN_OUT,LOW);
        delay(1);
    }
*/
  }




  /*

  if(digitalRead(PIN_UP_IN) == 1)
    Serial.print("up 1\n");         // testing IO pins
  else
    Serial.print("up 0\n");
  if(digitalRead(PIN_DOWN_IN) == 1)
    Serial.print("down 1\n");         // testing IO pins
  else
    Serial.print("down 0\n");
  */


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
