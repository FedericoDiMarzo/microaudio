
#ifndef MIOSIX_AUDIO_AUDIO_BUFFER_H
#define MIOSIX_AUDIO_AUDIO_BUFFER_H

#include <array>
#include <algorithm>


/**
 * This template class define a multi channel buffer that can be used to
 * store and process audio.
 *
 * @tparam T type stored in the buffer // TODO: restrict T for numeric types
 * @tparam CHANNEL_NUM number of channels of the buffer
 * @tparam BUFFER_LEN length of each channel of the buffer
 */
template<typename T, size_t CHANNEL_NUM, size_t BUFFER_LEN>
class AudioBuffer {
public:
    /**
     * Constructor, statically asserts that the length of the
     * AudioBuffer is even.
     */
    AudioBuffer() {
        static_assert((BUFFER_LEN % 2) == 0, "The AudioBuffer BUFFER_LEN must be even");
        clear();
    };

    /**
     * Returns the channel count.
     *
     * @return number of channels of the buffer
     */
    inline size_t getNumChannels() const { return bufferContainer.size(); };

    /**
     * Returns the length of the buffers (that is the same for
     * each channel).
     *
     * @return buffer length
     */
    inline size_t getBufferLength() const { return bufferContainer[0].size(); };

    /**
     * Returns a raw pointer to the data array of a certain channel
     * of the buffer. The access is read only.
     *
     * @param channelNumber target channel
     * @return read pointer to the channel data
     */
    inline const T *
    getReadPointer(const unsigned int channelNumber) const { return bufferContainer[channelNumber].data(); };

    /**
     * Returns a raw pointer to the data array of a certain channel
     * of the buffer. The pointer grants both read and write accesses.
     *
     * @param channelNumber target channel
     * @return write pointer to the channel data
     */
    inline T *getWritePointer(const unsigned int channelNumber) { return bufferContainer[channelNumber].data(); };

    /**
     * Returns the std::array that contains the channels data.
     *
     * @return array containing arrays of data
     */
    inline std::array <std::array<T, BUFFER_LEN>, CHANNEL_NUM> &getBufferContainer() { return bufferContainer; };

    /**
     * Applies a constant gain to the AudioBuffer.
     *
     * @param gain multiplicative factor
     */
    void applyGain(float gain);

    /**
     * Sums a second AudioBuffer to this AudioBuffer.
     *
     * @param buffer AudioBuffer to sum to this instance
     */
    void add(const AudioBuffer<T, CHANNEL_NUM, BUFFER_LEN> &buffer);

    /**
     * Multiplies a second AudioBuffer to this AudioBuffer.
     *
     * @param buffer AudioBuffer to sum to this instance
     */
    void multiply(const AudioBuffer<T, CHANNEL_NUM, BUFFER_LEN> &buffer);

    /**
     * Performs a copy from another buffer of the same dimensions.
     *
     * @param audioBuffer buffer to copy from
     */
    void copyFrom(const AudioBuffer<T, CHANNEL_NUM, BUFFER_LEN> &audioBuffer);

    /**
     * Copy from a mono buffer on a certain channel.
     *
     * @param audioBuffer target buffer to copy
     * @param channelNumber destination channel
     */
    void copyOnChannel(const AudioBuffer<T, 1, BUFFER_LEN> &audioBuffer, size_t channelNumber);

    /**
     * Clear the buffer by filling it with zeroes
     */
    void clear();

private:
    /**
     * Data structure containing the buffer data.
     */
    std::array <std::array<T, BUFFER_LEN>, CHANNEL_NUM> bufferContainer;

    /**
     * Disabling copy constructor.
     */
    AudioBuffer<T, CHANNEL_NUM, BUFFER_LEN>(const AudioBuffer<T, CHANNEL_NUM, BUFFER_LEN> &);

    /**
     * Disabling move operator.
     */
    AudioBuffer<T, CHANNEL_NUM, BUFFER_LEN> &operator=(const AudioBuffer<T, CHANNEL_NUM, BUFFER_LEN> &);
};


