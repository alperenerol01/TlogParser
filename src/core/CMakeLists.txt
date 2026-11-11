add_library(TlogCore STATIC
    MavlinkPacket.h
    MavlinkFrameParser.h
    MavlinkFrameParser.cpp
    TlogReader.h
    TlogReader.cpp
    MavlinkDecoder.h
    MavlinkDecoder.cpp
)

target_include_directories(TlogCore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(TlogCore SYSTEM PRIVATE "${MAVLINK_INCLUDE_DIR}")

target_compile_options(TlogCore PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)
