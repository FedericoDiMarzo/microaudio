

#ifndef STM32_MONOSYNTH_AUDIO_MODULE_H
#define STM32_MONOSYNTH_AUDIO_MODULE_H

#include "audio.h"
#include "audio_buffer.h"
#include "audio_processor.h"

/**
 * An AudioModule is an abstract template class that can
 * be subclassed to implement a module that writes and processes
 * an AudioBuffer.
 *
 * @tparam CHANNEL_NUM specifies if the AudioModule is mono, stereo or multichannel
 */
template<size_t CHANNEL_NUM>
class AudioModule {
public:

    /**
     * Constructor.
     */
    AudioModule(AudioProcessor &audioProcessor) : audioProcessor(audioProcessor) {};

    /**
     * Disabling default constructor.
     */
    AudioModule() = delete;

    /**
     * Precess an external AudioBuffer.
     *
     * An implementation of an AudioModule must implement this
     * virtual method.
     *
     * @param buffer AudioBuffer to be processed
     */
    virtual void process(AudioBuffer<float, CHANNEL_NUM, AUDIO_DRIVER_BUFFER_SIZE> &buffer) = 0;

    /**
     * Returns the sample rate of the AudioDriver used by the AudioProcessor.
     *
     * @return sample rate
     */
    inline float getSampleRate() { return audioProcessor.getSampleRate(); };

    /**
    * Disabling copy constructor.
    */
    AudioModule<CHANNEL_NUM>(const AudioModule<CHANNEL_NUM> &) = delete;

    /**
     * Disabling move operator.
     */
    AudioModule<CHANNEL_NUM> &operator=(AudioModule<CHANNEL_NUM> &) = delete;

private:
    /**
     * Reference to the AudioProcessor using this AudioModule.
     */
    AudioProcessor &audioProcessor;

};

#endif //STM32_MONOSYNTH_AUDIO_MODULE_H
