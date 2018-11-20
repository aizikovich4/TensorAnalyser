cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 14)
project(Tensor_Analyzer)

set(SOURCE_DIRS
                "."
                ".."
                )

# Формирование списка файлов с исходниками
foreach(arg ${SOURCE_DIRS})
    aux_source_directory(${arg} SOURCE_FILES)
endforeach(arg ${SOURCE_DIRS})

# Задаём пути для инклюдников
foreach(arg ${SOURCE_DIRS})
    include_directories(${arg})
endforeach()



FIND_PACKAGE(Boost COMPONENTS program_options filesystem REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Boost_LIBRARIES})
