#pragma once

/* Debugging */
#define DEBUG // Send Debug messages

/* System behavior */
//#define DEEPSLEEP // Use Deepsleep (if not, it sleep will be simulated)
#define SEND_INTERVAL 30   // Time in seconds
#define OBSERVATION_INTERVAL 2 // Time in seconds

/* LoRa Settings */
//#define DATARATE SF12BW125
#define DATARATE SF7BW125
#define EU863

/* Hardware Configuration */
#ifdef FEATHERM0
  #define VBATPIN A7
  #define REFVOL 3.3
  // Pinout for Adafruit Feather M0 LoRa
  #define DIO1 3     // irq = dio1
  #define NSS 8      // cs = nss
  #define RST 4
#else
  #define VBATPIN A7
  #define REFVOL 5
  // Pinout for Uno with Dragino Lora Shield v1.2
  #define DIO1 2    // irq = dio1
  #define NSS 10      // cs = nss
  #define RST 9
#endif 
