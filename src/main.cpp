#pragma once
#define EU863

#include <Arduino.h>
#include <Debug.h>
#include <TinyLoRa.h>
#include <SPI.h>
#include <lmic_util.h>
#include <Adafruit_SleepyDog.h>
#include <avr/pgmspace.h>
#include <config.h>
#include <deviceConfig.h>

//******************Configuration of TinyLoRa********************
uint8_t NwkSkey[16] = NWKSKEY;
uint8_t AppSkey[16] = APPSKEY;
uint8_t DevAddr[4] = DEVADDR;

TinyLoRa lora = TinyLoRa(DIO1, NSS, RST);

// Buffer for the LoRa payload.
unsigned char payload[6];

int sendInterval = SEND_INTERVAL; // duration of watchdochg sleeptime in s. Minimal sleepduration is 30s
int measureInverval = MEASURE_INTERVAL;
int sleepcounter = 0; // counts the how many times the watchdogSleep or simulateSleep reseted the counter.
volatile bool sleepbit=false; //first loop without sleeping

/*
  Converts a float variable to a 2 byte int and ads it to the payload buffer. The Value needs to be decoded in the TTN decoder. 
*/
void floatToPayload(uint8_t i, float value ) {
  // float -> int
  uint16_t payloadValue = LMIC_f2sflt16(value);
  // int -> bytes
  byte low = lowByte(payloadValue);
  byte high = highByte(payloadValue);
  // place the bytes into the payload
  payload[i] = low;
  payload[i+1] = high;
}

void intToPayLoad(uint8_t i, int value) {
  byte low = lowByte(value);
  byte high = highByte(value);
  // place the bytes into the payload
  payload[i] = low;
  payload[i+1] = high;
}

/* Prints the payload. Used for debugging. */
void print_payload(){
  debug("Payload: ");
  for(unsigned int i=0; i<sizeof(payload);i++){
    debug(payload[i], HEX);
  debug(" ");
  }
  debugLn();
}

/* Encode the data to send to the byte array payload. */
void  preparePayolad() {
  float temperature = 25;
  intToPayLoad(0, temperature);
  float humidity = 0.8;
  floatToPayload(2, humidity);
  // only works on Feather boards
  float vbat = analogRead(VBATPIN);
  vbat *= 2;    // we divided by 2, so multiply back
  vbat *= REFVOL;  // Multiply by 3.3V, our reference voltage
  vbat /= 1024; // convert to voltage
  vbat /= 10;
  floatToPayload(4, vbat);
}

/* Prepare a simple state machine*/
enum States{
  observing,
  sending
};

States currState = observing;

/* This function is used to set the watchdog timer multipel times, so the sleep duration is longer than the max. value of the watchdogtimer.*/
void watchdogSleep(int time_s, volatile bool*sleepflag){
  
  double sleep_rep=time_s/30;
  while (*sleepflag==true){
    Watchdog.sleep(30000);//sleeptime in ms  
    sleepcounter++;
    if(sleepcounter>=sleep_rep){
      *sleepflag=false;    
      sleepcounter=0; // reset the sleepcounter
    }
  }
  *sleepflag=true;// reset sleepbit
}

/* This function simulates sleep. But the mikrocontroller doesn't enter sleepmode, so the serial communication doesn't break down.*/
void simulateSleep(int time_s, volatile bool*sleepflag){
  // ToDo: Simulate Interrupt
  delay(time_s*1000);
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
  switch (currState) { 
    
    case observing:{ // u gathering and processing information with sleep pauses
      debugLn("observing...");
      
      digitalWrite(LED_BUILTIN,LOW);
      #ifndef DEEPSLEEP
        simulateSleep(sendInterval, &sleepbit);
      #endif
      #ifdef DEEPSLEEP 
        watchdogSleep(measureInverval, &sleepbit);
      #endif
      digitalWrite(LED_BUILTIN,HIGH);
      debugLn("end of sleep");
      // Read sensor data.,
      
      currState=sending;
      break; 
    }
      
    case sending:{ 
      digitalWrite(LED_BUILTIN, HIGH);
      debugLn("Sending LoRa Data...");
      preparePayolad();
      #ifdef DEBUG
        print_payload();
      #endif
      debug("Frame Counter: "); 
      debugLn(lora.frameCounter);
      lora.sendData(payload, sizeof(payload), lora.frameCounter);  
      lora.frameCounter++;
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
      currState=observing;
      break;
    } 
  }       
}

