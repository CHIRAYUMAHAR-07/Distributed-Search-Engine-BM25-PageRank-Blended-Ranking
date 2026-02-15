cmake_minimum_required(VERSION 3.16)
project(IndexEngine)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(index_engine
    src/main.cpp
    src/index_engine.cpp
    src/tokenizer.cpp
    src/serializer.cpp
)
