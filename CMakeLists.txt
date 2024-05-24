# specify the minimum version of CMake that is supported
cmake_minimum_required(VERSION 3.16)

# include a project name
project(code)

# set C++ Version & executable output path
set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")

# make your compiler aware of header directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# find & include ncurses library
# find_package(Curses REQUIRED)
# Hartmann_Psi
# include_directories(${CURSES_INCLUDE_DIR})

# create a variable which includes needed source files
set(MY_SOURCES
    ${PROJECT_SOURCE_DIR}/main.cpp
    ${PROJECT_SOURCE_DIR}/account.cpp
    ${PROJECT_SOURCE_DIR}/input_process.cpp
    ${PROJECT_SOURCE_DIR}/time.cpp
    ${PROJECT_SOURCE_DIR}/train.cpp
)

# specify an executable, 
# build from the specified source files
add_executable(code ${MY_SOURCES})

# link ncurses library with your executable
# target_link_libraries(code ${CURSES_LIBRARY})