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
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS system python3)
message(STATUS "boost: ${Boost_FOUND} - system: ${Boost_SYSTEM_FOUND} - python3: ${Boost_PYTHON3_FOUND}")

### RLPOWER DEPENDENCIES ###
# GSL
# Gnu mathematical library
find_package(GSL REQUIRED)


if (${Boost_SYSTEM_FOUND})
else()
    message(FATAL_ERROR "boost system required and not found")
endif()

if (${Boost_PYTHON3_FOUND})
else()
    if (${APPLE})
        message(FATAL_ERROR "boost python3 not found, try to run command\n"
                "$ brew install boost-python --without-python --with-python3")
    endif()

    # try python-py34 for raspberry
    message(STATUS "boost python3 not found, searching for python-py34")
    find_package(Boost COMPONENTS system python-py34)
    if ($Boost_PYTHON3_FOUND)
    else()
        message(STATUS "boost python-py34 not found, searching for python-py35")
        find_package(Boost REQUIRED COMPONENTS system python-py35)
    endif()
endif()

if (${APPLE})
    set(PYTHON_INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS} "/usr/include/python3.5m")
    set(Python_LIBRARIES ${Python_LIBRARIES} "/Library/Frameworks/Python.framework/Versions/3.5/lib/python3.5/config-3.5m/libpython3.5m.a")
endif()


# end Libraries stuff
include_directories(${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR} ${GSL_INCLUDE_DIRS})


# Compiling ####################################################################
add_library(revolve-brain SHARED brain/rlpower.cpp brain/neural_network.cpp)
add_library(revolve-brain-static STATIC brain/rlpower.cpp brain/neural_network.cpp)
python_add_module(revolve_brain_python brain/python/brain.cpp brain/python/rlpower_python.cpp)


# Linking ######################################################################
target_link_libraries(revolve-brain ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${GSL_LIBRARIES})
target_link_libraries(revolve_brain_python revolve-brain)


# Install ######################################################################
install(TARGETS revolve-brain DESTINATION lib)
#install(TARGETS revolve_brain_python DESTINATION lib)


# Tests ########################################################################
#if tests are enabled
enable_testing()
file(COPY brain/python/import_test.py DESTINATION .)
file(COPY brain/python/neuralnetwork_test.py DESTINATION .)
file(COPY brain/python/sensor_test.py DESTINATION .)
file(COPY brain/python/actuator_test.py DESTINATION .)
file(COPY brain/python/rlpower_test.py DESTINATION .)
add_test(00-Import python3 -m unittest import_test.py)
add_test(01-Sensor python3 -m unittest sensor_test.py)
add_test(01-Actuator python3 -m unittest actuator_test.py)
add_test(03-NeuralNetwork python3 -m unittest neuralnetwork_test.py)
add_test(04-RLPower python3 -m unittest rlpower_test.py)
