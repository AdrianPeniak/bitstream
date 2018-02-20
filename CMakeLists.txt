cmake_minimum_required(VERSION 2.8)
project(BitStream)

add_library(${PROJECT_NAME} INTERFACE)

#target_sources(${PROJECT_NAME} INTERFACE lib/BitStream.h)

target_include_directories(${PROJECT_NAME} INTERFACE
    "${PROJECT_SOURCE_DIR}/lib"
)

install(FILES
    lib/BitStream.h
    DESTINATION
    include
)