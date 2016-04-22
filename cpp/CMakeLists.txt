cmake_minimum_required(VERSION 2.8)
project(revolve-brain)


# C++11 ########################################################################
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR CMAKE_COMPILER_IS_GNUCXX)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(--std=c++11 SUPPORTS_STD_CXX11)
    if(SUPPORTS_STD_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=c++11")
    else()
        message(FATAL_ERROR "Compiler does not support --std=c++11")
    endif()
endif()


# Libraries ####################################################################
# python3 libraries
find_package(PythonLibs REQUIRED)

# boost libraries
if (${APPLE})
    find_package(Boost REQUIRED COMPONENTS system)
    find_package(Boost COMPONENTS system python3)

    if (NOT ${Boost_python3_FOUND})
        message(FATAL_ERROR "boost python3 not found, try to run command\n"
                "$ brew install boost-python --without-python --with-python3")
    endif()

    set(PYTHON_INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS} "/usr/include/python3.5m")
    set(Python_LIBRARIES ${Python_LIBRARIES} "/Library/Frameworks/Python.framework/Versions/3.5/lib/python3.5/config-3.5m/libpython3.5m.a")
else()
    find_package(Boost REQUIRED COMPONENTS system python3)
endif()

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

# end Libraries stuff
include_directories(${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR})


# Compiling ####################################################################
add_library(revolve-brain SHARED brain/neuralnetwork.cpp brain/actuator.cpp brain/sensor.cpp)
python_add_module(revolve_brain_python brain/python/brain.cpp)


# Linking ######################################################################
target_link_libraries(revolve-brain ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
target_link_libraries(revolve_brain_python revolve-brain)


# Install ######################################################################
install(TARGETS revolve-brain DESTINATION lib)
#install(TARGETS revolve_brain_python DESTINATION lib)


# Tests ########################################################################
#if tests are enabled
enable_testing()
file(COPY brain/python/import_test.py DESTINATION .)
file(COPY brain/python/neuralnetwork_test.py DESTINATION .)
add_test(00-Import python3 -m unittest import_test.py)
add_test(02-NeuralNetwork python3 -m unittest neuralnetwork_test.py)
