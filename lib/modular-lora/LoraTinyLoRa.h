#ifndef LORA_TINYLORA_H
#define LORA_TINYLORA_H

#include "Lora.h"
#include <TinyLoRa.h>
#include "Debug.h"

class LoraTinyLoRa : public Lora {
    public:
     
        void begin(rfm_datarates_t datarate, rfm_channels_t channel) {
            debug("Starting LoRa...");
            lora.setChannel(channel);
            lora.setDatarate(datarate);
            if(!lora.begin())                                                                                                                                                                                                                                                                                                                                                                                                                                                                    {
                debugLn("Failed: Check your radio");
            }
            debugLn(" OK");
        }
        
        void send(uint8_t *buffer, uint8_t size) {
            lora.sendData(buffer, size, lora.frameCounter);  
            lora.frameCounter++;
        };

        int getFrameCounter() {
            return lora.frameCounter;
        }


    protected:
        TinyLoRa lora {DIO1, NSS, RST}; // The LoRa Implmentation used in this Boilerplate
};


#endif