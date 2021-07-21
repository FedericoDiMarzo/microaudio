
#ifndef MIOSIX_AUDIO_AUDIO_MATH_H
#define MIOSIX_AUDIO_AUDIO_MATH_H

#include <functional>
#include <array>

namespace AudioMath {

    /**
     * Linear interpolation between two values.
     * the mix parameter must be between 0 and 1
     *
     * @param a first value to interpolate
     * @param b second value to interpolate
     * @param mix interpolation index between 0 and 1
     * @return linear interpolation between a and b based on mix
     */
    inline float linearInterpolation(float a, float b, float mix) {
        return (a * (1.0f - mix)) + (b * mix);
    };

    /**
     * Linear mapping of input x from an input range
     * to an output range.
     *
     * @param x input
     * @param inputMin input range minimum
     * @param inputMax input range maximum
     * @param outputMin output range minimum
     * @param outputMax output range maximum
     * @return linear transformation of x in the output range
     */
    inline float linearMap(float x, float inputMin, float inputMax, float outputMin, float outputMax) {
        return (x - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin;
    }

    /**
     * Clips the input in a specified range.
     *
     * @param x input
     * @param min low clip bound
     * @param max high clip bound
     * @return clipped input
     */
    inline float clip(float x, float min, float max) {
        x = (x < min)? min : x;
        x = (x > max)? max : x;
        return x;
    }

    /**
     * Enumeration describing the behaviour of
     * the LUT tables outside the max and argMin extremes.
     */
    enum class LookupTableEdges {
        /**
         * The LUT returns 0.0 outside the extremes.
         */
        ZEROED,

        /**
         * The extremes of the LUT are returned for input
         * values outside the range.
         */
        EXTENDED,

        /**
         * The LUT describes a periodic function between argMin and max.
         */
        PERIODIC
    };

    /**
     * Implementation of a lookup table.
     *
     * @tparam SIZE dimension of the lookup table
     */
    template<size_t SIZE>
    class LookupTable {
    public:
        /**
         * Constructor.
         *
         * @param function it must return a float and have a float as a single parameter.
         * @param argMin minimum argument of the function stored in the LUT
         * @param argMax maximum argument of the function stored in the LUT
         * @param edges behaviour of the LUT exceeding the edges (check LookupTableEdges)
         */
        LookupTable(std::function<float(float)> function, float argMin, float argMax,
                    LookupTableEdges edges = LookupTableEdges::EXTENDED)
                : argMin(argMin), argMax(argMax), edges(edges) {

            float x; // input value of the function
            for (uint32_t i = 0; i < SIZE; i++) {
                // getting the x value through a linear interpolation
                // between max and argMin
                x = linearInterpolation(argMin, argMax, static_cast<float>(i) / static_cast<float>(SIZE));

                // storing f(x) in the table
                table[i] = function(x);
            }

            switch (edges) {
                case LookupTableEdges::PERIODIC:
                    // Set last position (table size is SIZE+1) equal to the first
                    table[SIZE] = table[0];
                    break;
                case LookupTableEdges::EXTENDED:
                    // Set last position equal to last (extending it)
                    table[SIZE] = table[SIZE - 1];
                    break;
                case LookupTableEdges::ZEROED:
                    // Set last position to 0 (zero padding it)
                    table[SIZE] = 0;
                    break;
            }
        };

        /**
         * Using the operator() simulates a call to the approximated
         * function using the internal lookup table and LookupTableEdges rule
         *
         * @param x input value
         * @return output value approximated using the LUT
         */
        inline float operator()(float x) {
            float y;
            if ((x < argMin) | (x >= argMax)) {
                switch (edges) {
                    case LookupTableEdges::ZEROED:
                        y = 0;
                        break;
                    case LookupTableEdges::EXTENDED:
                        y = (x < argMin) ? table[0] : table[SIZE - 1];
                        break;
                    case LookupTableEdges::PERIODIC:
                        // wrapping the value to the correct range
                        // and calling again operator()
                        // TODO: warning on phase wrapping
                        while (x < argMin) x += (argMax-argMin);
                        while (x >= argMax) x -= (argMax-argMin);
                        y = this->operator()(x);
                        break;
                }
            } else {
                // Extract index as a float
                float floatIndex = linearMap(x, argMin, argMax, 0, SIZE);

                // Cast float index to the nearest integers
                int index0 = static_cast<int>(floatIndex);
                int index1 = index0 + 1;

                float interpolationFactor = floatIndex - index0;
                float value0 = table[index0];
                float value1 = table[index1];

                y = linearInterpolation(value0, value1, interpolationFactor);
            }
            return y;
        };

    private:
        float argMin;
        float argMax;
        LookupTableEdges edges;
        std::array<float, SIZE + 1> table;

        LookupTable(const LookupTable &);

        LookupTable &operator=(const LookupTable &);
    };

};

#endif //MIOSIX_AUDIO_AUDIO_MATH_H
