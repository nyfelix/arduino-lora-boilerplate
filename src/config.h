#pragma once

/* Debugging */
#define SERIAL_BEGIN // Enable serial output
#define DEBUG // Send Debug messages

/* System behavior */
//#define DEEPSLEEP // Use Deepsleep (if not, it sleep will be simulated)
#define SEND_INTERVAL 10   // Time in seconds
#define MEASURE_INTERVAL 2 // Time in seconds

/* LoRa Settings */
#define DATARATE SF12BW125


/* Hardware Configuration */
#ifdef FEATHERM0
  #define VBATPIN A7
  #define REFVOL 3.3
  // Pinout for Adafruit Feather M0 LoRa
  #define DIO1 3    // irq = dio1
  #define CS 8      // cs = nss
#else
  #define VBATPIN A7
  #define REFVOL 5
  // Pinout for Uno with Dragino Lora Shield v1.2
  #define DIO1 3    // irq = dio1
  #define CS 8      // cs = nss
#endif 
