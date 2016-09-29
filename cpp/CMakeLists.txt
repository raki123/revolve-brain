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
# build static libraries with position indipendent code, so they can be used in dynamic libraries
set (CMAKE_POSITION_INDEPENDENT_CODE TRUE)

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


if (NOT ${Boost_SYSTEM_FOUND})
    message(FATAL_ERROR "boost system required and not found")
endif()

if (NOT ${Boost_PYTHON3_FOUND})
    if (${APPLE})
        message(FATAL_ERROR "boost python3 not found, try to run command\n"
                "$ brew install boost-python --without-python --with-python3")
    endif()

    # try python-py34 for raspberry
    message(STATUS "boost python3 not found, searching for python-py34")
    find_package(Boost COMPONENTS system python-py34)
    if (NOT $Boost_PYTHON3_FOUND)
        message(STATUS "boost python-py34 not found, searching for python-py35")
        find_package(Boost REQUIRED COMPONENTS system python-py35)
    endif()
endif()

if (${APPLE})
    set(PYTHON_INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS} "/usr/include/python3.5m")
    set(Python_LIBRARIES ${Python_LIBRARIES} "/Library/Frameworks/Python.framework/Versions/3.5/lib/python3.5/config-3.5m/libpython3.5m.a")
endif()

# add accneat
add_subdirectory("neat/accneat")
#TODO make include path for accneat nicer
include_directories("neat/accneat/src")

# end Libraries stuff
include_directories(${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR} ${GSL_INCLUDE_DIRS})


file(GLOB_RECURSE BRAIN_SRCS
        brain/rlpower.cpp
        brain/neural_network.cpp
        brain/supgbrain.cpp
        brain/supg/supgneuron.cpp
        brain/supg/supggenomemanager.cpp
        neat/asyncneat.cpp
        neat/neatevaluation.cpp )

# Compiling ####################################################################
add_library(revolve-brain-static STATIC ${BRAIN_SRCS})
add_library(revolve-brain SHARED)

python_add_module(revolve_brain_python
    brain/python/brain.cpp
    brain/python/rlpower_python.cpp
    brain/python/supgbrain_python.cpp
)


# Linking ######################################################################
target_link_libraries(revolve-brain-static accneat ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${GSL_LIBRARIES})
target_link_libraries(revolve-brain revolve-brain-static)
target_link_libraries(revolve_brain_python revolve-brain-static)


# Install ######################################################################
install(TARGETS revolve-brain DESTINATION lib)
#install(TARGETS revolve_brain_python DESTINATION lib)


# Tests ########################################################################
#if tests are enabled
enable_testing()
file(COPY brain/python/test/import_test.py DESTINATION .)
file(COPY brain/python/test/neuralnetwork_test.py DESTINATION .)
file(COPY brain/python/test/sensor_test.py DESTINATION .)
file(COPY brain/python/test/actuator_test.py DESTINATION .)
file(COPY brain/python/test/rlpower_test.py DESTINATION .)
file(COPY brain/python/test/supg_test.py DESTINATION .)
add_test(00-Import python3 -m unittest import_test.py)
add_test(01-Sensor python3 -m unittest sensor_test.py)
add_test(01-Actuator python3 -m unittest actuator_test.py)
add_test(03-NeuralNetwork python3 -m unittest neuralnetwork_test.py)
add_test(04-RLPower python3 -m unittest rlpower_test.py)
add_test(05-SUPG python3 -m unittest supg_test.py)

add_executable(testAsyncNeat neat/test/testasyncneat.cpp)
add_executable(testCustomGenomeManager neat/test/testcustomgenomemanager.cpp)
add_executable(testMultiNNSpecies neat/test/testmultinnspeciesneat.cpp)
add_executable(testSUPGBrain test/testsupgbrain.cpp)
target_link_libraries(testAsyncNeat revolve-brain)
target_link_libraries(testCustomGenomeManager revolve-brain)
target_link_libraries(testMultiNNSpecies revolve-brain)
target_link_libraries(testSUPGBrain revolve-brain)
add_test(testAsyncNeat testAsyncNeat)
add_test(testCustomGenomeManager testCustomGenomeManager)
add_test(testMultiNNSpecies testMultiNNSpecies)
add_test(testSUPGBrain testSUPGBrain)
