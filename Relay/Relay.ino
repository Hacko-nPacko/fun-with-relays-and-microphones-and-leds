#include <Arduino.h>


#ifdef ARDUINO_ARCH_MEGAAVR
#include "EveryTimerB.h"
#define Timer1 TimerB2 // use TimerB2 as a drop in replacement for Timer1
#else // assume architecture supported by TimerOne ....
#include "TimerOne.h"
#endif

#include "LPD8806.h"


#define RELAY_1 2
#define RELAY_2 3

#define CLOCK_PIN 4
#define DATA_PIN 5

#define MICROPHONE 6

LPD8806 strip = LPD8806(12, DATA_PIN, CLOCK_PIN);


unsigned int iteration;
unsigned int relayIteration;
int previousClap;


void setup() {

  // setup vars
  iteration = 0;
  relayIteration = 0;
  previousClap = millis();

  // setup pins
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(MICROPHONE, INPUT);

  // Start up the LED strip
  strip.begin();
  strip.setPixelColor(0, 0, 127, 0); //A Arduino is ok
  strip.show();

  // the interrupt
  Timer1.initialize();
  Timer1.attachInterrupt(led_belt);
  Timer1.setPeriod(1000000); // like the TimerOne library this will start the timer as well

  Serial.begin(9600);

  set_relays();

}

void led_belt() {
  strip.setPixelColor((iteration-3) % 12, 0, 0, 0);
  strip.setPixelColor((iteration-2) % 12, 32, 32, 0);
  strip.setPixelColor((iteration-1) % 12, 0, 64, 0);
  strip.setPixelColor(iteration % 12, 0, 64, 64);
  strip.show();
  iteration++;
  digitalWrite(LED_BUILTIN, iteration % 2 == 0 ? HIGH : LOW);

}

// the loop function runs over and over again forever
void loop() {
  int val = digitalRead(MICROPHONE);
  if (val == HIGH) {
    int currentClap = millis();

    // it was toooo fast
    if (currentClap - previousClap < 50) {
      return;
    }

    
    if (currentClap - previousClap < 500) {
      Serial.println("PING");  
      // relays
      set_relays();
    }
    previousClap = currentClap;
  }
  
}

void set_relays() {
  delay(50);
  digitalWrite(RELAY_1, relayIteration % 2 == 0 ? HIGH : LOW);
  digitalWrite(RELAY_2, relayIteration % 2 == 1 ? HIGH : LOW);
  relayIteration++;
}
