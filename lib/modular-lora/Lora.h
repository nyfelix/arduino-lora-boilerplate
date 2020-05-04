#ifndef LORA_H
#define LORA_H

class Lora {
    public:
        virtual void send(uint8_t *buffer, uint8_t size);
        virtual int getFrameCounter();

    protected:

};


#endif