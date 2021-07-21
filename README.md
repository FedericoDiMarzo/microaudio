# Microaudio
Microaudio is a framework developed in C++ that offers different tools to facilitate the development of real-time audio applications in the embedded field. It contains several classes and utilities, exposed as header files that can be imported into your projects as needed.

Having no external dependency beyond the standard C ++ library, microaudio can be easily integrated into any small or large project, being agnostic to building systems and thus just ready to be used in many different scenarios.

## Philosophy
The goal of this framework is to allow its users to reduce their coding time by providing the necessary tools to focus their attention on what should really matter: audio software development. Indeed, the implementation of real time software for audio can hide numerous pitfalls, that can easily be avoided by using suitable design patterns, proposed and tested within microaudio.

Using this code base, you can take advantage of some guidelines to follow, exposed by the classes of the framework itself, which will help you develop embedded audio code more easily, without having to reinvent the wheel every time.

## First steps
Before configuring microaudio with your chosen embedded environment, you need to familiarize with the main components of the framework.

### Audio Buffer
To manage the audio stream in real time, it is necessary to manage the data in contiguous sections with an appropriate data structure. The ```AudioBuffer``` template class, contained within ```audio_buffer.h``` solves the problem by offering a container of numeric data of fixed length, which can be structured as a single channel, or as multiple channels modifying the template parameters.



```c++
#include "microaudio/include/audio_buffer.h"

// a stereo buffer of 16 bit integers of size 128
AudioBuffer<uint16_t, 2, 128> buffer1;

// a mono buffer of floats of size 256
AudioBuffer<float, 1, 256> buffer2;

// a multi channel buffer of doubles of size 64
AudioBuffer<double, 4, 64> buffer3;
```

The AudioBuffer is used extensively throughout the framework. For performance reasons, it is stuctured to directly access the arrays of the individual channels for reading and writing to the buffer. 

```c++
// creating a float mono buffer
AudioBuffer<int, 1, 64> buffer;

// this pointer can be used to write into the buffer of the channel 0;
// for a multichannel buffer, the argument of the method is used to return
// the respective array
int* writePointer = buffer.getWritePointer(0);

// writing into the buffer
for (int i = 0; i < buffer.getBufferLength(); i++) {
	writePointer[i] = i;
}

// this array is read only
const int* readPointer = buffer.getReadPointer(0);
```

In order to simplify some common audio processing tasks, some utility methods are provided.

```c++
buffer.applyGain(2); // multiplication with a constant
buffer.add(otherBuffer); // element wise sum with a compatible buffer
buffer.multiply(otherBuffer); // element wise multiplication with a compatible buffer
buffer.copyFrom(otherBuffer); // copies a compatible buffer
buffer.copyOnChannel(monoBuffer, 1); // copies a compatible mono buffer on the right channel
buffer.clear(); // resets the buffer to all zeroes
```

### Audio Driver
The main component that allows the integration with the embedded environment is called ```AudioDriver```. The microaudio framework is designed to be adapted to multiple scenarios and does not implement any particular driver for a given architecture. Therefore it is up to the user to implement the abstract class ```AudioDriver``` present in ```audio_driver.h```, following the guidelines explained in a later section.

From a high level perspective, an ```AudioDriver``` exposes to the user and to the framework itself a series of structural functions, ensuring the correct functioning of the developed audio application. It can be stated that this component is at an intermediate level between the hardware on which it is being developed, and the audio software components to be implemented.

### Audio Processor
The abstract class that implements the actual audio application is called ```AudioProcessor``` (found inside ```audio_processor.h```) , which in turn uses the ```AudioProcessable``` interface (located in ```audio_processable.h```). The processor is directly connected to the ```AudioDriver```, and exposes the ```process()``` method, inherited from ```AudioProcessable```.

During execution, the ```AudioDriver``` will call the ```process()``` method of the ```AudioProcessor```, where the processor will process the ```AudioBuffer``` contained in the driver. After the processing, the driver will need to redirect the output buffer, using the DAC, or any output device used in the development environment.

## Driver Configuration
In order to configure microaudio with your embedded environment, you need to implement the abstract class ```AudioDriver```. This section proposes some guidelines to follow to successfully complete the implementation.

Before implementing the driver, it is necessary to modify the configuration header ```audio_config.h```.

```c++
// example configuration
#define AUDIO_DRIVER_SAMPLE_RATE 44100
#define AUDIO_DRIVER_BUFFER_SIZE 256
#define AUDIO_DRIVER_BIT_DEPTH 16
```

These constants must be used during the development of the driver implementation, that will allow doing  future changes in a simpler and more elegant way.

The ```constructor``` must set the sampleRate and bufferSize attributes, using the previously configured values inside ```audio_config.h```.

The ```init``` method must initialize all the devices necessary for the  audio rendering. This method is used to configure any timer, DACs, DMAs peripherals needed for the driver to work correctly. 

The ```start``` method implements the driver's operating logic. It must be called at runtime after the ```init``` method and after having associated a processor with the ```setAudioProcessable``` method.
It must be implemented as a blocking method, containing an infinite loop which, using the associated processor, processes the internal ```audioBuffer```, and then outputs it at audio frequency. 

Given the demand for real time, it is recommended to use a DMA peripheral instead of an interrupts logic where available, since in this way it will be possible to make a better use of the cpu for audio processing, rather than using it for moving the buffer content into the output peripheral.



