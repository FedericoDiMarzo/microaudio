
#ifndef MIOSIX_AUDIO_AUDIO_CONFIG_H
#define MIOSIX_AUDIO_AUDIO_CONFIG_H

/**
 * This header is used to modify the audio
 * configuration of the system.
 */

/**
 * Sample rate of the DAC.
 */
#define AUDIO_DRIVER_SAMPLE_RATE 44100

/**
 * Size of the stereo DAC buffer.
 */
#define AUDIO_DRIVER_BUFFER_SIZE 256

/**
 * Bit depth of the DAC.
 */
#define AUDIO_DRIVER_BIT_DEPTH 16

/**
 * Max value for DAC conversion ((2 ^ (BIT_DEPTH-1)) - 1).
 * Positive integer upper bound of the DAC values.
 */
#define DAC_MAX_POSITIVE_VALUE 32767

#endif //MIOSIX_AUDIO_AUDIO_CONFIG_H
