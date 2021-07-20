
#ifndef MIOSIX_KERNEL_AUDIO_PROCESSABLE_H
#define MIOSIX_KERNEL_AUDIO_PROCESSABLE_H

/***
 * Interface used to establish a callback method
 * to be used with the AudioDriver in order
 * to write in the audio buffer.
 */
class AudioProcessable {
public:
    /**
     * This method is called by the AudioDriver during
     * a DMA complete transfer IRQ.
     */
    virtual void process() = 0;
};

/**
 * This dummy AudioProcessable class in used to initialize the
 * audio driver with an empty callback.
 */
class AudioProcessableDummy : public AudioProcessable {
public:
    inline void process() override {};
};

#endif //MIOSIX_KERNEL_AUDIO_PROCESSABLE_H
