# Microaudio
Microaudio is a framework developed in C++ that offers different tools to facilitate the development of real-time audio applications in the embedded field. It contains several classes and utilities, exposed as header files that can be imported into your projects as needed.

Having no external dependency beyond the standard C ++ library, microaudio can be easily integrated into any small or large projects, being agnostic to building systems and thus just ready to be used in many different scenarios.

## Philosophy
The goal of this framework is to allow its users to reduce their coding time by providing the necessary tools to focus their attention on what should really matter: audio software development. Indeed, the implementation of real time software for audio can hide numerous pitfalls, that can easily be avoided by using suitable design patterns, proposed and tested within microaudio.

Using this code base, you can take advantage of some guidelines to follow, exposed by the classes of the framework itself, which will help you develop embedded audio code more easily, without having to reinvent the wheel every time.

## First steps
Before configuring microaudio with your chosen embedded environment, you need to familiarize with the main components of the framework.

### Audio Buffer
To manage the audio stream in real time, it is necessary to manage the data in contiguous sections with an appropriate data structure. The **AudioBuffer** template class, contained within *audio_buffer.h* solves the problem by offering a container of numeric data of fixed length, which can be structured as a single channel, or as multiple channels modifying the template parameters.



```c++
#include "microaudio/include/audio_buffer.h"

// a stereo buffer of 16 bit integers of size 128
AudioBuffer<int, 2, 128> buffer1;

// a mono buffer of floats of size 256
AudioBuffer<float, 1, 256> buffer2;

// a multi channel buffer of doubles of size 64
AudioBuffer<double, 4, 64> buffer3;
```

The **AudioBuffer** is used extensively throughout the framework. For performance reasons, it is stuctured to directly access the arrays of the individual channels for reading and writing to the buffer. 

```c++
void test_buffer() {
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

    // this pointer is read only
    const int* readPointer = buffer.getReadPointer(0);
}
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
The main component that allows the integration with the embedded environment is called **AudioDriver**. The microaudio framework is designed to be adapted to multiple scenarios and does not implement any particular driver for a given architecture. Therefore it is up to the user to implement a class inheriting from **AudioDriver** present in *audio_driver.h*, following the guidelines explained in a later section.

From a high level perspective, an **AudioDriver** exposes to the user and to the framework itself a series of structural functions, ensuring the correct functioning of the developed audio application. It can be stated that this component is at an intermediate level between the hardware on which it is being developed, and the audio software components to be implemented.

### Audio Processor
The abstract class that implements the actual audio application is called **AudioProcessor** (found inside *audio_processor.h*) , which in turn uses the **AudioProcessable** interface (located in *audio_processable.h*). The processor is directly connected to the **AudioDriver**, and exposes the ```process``` method, inherited from **AudioProcessable**.

During execution, the **AudioDriver** will call the ```process``` method of the **AudioProcessor**, where the processor will process the **AudioBuffer** contained in the driver. After the processing, the driver will need to redirect the output buffer, using the DAC, or any output device used in the development environment.

## Driver Configuration
In order to configure microaudio with your embedded environment, you need to inherit from the class **AudioDriver** and implement its methods. This section proposes some guidelines to follow to successfully complete the implementation.
Currently, microaudio is structured to work with float buffers as far as the audio engine is concerned, but other types will be supported in a future update.

Before implementing the driver, it is necessary to modify the configuration header *audio_config.h*.

```c++
// example configuration
#define AUDIO_DRIVER_SAMPLE_RATE 44100
#define AUDIO_DRIVER_BUFFER_SIZE 256
#define AUDIO_DRIVER_BIT_DEPTH 16
```

These constants must be used during the development of the driver implementation, that will allow doing  future changes in a simpler and more elegant way.

The ```constructor``` must set the sampleRate and bufferSize attributes, using the previously configured values inside *audio_config.h*.

The ```init``` method must initialize all the devices necessary for the  audio rendering. This method is used to configure any timer, DACs, DMAs peripherals needed for the driver to work correctly. 

The ```start``` method implements the driver's operating logic. It must be called at runtime after the ```init``` method and after having associated a processor with the ```setAudioProcessable``` method.
It must be implemented as a blocking method, containing an infinite loop which, using the associated processor, processes the internal **AudioBuffer**, and then outputs it at audio frequency. 

![AudioDriver](https://user-images.githubusercontent.com/25433493/126712343-5ace7231-8406-4018-b7a3-4a1c103b73b1.png)

Given the demand for real time, it is recommended to use a DMA peripheral instead of a timer interrupts logic where available, since in this way it will be possible to make a better use of the cpu for audio processing, rather than using it for moving the buffer content into the output peripheral.

## Developing an audio application
After configuring the **AudioDriver**, it will finally be possible to develop the audio application exploiting the full potential of the framework. To better understand how to configure the code template needed to develop new software, you need to familiarize yourself with one last class.

### Audio Module
In order to facilitate the reuse and interconnection of processing blocks, you can use the **AudioModule** class. These modules can be defined to work with one or more channels, and by design, they fit the buffer length specified when configuring the audio driver.

To implement a custom audio module, you need create a new class inheriting from **AudioModule**

```c++
#include “audio_processor.h”
#include “audio_module.h”

