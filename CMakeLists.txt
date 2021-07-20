cmake_minimum_required(VERSION 3.19)
project(microaudio CXX)

set(CMAKE_CXX_STANDARD 14)

add_library(${PROJECT_NAME}
        src/audio.h
        src/audio_buffer.h
        src/audio_config.h
        src/audio_math.h
        src/audio_module.h
        src/audio_parameter.h
        src/audio_processable.h
        src/audio_processor.h
        src/circular_buffer.h)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(tests)

