CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(revolve-brain)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR CMAKE_COMPILER_IS_GNUCXX)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(--std=c++11 SUPPORTS_STD_CXX11)
    if(SUPPORTS_STD_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=c++11")
    else()
        message(ERROR "Compiler does not support --std=c++11")
    endif()
endif()

FIND_PACKAGE(PythonLibs)
FIND_PACKAGE(Boost COMPONENTS python3 thread)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR})

ADD_LIBRARY(revolve-brain SHARED brain/neuralnetwork.cpp brain/actuator.cpp brain/sensor.cpp)
PYTHON_ADD_MODULE(revolve_brain_python brain/python/brain.cpp)

TARGET_LINK_LIBRARIES(revolve-brain ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
TARGET_LINK_LIBRARIES(revolve_brain_python revolve-brain)

install(TARGETS revolve-brain DESTINATION lib)
#install(TARGETS revolve_brain_python DESTINATION lib)
