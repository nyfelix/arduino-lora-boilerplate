#include <Arduino.h>
#include "config.h"
#include "deviceConfig.h"
#include <TinyLoRa.h>
#include <SPI.h>
#include <Adafruit_SleepyDog.h>
#include <avr/pgmspace.h>
#include <CayenneLPP.h>
#include <FSM.h>
#include <Debug.h>

uint8_t NwkSkey[16] = NWKSKEY;  // TTN Network Session Key
uint8_t AppSkey[16] = APPSKEY;  // TTN Application Session Key
uint8_t DevAddr[4] = DEVADDR;   // TTN Device Adress

TinyLoRa lora {DIO1, NSS, RST}; // The LoRa Implmentation used in this Boilerplate
CayenneLPP payload(12); // Buffer for the LoRa payload.

const int C_SEND_INTERVAL {SEND_INTERVAL}; // duration of watchdochg sleeptime in s. Minimal sleepduration is 30s
const int C_OBSERVATION_INTERVAL {OBSERVATION_INTERVAL};
int sleepCounter {0};   // counts the how many times the watchdogSleep or simulateSleep reseted the counter.
const int numObservationsTillSend {C_SEND_INTERVAL/C_OBSERVATION_INTERVAL};

/* Define States and Statemachine */
void observe();
void send();
void sleep();

State stateSleeping (&sleep, NULL , NULL);
State stateObserving (&observe, NULL, NULL);
State stateSending (&send, NULL, NULL);
State stateFinished (NULL, NULL,  NULL);
Fsm fsm(&stateObserving);

/* States of our simple state machine*/
enum Event{
  WAKEUP,       // Imediate Wake up (e.g. on interupt)
  TIMED_WAKEUP, // Wake up after C_OBSERVATION_INTERVAL
  SLEEP,        // Sleep and try to save energy
  SEND,         // Send the payload out over LoRa Network aber a numeber of obervations (could also be triggerd by time)
  FINISH        // For debugging we want to stop the program and do nothing after sending out over LoRa (keep the device from beeing blocked)
};

void defineFSMTransitions () {
  fsm.add_transition(&stateObserving, &stateSleeping,  Event::SLEEP,  NULL);
  fsm.add_transition(&stateSleeping,  &stateObserving, Event::WAKEUP, NULL);
  fsm.add_transition(&stateObserving, &stateSending,   Event::SEND,   NULL);
  fsm.add_transition(&stateSending,   &stateSleeping,  Event::SLEEP,  NULL);
  fsm.add_transition(&stateSending,   &stateFinished,  Event::FINISH, NULL);
}

void observe() {
  // Process all sensors
  debugLn("Start observing...");
    /*payload.reset();
    float temperature = 25;
    payload.addTemperature(1, temperature);
    float humidity = 0.8;
    payload.addRelativeHumidity(2, humidity);
    float vbat = analogRead(VBATPIN);
    vbat *= 2;    // we divided by 2, so multiply back
    vbat *= REFVOL;  // Multiply by 3.3V, our reference voltage
    vbat /= 1024; // convert to voltage
    vbat /= 10;
    payload.addVoltage(3, vbat);*/
  debugLn("... end observing ");
  if (sleepCounter < numObservationsTillSend) {
    fsm.trigger(Event::SLEEP);
  } else {
    fsm.trigger(Event::SEND);
  }
}

void send() {
  digitalWrite(LED_BUILTIN, HIGH);
  debugLn("Sending LoRa Data...");
  debug("Frame Counter: "); 
  debugLn(lora.frameCounter);
  //lora.sendData(payload.getBuffer(), payload.getSize() , lora.frameCounter);  
  lora.frameCounter++;
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  fsm.trigger(Event::SLEEP);
}

/* This function is used to set the watchdog timer multipel times, so the sleep duration is longer than the max. value of the watchdogtimer.*/
#ifdef FEATHERM0
void sleep(){
  Watchdog.sleep(C_OBSERVATION_INTERVAL*1000); //sleeptime in ms  
  sleepCounter++;
  fsm.trigger(Event::WAKEUP);
}
#else
/* This function simulates sleep. But the mikrocontroller doesn't enter sleepmode, so the serial communication doesn't break down.*/
void sleep(){
  debugLn("Start sleeping...");
  delay(C_OBSERVATION_INTERVAL*1000);
  sleepCounter++;
  debugLn("...Wake up");
  fsm.trigger(Event::WAKEUP);
}
#endif

void setup() {  
  #ifdef DEBUG
    Serial.begin(9600);
    while (! Serial);
    debugLn("Serial started");
  #endif 
  debug("Starting LoRa...");
  lora.setChannel(MULTI);
  lora.setDatarate(DATARATE);
  if(!lora.begin())                                                                                                                                                                                                                                                                                                                                                                                                                                                                    {
    debugLn("Failed: Check your radio");
  }
  debugLn(" OK");
  defineFSMTransitions();
}

void loop() {
  fsm.run_machine();
}

