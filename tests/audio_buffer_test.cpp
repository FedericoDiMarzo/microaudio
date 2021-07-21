#include "catch.hpp"
#include "../include/audio_buffer.h"

TEST_CASE("AudioBuffer", "[audio]") {

    SECTION("buffer index check") {
        AudioBuffer<int, 4, 128> buffer;
        int *bufferRaw = buffer.getWritePointer(buffer.getNumChannels() - 1);
        REQUIRE_NOTHROW(bufferRaw[buffer.getBufferLength() - 1]);
        REQUIRE(buffer.getNumChannels() == buffer.getBufferContainer().size());
        REQUIRE(buffer.getBufferLength() == buffer.getBufferContainer()[0].size());
    }

    SECTION("stereo int buffers") {
        AudioBuffer<int, 2, 128> buffer1;
        AudioBuffer<int, 2, 128> buffer2;
        int *bufferRaw1L = buffer1.getWritePointer(0);
        int *bufferRaw1R = buffer1.getWritePointer(1);

        // writing some values on buffer1
        for (int i = 0; i < 128; i++) {
            bufferRaw1L[i] = i;
            bufferRaw1R[i] = i * 2;
        }

        SECTION("copying") {
            buffer2.copyFrom(buffer1);
            REQUIRE(buffer1.getBufferContainer() == buffer2.getBufferContainer());
        }

        SECTION("copyingOnChannel") {
            AudioBuffer<int, 1, 128> monoBuffer;
            buffer1.copyOnChannel(monoBuffer, 0);
            buffer1.copyOnChannel(monoBuffer, 1);
            REQUIRE(buffer1.getBufferContainer() == buffer2.getBufferContainer());
        }

        SECTION("summing") {
            buffer2.add(buffer1);
            REQUIRE(buffer1.getBufferContainer() == buffer2.getBufferContainer());
        }

        SECTION("multiplying") {
            buffer1.multiply(buffer2);
            REQUIRE(buffer1.getBufferContainer() == buffer2.getBufferContainer());
        }

        SECTION("gain") {
            buffer2.copyFrom(buffer1);
            buffer2.applyGain(-1.0);
            buffer2.add(buffer1);
            buffer1.clear();
            REQUIRE(buffer1.getBufferContainer() == buffer2.getBufferContainer());
        }
    }
}