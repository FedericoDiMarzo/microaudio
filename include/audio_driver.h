
#ifndef MIOSIX_AUDIO_DRIVER_AUDIO_H
#define MIOSIX_AUDIO_DRIVER_AUDIO_H

#include "audio_config.h"
#include "audio_processable.h"
#include "audio_buffer.h"

// TODO: make the output buffer working with a custom type (int, double, float...)

/**
 * This singleton class offers an interface to the low level audio
 * functionalities of the system.
 * It allows to set an AudioProcessable object to be called as a callback
 * to handle the audio processing.
 *
 * Inherit from this class to implement the actual driver for your system.
 */
class AudioDriver {
public:

    /**
     * Constructor.
     */
    AudioDriver() {};

    /**
     * Initializes the audio driver.
     */
    void init() {};

    /**
     * Blocking call that starts the audio driver and
     * begins the audio processing.
     * In normal execution cases it never returns.
     */
    void start() {};

    /**
     * Getter for audioProcessable.
     *
     * @return audioProcessable
     */
    inline AudioProcessable &getAudioProcessable() {
        return *audioProcessable;
    }

    /**
     * Set the audio processable, this method must be called
     * after the init method.
     *
     */
    inline void setAudioProcessable(AudioProcessable &newAudioProcessable) {
        audioProcessable = &newAudioProcessable;
    }

    /**
     * Getter method for AudioBuffer.
     *
     * @return AudioBuffer
     */
    AudioBuffer<float, 2, AUDIO_DRIVER_BUFFER_SIZE> &getBuffer() { return audioBuffer; };

    /**
     * Getter method for the bufferSize.
     *
     * @return bufferSize
     */
    inline unsigned int getBufferSize() const { return bufferSize; };

    /**
     * Getter method for the sampleRate.
     *
     * @return sampleRate
     */
    inline float getSampleRate() const { return sampleRate; };

    /**
     * Sets the volume in a value between 0 and 1.
     * This value is mapped to the full decibel range of
     * the DAC.
     *
     * @param newVolume new volume value between 0 and 1
     */
    void setVolume(float newVolume) {};

    /**
     * Getter for the volume of the DAC.
     *
     * @return a value between 0 and 1
     */
    inline float getVolume() { return volume; };

    /**
     * Destructor.
     */
    ~AudioDriver() {};

    /**
     * The copy constructor is disabled.
     */
    AudioDriver(const AudioDriver &) = delete;

    /**
     * The move operator is disabled.
     */
    AudioDriver &operator=(const AudioDriver &) = delete;

private:

    /**
     * Size of the buffer.
     */
    unsigned int bufferSize;

    /**
     * Instance of an AudioProcessable used as a callback
     * to process the buffer.
     */
    AudioProcessable *audioProcessable;

    /**
     * Sample rate of the DAC conversion in float.
     */
    float sampleRate;

    /**
     * This stereo buffer is used by the audioProcessable for
     * the sound processing. the values inside the buffer must
     * be bounded in the interval [-1.0, 1.0].
     */
    AudioBuffer<float, 2, AUDIO_DRIVER_BUFFER_SIZE> audioBuffer;

    /**
     * Volume value of the audio driver.
     */
    float volume;

protected:

    /**
     * Setup of the sample rate from SampleRate enum class
     */
    void setSampleRate(uint32_t newSampleRate) {};

    /**
     * Utility method to copy current float buffers to the DAC integer output buffer
     */
    void writeToOutputBuffer(int16_t *writableRawBuffer) {};

};


#endif //MIOSIX_AUDIO_DRIVER_AUDIO_H