// a stereo audio module that does nothing
class DummyModule: public AudioModule<2> {
public:
    // passing the audio processor as parameter of the class and of the superclass
    DummyModule(AudioProcessor &audioProcessor) : AudioModule<2>(audioProcessor) {};
    
    // this method can be used to process the audio buffer passed as parameter in input
    void process(AudioBuffer<float, 2, AUDIO_DRIVER_BUFFER_SIZE> &buffer) override {};

}
```

Audio modules can work together in series, processing a single buffer multiple times, or in parallel, using two or more buffers and adding them together after the processing.

```c++
#include "audio_buffer.h"

// your custom modules
#include "my_modules/saw_oscillator.h"
#include "my_modules/square_oscillator.h"
#include "my_modules/envelope.h"
#include "my_modules/delay.h"


// chaining example inside your processor, containing all the modules
void CustomProcessor::test_chain() {
    AudioBuffer<float, 1, AUDIO_DRIVER_BUFFER_SIZE> buffer1;
    AudioBuffer<float, 1, AUDIO_DRIVER_BUFFER_SIZE> buffer2;
    
    // series connection
    this.sawOscillator.process(buffer1);
    this.envelope.process(buffer1);
    
    // another series connection
    this.squareOscillator.process(buffer2);
    this.envelope.process(buffer2);

    // merging the parallel of the two series connections into the first buffer
    buffer1.add(buffer2);
    
    // adding another module after the merge
    this.delay.process(buffer1);    
}
```

### Putting all together
Now that all the main components of the framework have been described, it's possible to put it all together to build a simple example.

Let's start by creating an **AudioModule** for the tone generation (the actual code for the sine tone generation, will be explained in a later section, for now, the focus is into the code structure).


```c++
// my_modules/test_tone_generator.h

#include “audio_processor.h”
#include “audio_module.h”

// sine wave generator
class TestToneGenerator: public AudioModule<2> {
public:
    // passing the audio processor as parameter of the class and of the superclass
    DummyModule(AudioProcessor &audioProcessor) : AudioModule<2>(audioProcessor) {};
    
    // this method can be used to process the audio buffer passed as parameter in input
    void process(AudioBuffer<float, 2, AUDIO_DRIVER_BUFFER_SIZE> &buffer) override;

private:
    // attributes needed for the tone generation
}
```

```c++
// my_modules/test_tone_generator.cpp

#include "my_modules/test_tone_generator.h"

void TestToneGenerator::process(AudioBuffer<float, 2, AUDIO_DRIVER_BUFFER_SIZE> &buffer) {
    // here goes the code for the sine generation into the stereo buffer
}

```
Subsequently it is necessary to implement the **AudioProcessor** which will contain the module and which will be used to communicate with the driver.

```c++
// my_processors/example_processor.h

