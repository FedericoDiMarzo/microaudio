#include "catch.hpp"
#include "../include/audio_math.h"
#include <functional>

TEST_CASE("linearInterpolation", "[audio]") {
    float a = -10;
    float b = 10;

    SECTION("mix = 0") {
        REQUIRE(AudioMath::linearInterpolation(a, b, 0) == Approx(-10));
    }

    SECTION("mix = 0.5") {
        REQUIRE(AudioMath::linearInterpolation(a, b, 0.5) == Approx(0));
    }

    SECTION("mix = 1") {
        REQUIRE(AudioMath::linearInterpolation(a, b, 1) == Approx(10));
    }
}

TEST_CASE("linearMap", "[audio]") {
    float x = 5;
    float y;

    SECTION("mapping back to the original value") {
        y = AudioMath::linearMap(x, 0, 10, 30, 50);
        y = AudioMath::linearMap(y, 30, 50, 0, 10);
        REQUIRE(x == Approx(y));
    }

    SECTION("inverting a value") {
        y = AudioMath::linearMap(x, 0, 1, 0, -1);
        REQUIRE(x == Approx(-y));
    }
}

TEST_CASE("clip", "[audio]") {
    SECTION("test on normalized range") {
        REQUIRE(AudioMath::clip(0.5, 0, 1) == Approx(0.5));
        REQUIRE(AudioMath::clip(-1, 0, 1) == Approx(0));
        REQUIRE(AudioMath::clip(2, 0, 1) == Approx(1));
    }

    SECTION("test on a larger range") {
        REQUIRE(AudioMath::clip(5.1, -10, 10) == Approx(5.1));
        REQUIRE(AudioMath::clip(-13.1, -10, 10) == Approx(-10));
        REQUIRE(AudioMath::clip(204.5, -10, 10) == Approx(10));
    }
}

TEST_CASE("LookupTable", "[audio]") {
    float testValue;
    SECTION("sine LUT") {
        auto f = [](float x) -> float { return std::sin(x); };
        AudioMath::LookupTable<4096> sineLUT(f, 0, 2 * M_PI, AudioMath::LookupTableEdges::PERIODIC);

        SECTION("values inside the table") {
            testValue = GENERATE(0.0, M_PI, M_PI / 4);
            REQUIRE(sineLUT(testValue) - std::sin(testValue) < 0.0001);
        }

        SECTION("periodicity") {
            testValue = GENERATE(-M_PI, 0.0, M_PI);
            REQUIRE(sineLUT(testValue) - sineLUT(2 * M_PI + testValue) < 0.0001);
        }
    }

    SECTION("tanh LUT") {
        auto f = [](float x) -> float { return std::tanh(x); };
        AudioMath::LookupTable<4096> tanhLUT(f, -3, 3 * M_PI, AudioMath::LookupTableEdges::EXTENDED);

        SECTION("values inside the table") {
            testValue = GENERATE(-2.5, -1, 0, 1.2, 2.7);
            REQUIRE(tanhLUT(testValue) - std::tanh(testValue) < 0.0001);
        }
    }

    SECTION("interpolation EXTENDED") {
        auto f = [](float x) -> float { return x; };
        AudioMath::LookupTable<5> linearLUT(f, 0, 5, AudioMath::LookupTableEdges::EXTENDED);

        SECTION("values in between LUT") {
            testValue = GENERATE(0.5, 1.5, 2.5, 3.5);
            REQUIRE(linearLUT(testValue) == Approx(testValue));
        }

        SECTION("values at extremities") {
            testValue = GENERATE(4.0, 4.2, 4.5, 5, 5.5);
            REQUIRE(linearLUT(testValue) == Approx(4.0));
        }
    }

    SECTION("interpolation PERIODIC & ZEROED") {
        auto f = [](float x) -> float { return x; };
        AudioMath::LookupTable<5> linearPeriodicLUT(f, 0, 5, AudioMath::LookupTableEdges::PERIODIC);
        AudioMath::LookupTable<5> linearZeroedLUT(f, 0, 5, AudioMath::LookupTableEdges::ZEROED);


        SECTION("values in between LUT") {
            testValue = GENERATE(0.5, 1.5, 2.5, 3.5);
            REQUIRE(linearPeriodicLUT(testValue) == Approx(testValue));
            REQUIRE(linearZeroedLUT(testValue) == Approx(testValue));
        }

        SECTION("values at extremities") {
            testValue = 4.5;
            REQUIRE(linearPeriodicLUT(testValue) == Approx(2.0));
            REQUIRE(linearZeroedLUT(testValue) == Approx(2.0));

            testValue = 4.9;
            REQUIRE(linearPeriodicLUT(testValue) == Approx(0.1*4));
            REQUIRE(linearZeroedLUT(testValue) == Approx(0.1*4));

            testValue = 5.0;
            REQUIRE(linearPeriodicLUT(testValue) == Approx(0.0));
            REQUIRE(linearZeroedLUT(testValue) == Approx(0.0));
        }
    }
}