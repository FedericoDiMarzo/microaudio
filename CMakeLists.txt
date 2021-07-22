cmake_minimum_required(VERSION 3.19)
project(microaudio CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS " -Wa,-mbig-obj")

add_library(${PROJECT_NAME}
        include/audio_driver.h
        include/audio_buffer.h
        include/audio_config.h
        include/audio_math.h
        include/audio_module.h
        include/audio_parameter.h
        include/audio_processor.h
        include/audio_processable.h
        include/circular_buffer.h)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(tests)

