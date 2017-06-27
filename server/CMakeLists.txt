cmake_minimum_required(VERSION 3.7)
project(server)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        command.h
        deckC.cpp
        deckC.h
        log.cpp
        log.h
        pack.cpp
        pack.h
        seat.cpp
        seat.h
        server.cpp
        server.h
        socketHandler.cpp)

add_executable(server ${SOURCE_FILES})