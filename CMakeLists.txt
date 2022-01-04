cmake_minimum_required(VERSION 3.16.3)
set(PROJECT_NAME rtsp_socket_relay)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        RtspManager.cpp
        RtpManager.cpp
        RtcpManager.cpp include/RtcpManager.h
        Util.cpp util/Network.cpp )
set(EXECUTABLE_FILES main.cpp)

add_library(${PROJECT_NAME}-lib ${SOURCE_FILES})
add_executable(${PROJECT_NAME}-exe ${EXECUTABLE_FILES})
target_link_libraries(${PROJECT_NAME}-exe ${PROJECT_NAME}-lib)