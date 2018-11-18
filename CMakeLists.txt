cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 14)
project(Tensor_Analyzer)

FIND_PACKAGE(Boost COMPONENTS program_options filesystem REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
add_executable(${PROJECT_NAME} "main.cpp")
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Boost_LIBRARIES})
