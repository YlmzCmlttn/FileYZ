cmake_minimum_required(VERSION 3.5)


project(Test LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3 -mavx")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -mavx")

set(THIRD_PARTY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/3rd_Party")

set (Cryptopp_INCLUDE_DIR ${THIRD_PARTY_PATH}/Cryptopp_8.6.0/include)
set (Cryptopp_LIBS_DIR ${THIRD_PARTY_PATH}/Cryptopp_8.6.0/lib)
set (Cryptopp_LIBS cryptopp)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

link_directories(${Cryptopp_LIBS_DIR})
include_directories(${Cryptopp_INCLUDE_DIR})

add_executable(Test
    main.cpp
    mainwindow.cpp
    mainwindow.h)

target_link_libraries(Test PRIVATE Qt5::Widgets stdc++fs pthread ${Cryptopp_LIBS})