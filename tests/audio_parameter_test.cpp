#include "catch.hpp"
#include "../src/audio_parameter.h"

TEST_CASE("AudioParameter", "[audio]") {
    AudioParameter<float> parameter(30.0);

    SECTION("constructor") {
        REQUIRE(parameter.getValue() == Approx(30.0));
        REQUIRE(parameter.getLastValue() == Approx(30.0));
    }

    SECTION("updating the value") {
        AudioParameter<float> parameter2(30.0);
        parameter2.setValue(50.0);
        REQUIRE(parameter2.getValue() == Approx(50.0));
        REQUIRE(parameter2.getLastValue() == Approx(30.0));
    }

    SECTION("transition sample test") {
        parameter.setTransitionSamples(10);
        parameter.setValue(60.0);

        SECTION("normal transition") {
            REQUIRE(parameter.getTransitionIndex() == Approx(0));
            parameter.updateSampleCount(5);
            REQUIRE(parameter.transitionIsComplete() == false);
            REQUIRE(parameter.getTransitionIndex() == Approx(0.5));
            REQUIRE(parameter.transitionIsComplete() == false);
            parameter.updateSampleCount(5);
            REQUIRE(parameter.transitionIsComplete() == true);
            REQUIRE(parameter.getTransitionIndex() == Approx(1));
            REQUIRE(parameter.getInterpolatedValue() == Approx(60));
        }

        SECTION("overflow") {
            REQUIRE(parameter.transitionIsComplete() == false);
            parameter.updateSampleCount(100);
            REQUIRE(parameter.transitionIsComplete() == true);
            REQUIRE(parameter.getTransitionIndex() == Approx(1));
        }
    }

    SECTION("transition time test") {
        parameter.setTransitionTime(10, 1);
        parameter.setValue(60.0);
        REQUIRE(parameter.transitionIsComplete() == false);
        REQUIRE(parameter.getTransitionIndex() == Approx(0));
        parameter.updateSampleCount(10);
        REQUIRE(parameter.transitionIsComplete() == true);
        REQUIRE(parameter.getTransitionIndex() == Approx(1));
    }
}
