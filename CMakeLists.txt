cmake_minimum_required(VERSION 3.14)

project(grasp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(SOURCES
    src/main.cpp
    src/GRASP.cpp
    src/parser.cpp
    src/to_python.cpp
)

find_package(nlohmann_json 3.2.0 QUIET)

add_executable(grasp ${SOURCES})

target_link_libraries(grasp PRIVATE nlohmann_json::nlohmann_json)

file(COPY ${CMAKE_SOURCE_DIR}/data DESTINATION ${CMAKE_BINARY_DIR})