template<typename T, size_t CHANNEL_NUM, size_t BUFFER_LEN>
void AudioBuffer<T, CHANNEL_NUM, BUFFER_LEN>::applyGain(float gain) {
    for (uint32_t channelNumber = 0; channelNumber < CHANNEL_NUM; channelNumber++) {
        // iterating for each channel
        T *channel = getWritePointer(channelNumber);
        for (uint32_t i = 0; i < BUFFER_LEN; i++) {
            // applying the gain to each sample of the channel
            channel[i] *= gain;
        }
    }
}

template<typename T, size_t CHANNEL_NUM, size_t BUFFER_LEN>
void AudioBuffer<T, CHANNEL_NUM, BUFFER_LEN>::add(const AudioBuffer<T, CHANNEL_NUM, BUFFER_LEN> &buffer) {
    for (uint32_t channelNumber = 0; channelNumber < CHANNEL_NUM; channelNumber++) {
        // iterating for each channelBuffer1
        T *channelBuffer1 = getWritePointer(channelNumber);
        const T *channelBuffer2 = buffer.getReadPointer(channelNumber);
        for (uint32_t sampleIndex = 0; sampleIndex < BUFFER_LEN; sampleIndex++) {
            // summing AudioBuffer2 on AudioBuffer1
            channelBuffer1[sampleIndex] += channelBuffer2[sampleIndex];
        }
    }
}

template<typename T, size_t CHANNEL_NUM, size_t BUFFER_LEN>
void AudioBuffer<T, CHANNEL_NUM, BUFFER_LEN>::multiply(const AudioBuffer<T, CHANNEL_NUM, BUFFER_LEN> &buffer) {
    for (uint32_t channelNumber = 0; channelNumber < CHANNEL_NUM; channelNumber++) {
        // iterating for each channelBuffer1
        T *channelBuffer1 = getWritePointer(channelNumber);
        const T *channelBuffer2 = buffer.getReadPointer(channelNumber);
        for (uint32_t sampleIndex = 0; sampleIndex < BUFFER_LEN; sampleIndex++) {
            // summing AudioBuffer2 on AudioBuffer1
            channelBuffer1[sampleIndex] *= channelBuffer2[sampleIndex];
        }
    }
}

template<typename T, size_t CHANNEL_NUM, size_t BUFFER_LEN>
void AudioBuffer<T, CHANNEL_NUM, BUFFER_LEN>::copyOnChannel(const AudioBuffer<T, 1, BUFFER_LEN> &audioBuffer,
                                                            size_t channelNumber) {

    T *buffer1 = getWritePointer(channelNumber);
    const T *buffer2 = audioBuffer.getReadPointer(0);
    std::copy(buffer2, buffer2 + BUFFER_LEN, buffer1);
}

template<typename T, size_t CHANNEL_NUM, size_t BUFFER_LEN>
void AudioBuffer<T, CHANNEL_NUM, BUFFER_LEN>::copyFrom(const AudioBuffer<T, CHANNEL_NUM, BUFFER_LEN> &audioBuffer) {
    T *buffer1;
    const T *buffer2;
    for (uint32_t channelNumber = 0; channelNumber < CHANNEL_NUM; channelNumber++) {
        buffer1 = getWritePointer(channelNumber);
        buffer2 = audioBuffer.getReadPointer(channelNumber);
        std::copy(buffer2, buffer2 + BUFFER_LEN, buffer1);
    }
}

template<typename T, size_t CHANNEL_NUM, size_t BUFFER_LEN>
void AudioBuffer<T, CHANNEL_NUM, BUFFER_LEN>::clear() {
    for (uint32_t channelNumber = 0; channelNumber < CHANNEL_NUM; channelNumber++) {
        bufferContainer[channelNumber].fill(0); // TODO: manage templating generalization
    }
}

#endif //MIOSIX_AUDIO_AUDIO_BUFFER_H