#include "audio_processor.h"
#include "my_modules/test_tone_generator.h"
    
    class ExampleProcessor : public AudioProcessor {
public:
    ExampleProcessor(AudioDriver &audioDriver) : AudioProcessor(audioDriver) {};
    
    void process() override;
    
private:
    TestToneGenerator testToneGenerator;
```

```c++
// my_processors/example_processor.cpp

void ExampleProcessor::process() {
    auto outputBuffer = getBuffer();
    testToneGenerator.process(outputBuffer);
}

```

Finally, all components need to be instantiated in the main.

```c++
// main.cpp

#include "my_drivers/audio_driver_implementation.h" // actual driver implementation
#include "my_processors/example_processor.h"
#include "my_modules/test_tone_generator.h"

int main() {
    AudioDriverImplementation audioDriver;
    ExampleProcessor processor(audioDriver); // linking the audioDriver to the processor
    
    audioDriver.init();
    audioDriver.setVolume(0.8); // just for lowering the volume
    audioDriver.setAudioProcessable(processor); // linking the processor to the audioDriver
    audioDriver.start() // starting the audio rendering loop

    return 0;
}
```

## More audio dsp developing tools
After having explored the structural functionalities of the framework, it is time to dive into its more advanced tools, which will make the development of dsp audio code easier.

### Circular Buffer
Microaudio offers a particularly useful data structure for audio software development, the **CircularBuffer**. This class can be used as a queue, but it has a fixed maximum length, specified during construction. When this data structure overflows, two different behaviors can be observed.

```c++
#include "circular_buffer.h"

void test_circular_buffer() {
    // two circular buffer containing at most 4 integers
    CircularBuffer<int,4> overridingQueue; // this queue overrides the oldest element on overflow
    CircularBuffer<int,4, CircularBufferType::Discard> discardingQueue; // this one instead discardes the new element
    
    for (int i = 0; i < 6; i++) {
        overridingQueue.push(i); // 3 4 5 6
        discardingQueue.push(i); // 0 1 2 3
    }
```

The interface of the class is the same of ```std::queue```.

### Audio Parameter
In a real time audio application, it is often necessary to take special care in parameter variation. The threads involved in the variation of the parameters, normally run with a lower frequency than the audio driver, the sudden variation of one of these parameters generates artifacts that are often audible and that reduce the general perceived quality of the software.

To solve the problem, you can make use of the **AudioParameter** template class. Using this class, it is possible to define a minimum transition time for each parameter, obtaining a smooth transition in its variation.

```c++
#include "audio_parameter.h"

// inside a CustomProcessor private section ...
    AudioParameter<float> volume(1.0f);
// ...

// inside the CustomProcessor constructor ...
    volume.setTransitionTime(0.02f, getSampleRate); // 2ms of transition time
// ...

```

When using the parameter inside the process, it's necessary to update its state at each frame (or after a certain number of frames).

```c++
// custom_processor.cpp

#include "custom_processor.h"

void CustomProcessor::process() {
    for (int i = 0; i < getBufferSize(); i++) {
    volume.updateSampleCount(1); // updating the state at every frame
    
    // ...
    getBuffer.applyGain(volume.getInterpolatedValue()); // using the smoothed value during the transition;   
}
```

### Lookup Table
The microaudio framework defines an **AudioMath** namespace inside *audio_math.h*, which contains some dsp utilities. One of these is the **LookupTable** class; it allows to generate a looktable starting from an input function, in order to avoid, during the execution in an audio thread, the call to mathematical functions, often too expensive to be used in embedded configurations.

```c++
#include "audio_math.h"
#define LUT_RESOLUTION 256 // the actual length of the array containing the function

#define LOG20 2.995732273553991f
#define LOG20000 9.903487552536127f

// a sine LUT containing one period of the function
AudioMath::LookupTable<LUT_TABLE_RESOLUTION> AudioMath::sineLut([](float x) { return std::sin(x); },
                                          0.0f, 2.0f * M_PI,
                                          LookupTableEdges::PERIODIC);

// a ramp LUT ranging from 0 to 1 that is also periodic
AudioMath::LookupTable<LUT_TABLE_RESOLUTION> AudioMath::rampLut([](float x) { return x; },
                                          0.0f, 1.0f,
                                          LookupTableEdges::PERIODIC);

// an exponential LUT ranging through the audible frequency spectrum, it extends at its ends
AudioMath::LookupTable<LUT_TABLE_RESOLUTION> AudioMath::expLut([](float x) { return std::exp(x); },
                                         LOG20, 20000,
                                         LookupTableEdges::EXTENDED);
```

Lookup tables can define three different behaviors outside the range indicated during construction.

```c++
// The LUT returns 0.0 outside the extremes.
LookupTableEdges::ZEROED;

// The extremes of the LUT are returned for input
// values outside the range.
LookupTableEdges::EXTENDED;

// The LUT repeates periodically in its range.
LookupTableEdges::PERIODIC;

```



