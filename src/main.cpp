#pragma once

#include <Arduino.h>
#include <Debug.h>
#include <config.h>
#include <deviceConfig.h>
#include <TinyLoRa.h>
#include <SPI.h>
#include <lmic_util.h>
#include <Adafruit_SleepyDog.h>
#include <avr/pgmspace.h>
#include <CayenneLPP.h>

//******************Configuration of TinyLoRa********************
uint8_t NwkSkey[16] = NWKSKEY;
uint8_t AppSkey[16] = APPSKEY;
uint8_t DevAddr[4] = DEVADDR;

TinyLoRa lora {DIO1, NSS, RST}; // The LoRa Implmentation used in this Boilerplate
CayenneLPP payload(12); // Buffer for the LoRa payload.

int sendInterval {SEND_INTERVAL}; // duration of watchdochg sleeptime in s. Minimal sleepduration is 30s
int measureInverval {MEASURE_INTERVAL};
int sleepCounter {0};   // counts the how many times the watchdogSleep or simulateSleep reseted the counter.
int sendCounter {sendInterval/measureInverval};
volatile bool sleepbit=false; //first loop without sleeping

/* Prepare a simple state machine*/
enum State{
  OBSERVING,
  SENDING,
  FINISHED
};


State state {OBSERVING};

/* Encode the data to send to the byte array payload. */
void  preparePayolad() {
  payload.reset();
  float temperature = 25;
  payload.addTemperature(1,temperature);
  float humidity = 0.8;
  payload.addRelativeHumidity(2, humidity);
  // only works on Feather boards
  float vbat = analogRead(VBATPIN);
  vbat *= 2;    // we divided by 2, so multiply back
  vbat *= REFVOL;  // Multiply by 3.3V, our reference voltage
  vbat /= 1024; // convert to voltage
  vbat /= 10;
  payload.addVoltage(3, vbat);
}




/* This function is used to set the watchdog timer multipel times, so the sleep duration is longer than the max. value of the watchdogtimer.*/
void watchdogSleep(int time_s){
  sleepbit = true;
  Watchdog.sleep(time_s*1000);//sleeptime in ms  
  sleepCounter++;
  sleepbit = false;
}

/* This function simulates sleep. But the mikrocontroller doesn't enter sleepmode, so the serial communication doesn't break down.*/
void simulateSleep(int time_s){
  // ToDo: Simulate Interrupt
  sleepbit = true;
  delay(time_s*1000);
  sleepCounter++;
  sleepbit = false;
}

void measure() {
  
}

void setup(){  
  
  #ifdef SERIAL_BEGIN
    Serial.begin(9600);
    while (! Serial);
    debugLn("Serial started");
  #endif 

  debug("Starting LoRa...");
  lora.setChannel(MULTI);
  lora.setDatarate(DATARATE);
  if(!lora.begin())
  {
    debugLn("Failed: Check your radio");
  }
  debugLn(" OK");

}

/** Statemachine*/
void loop()
{
  switch (state) { 
    
    case State::OBSERVING:{ // u gathering and processing information with sleep pauses
      debugLn("measure...");
      measure();
      debugLn("sleep");
      #ifndef DEEPSLEEP
        simulateSleep(sendInterval);
      #else 
        watchdogSleep(measureInverval);
      #endif
      debugLn("end of sleep");
      // Read sensor data.,
      if (sleepCounter >= sendCounter) {
        state = SENDING;
      }
      break; 
    }
      
    case State::SENDING:{ 
      digitalWrite(LED_BUILTIN, HIGH);
      debugLn("Sending LoRa Data...");
      debug("Frame Counter: "); 
      debugLn(lora.frameCounter);
      preparePayolad();
      //lora.sendData(payload.getBuffer(), payload.getSize() , lora.frameCounter);  
      lora.frameCounter++;
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
      state = OBSERVING;
      //state = FINISHED;
      break;
    } 
    case State::FINISHED:{ 
      break;
    }       
}

