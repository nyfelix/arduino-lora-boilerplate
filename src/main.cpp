/* 
    A boilerplate for typical LoRa applications, built with libraries that work fine for me.
    This code is a working example that creates some random sensor values.
*/
#include <Arduino.h>
#include "config.h"
#include "deviceConfig.h"
#include "LoraTinyLoRa.h"       // TinyLora Wrapper, could be replaced by other LoRa implementation
#include <CayenneLPP.h>
#include <FSM.h>
#include "RandomDataSensor.h"
#include "VoltageSensor.h"
#include "Debug.h"

#ifdef FEATHERM0
  #include <Adafruit_SleepyDog.h>
#endif

// Alternatively choose other LoRa Implementation from the modular-lora framework
// Global Variables required for TinyLoRa ABP
uint8_t NwkSkey[16] = NWKSKEY;  // TTN Network Session Key
uint8_t AppSkey[16] = APPSKEY;  // TTN Application Session Key
uint8_t DevAddr[4] = DEVADDR;   // TTN Device Adress
LoraTinyLoRa lora;

// Any other playoad format coudl be use, currently we like CayenneLPP
CayenneLPP payload(22);         // Buffer for the LoRa payload.

const int C_SEND_INTERVAL {SEND_INTERVAL}; // Interval for sending the data to the cloud in seconds
const int C_OBSERVATION_INTERVAL {OBSERVATION_INTERVAL}; // Interval for observing the sensors
const int numObservationsTillSend {C_SEND_INTERVAL/C_OBSERVATION_INTERVAL}; // Number of sensor intervals before sending out the data
int sleepCounter {0};   // counts the how many times the watchdogSleep or simulateSleep was called

/* Define Statemachine: States, Functions, Events, Transiitions */
/* Function prototypes */
void observe();
void send();
void sleep();

/* States
                     Enter  State     Exit  */
State stateSleeping  (NULL,  &sleep ,  NULL);
State stateObserving (NULL,  &observe, NULL);
State stateSending   (NULL,  &send,    NULL);
State stateFinished  (NULL,  NULL,     NULL);
Fsm fsm(&stateSleeping);

/* Events */
enum Event{
  WAKEUP,       // Imediate Wake up (e.g. on interupt)
  SLEEP,        // Sleep and try to save energy
  SEND,         // Send the payload out over LoRa Network aber a numeber of obervations (could also be triggerd by time)
  FINISH        // For debugging we want to stop the program and do nothing after sending out over LoRa (keep the device from beeing blocked)
};

/* Transitions */
void defineFSMTransitions () {
  fsm.add_transition(&stateSleeping,  &stateObserving, Event::WAKEUP, NULL);
  fsm.add_transition(&stateObserving, &stateSleeping,  Event::SLEEP,  NULL);
  fsm.add_transition(&stateObserving, &stateSending,   Event::SEND,   NULL);
  fsm.add_transition(&stateSending,   &stateSleeping,  Event::SLEEP,  NULL);
  fsm.add_transition(&stateSending,   &stateFinished,  Event::FINISH, NULL);
}

/* end of define state machine */

/* Declare Sensors  */
RandomDataSensor temperature {10,30};
RandomDataSensor humiditiy {40,80};
VoltageSensor batteryVoltage {VBATPIN, REFVOL};

/* observe all sensors and process their data accoring to their sensor class implementation */
void observe() {
  debug("Start observing...");
  temperature.measure();
  humiditiy.measure();
  batteryVoltage.measure();
  debugLn(" end ");
  if (sleepCounter < numObservationsTillSend) {
    fsm.trigger(Event::SLEEP);
  } else {
    fsm.trigger(Event::SEND);
  }
}
/* Sleep implementation accoring to the platform */
/* This function is used to set the watchdog timer to sleep between the observation invervals.*/

#if defined(FEATHERM0) && !defined(DEBUG)
void sleep(){
  Watchdog.reset();
  Watchdog.sleep(C_OBSERVATION_INTERVAL*1000); //sleeptime in ms  
  sleepCounter++;
  fsm.trigger(Event::WAKEUP);
}
#else
/* This function simulates sleep. But the mikrocontroller doesn't enter sleepmode, so the serial communication doesn't break down.*/
void sleep(){
  debug("Start sleeping... ");
  delay(C_OBSERVATION_INTERVAL*1000);
  sleepCounter++;
  debugLn("wake up");
  fsm.trigger(Event::WAKEUP);
}
#endif

/* Prepare the payload and send over LoRa */
void send() {
  payload.reset();
  payload.addTemperature(1, temperature.getValue().avg);
  payload.addTemperature(2, temperature.getValue().min);
  payload.addTemperature(3, temperature.getValue().max);
  payload.addRelativeHumidity(4, humiditiy.getValue().avg/100);
  payload.addVoltage(5, batteryVoltage.getValue().avg);

  digitalWrite(LED_BUILTIN, HIGH);
  debugLn("Sending LoRa Data...");
  lora.send(payload.getBuffer(), payload.getSize());
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);

  temperature.reset();
  humiditiy.reset();
  batteryVoltage.reset();
  sleepCounter = 0;
  #ifdef DEBUG // For debuging only send once to prevent the device from beeing blocked by TTN
    if (lora.getFrameCounter() > 1) {
      fsm.trigger(Event::FINISH);
    } else {
      fsm.trigger(Event::SLEEP);
    }
  #else
    fsm.trigger(Event::SLEEP);
  #endif
}

void setup() {  
  #ifdef DEBUG
    Serial.begin(9600);
    while (! Serial);
    debugLn("Serial started");
  #endif 
  lora.begin(DATARATE, MULTI);
  defineFSMTransitions();
}

void loop() {
  fsm.run_machine();
}