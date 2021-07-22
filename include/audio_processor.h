
#ifndef MIOSIX_KERNEL_AUDIO_PROCESSOR_H
#define MIOSIX_KERNEL_AUDIO_PROCESSOR_H

#include "audio_processable.h"
#include "audio_buffer.h"


/**
 * Base abstract class that can be extended to implement a
 * processor for audio.
 */
class AudioProcessor : public AudioProcessable {
public:

    /**
     * Constructor.
     *
     * @param audioDriver the driver to which the processor is connected
     */
    AudioProcessor(AudioDriver &audioDriver) : audioDriver(audioDriver) {};

    /**
     * Method called at audio rate by the AudioDriver, to process the AudioBuffer
     * sent to the output.
     */
    virtual void process() = 0;

    /**
     * Gets the output buffer from the AudioDriver.
     *
     * @return output stereo buffer
     */
    inline AudioBuffer<float, 2, AUDIO_DRIVER_BUFFER_SIZE> &getBuffer() const { return audioDriver.getBuffer(); };

    /**
     * Gets the length of the the output buffer.
     *
     * @return output buffer length
     */
    inline unsigned int getBufferSize() const { return audioDriver.getBufferSize(); };

    /**
     * Get the sample rate of the AudioDriver.
     *
     * @return sample rate
     */
    inline float getSampleRate() const { return audioDriver.getSampleRate(); };

    /**
     * Destructor.
     */
    AudioProcessor() = delete;

private:
    /**
     * Reference to the AudioDriver.
     */
    AudioDriver &audioDriver;
};



#endif //MIOSIX_KERNEL_AUDIO_PROCESSOR_H
