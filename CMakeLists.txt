# Minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(mrfsat CXX)  # replace 'MyProjectName' with your desired project name

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(mrfsat
    src/main.cpp
    src/parsers/opbparser.cpp
    src/filereader.cpp
    src/graph.cpp
)

# Include directories
target_include_directories(mrfsat PRIVATE src)
# If you have any compiler flags you'd like to add, you can do it as follows:
# target_compile_options(MyExecutable PRIVATE -Wall -Wextra -Wpedantic)
