

#ifndef STM32_MONOSYNTH_AUDIO_PARAMETER_H
#define STM32_MONOSYNTH_AUDIO_PARAMETER_H

#include "audio_math.h"

/**
 * Default number of samples to pass
 * from lastValue to currentValue
 */
#define AUDIO_PARAMETER_DEFAULT_TRANSITION_SAMPLES 100

/**
 * This template class can be used as a wrapper for a parameter of
 * an AudioModule or similar classes.
 * It allows to store the current and last values for a parameter in order
 * to provide a smooth interpolation during the processing.
 *
 * @tparam T type
 */
template<typename T>
class AudioParameter {
public:

    /**
     * Constructor with an initializer for the parameter value.
     *
     * @param value
     */
    AudioParameter(T value) : totalTransitionSamples(AUDIO_PARAMETER_DEFAULT_TRANSITION_SAMPLES),
                              passedTransitionSamples(0),
                              currentValue(value),
                              lastValue(value) {};

    /**
     * Getter for currentValue.
     *
     * @return currentValue
     */
    inline T getValue() const { return currentValue; };

    /**
     * Getter for lastValue.
     *
     * @return
     */
    inline T getLastValue() const { return lastValue; };

    inline T getInterpolatedValue() const {
        return AudioMath::linearInterpolation(lastValue, currentValue, getTransitionIndex());
    };

    /**
     * Returns a value between 0.0 and 1.0 that indicates
     * the progress of the transition.
     *
     * @return float index
     */
    inline float getTransitionIndex() const {
        return static_cast<float>(passedTransitionSamples) / static_cast<float>(totalTransitionSamples);
    }

    /**
     * Sets the current value, saving the previous currentValue
     * into lastValue and resetting the passedTransitionSamples,
     * in order to indicate a new transition.
     *
     * @param newValue
     */
    inline void setValue(T newValue) {
        lastValue = getInterpolatedValue();
        currentValue = newValue;
        passedTransitionSamples = 0;
    };

    /**
     * Increases the count of the passedTransitionSamples.
     *
     * @param sampleNumber number of samples of the increase
     */
    inline void updateSampleCount(size_t sampleNumber = 1) {
        passedTransitionSamples += sampleNumber;
        if (passedTransitionSamples > totalTransitionSamples) {
            passedTransitionSamples = totalTransitionSamples;
        }
    }

    /**
     * Setter for totalTransitionSamples. A greater value indicates
     * a longer transition.
     *
     * @param sampleNumber number of total samples of the transition
     */
    inline void setTransitionSamples(size_t sampleNumber) { totalTransitionSamples = sampleNumber; }

    /**
     * Sets the transition time in seconds.
     *
     * @param time interval of the transition in seconds
     * @param sampleRate sample frequency
     */
    inline void setTransitionTime(float time, float sampleRate) {
        totalTransitionSamples = static_cast<size_t>(time * sampleRate);
    }

    /**
     * Indicates if the transition is complete.
     *
     * @return boolean flag
     */
    inline bool transitionIsComplete() const {
        return passedTransitionSamples >= totalTransitionSamples;
    }

private:
    /**
     * This value indicates the total samples needed
     * to pass from lastValue to currentValue.
     */
    size_t totalTransitionSamples;

    /**
     * This value indicates the already passed samples
     * from lastValue.
     */
    size_t passedTransitionSamples;

    /**
     * Current value of the AudioParameter.
     */
    T currentValue;

    /**
     * Value of the AudioParameter from the last call
     * of setValue().
     */
    T lastValue;
};

#endif //STM32_MONOSYNTH_AUDIO_PARAMETER_H
