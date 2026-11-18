cmake_minimum_required(VERSION 4.3)
project(Arq_sim C)

set(CMAKE_C_STANDARD 11)

add_executable(Arq_sim main.c
        BSC.c
        FileLoader.c
        CRC.c
        ARQ.c
        DetectionDecoder.c
        ARQ.h
        CRC.h
        FileLoader.h
        BSC.h
        DetectionDecoder.h
)
