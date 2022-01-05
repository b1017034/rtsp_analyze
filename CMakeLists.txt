cmake_minimum_required(VERSION 3.16.3)
set(PROJECT_NAME rtsp_socket_relay)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 14)

find_package(Threads REQUIRED)

set(SOURCE_FILES
        SocketTransport.cpp
        RtspManager.cpp
        RtpManager.cpp
        RtcpManager.cpp include/RtcpManager.h
        Util.cpp util/Network.cpp)
set(EXECUTABLE_FILES main.cpp)
set(TESTABLE_FILES main2.cpp)

add_library(${PROJECT_NAME}-lib ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}-lib ${CMAKE_THREAD_LIBS_INIT})

add_executable(${PROJECT_NAME}-exe ${EXECUTABLE_FILES})
target_link_libraries(${PROJECT_NAME}-exe ${PROJECT_NAME}-lib)

add_executable(${PROJECT_NAME}-test-exe ${TESTABLE_FILES})
target_link_libraries(${PROJECT_NAME}-test-exe ${PROJECT_NAME}-lib)