
#ifndef MIOSIX_KERNEL_AUDIO_PROCESSOR_H
#define MIOSIX_KERNEL_AUDIO_PROCESSOR_H

#include "drivers/common/audio.h"
#include "audio_processable.h"
#include "audio_buffer.h"

/**
 * Base abstract class that can be extended to implement a
 * processor for audio.
 */
class AudioProcessor : public AudioProcessable {
public:
    // TODO: comments

    AudioProcessor(AudioDriver &audioDriver) : audioDriver(audioDriver) {};

    virtual void process() = 0;

    inline AudioBuffer<float, 2, AUDIO_DRIVER_BUFFER_SIZE> &getBuffer() const { return audioDriver.getBuffer(); };

    inline unsigned int getBufferSize() const { return audioDriver.getBufferSize(); };

    inline float getSampleRate() const { return audioDriver.getSampleRate(); };

    AudioProcessor() = delete;

private:
    /**
     * Reference to the AudioDriver.
     */
    AudioDriver &audioDriver;
};

#endif //MIOSIX_KERNEL_AUDIO_PROCESSOR_H
