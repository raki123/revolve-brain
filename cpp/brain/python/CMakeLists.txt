# python3 libraries
find_package(PythonLibs REQUIRED)

# boost python3
find_package(Boost COMPONENTS system python3)
message(STATUS "boost: ${Boost_FOUND} - system: ${Boost_SYSTEM_FOUND} - python3: ${Boost_PYTHON3_FOUND}")

if (NOT ${Boost_FOUND})
    if (${APPLE})
        message(FATAL_ERROR "boost python3 not found, try to run command\n"
                "$ brew install boost-python --without-python --with-python3")
    endif()

    # try python-py35 for debian
    message(STATUS "boost python3 not found, searching for python-py35")
    find_package(Boost COMPONENTS system python-py35)
    if (NOT ${Boost_FOUND})
        message(STATUS "boost python-py35 not found, searching for python-py34")
        find_package(Boost COMPONENTS system python-py34)
    endif()
    if (NOT ${Boost_FOUND})
        message(STATUS "boost python-py34 not found, searching for python-py33")
        find_package(Boost COMPONENTS system python-py33)
    endif()
    if (NOT ${Boost_FOUND})
        message(STATUS "boost python-py33 not found, searching for python-py32")
        find_package(Boost COMPONENTS system python-py32)
    endif()
    if (NOT ${Boost_FOUND})
        message(STATUS "boost python-py32 not found, searching for python-py31")
        find_package(Boost COMPONENTS system python-py31)
    endif()

    if (NOT ${Boost_FOUND})
        message(FATAL_ERROR "boost python not found")
    endif()
    message(STATUS "boost: ${Boost_FOUND} - system: ${Boost_SYSTEM_FOUND} - python3: ${Boost_PYTHON3_FOUND}")
endif()

if (${APPLE})
    set(PYTHON_INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS} "/usr/include/python3.5m")
    set(Python_LIBRARIES ${Python_LIBRARIES} "/Library/Frameworks/Python.framework/Versions/3.5/lib/python3.5/config-3.5m/libpython3.5m.a")
endif()

# includes
include_directories(${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})

# add python wrapper target
python_add_module(revolve_brain_python
    brain.cpp
    rlpower_python.cpp
    supgbrain_python.cpp
)

# link libraries
target_link_libraries(revolve_brain_python revolve-brain-static ${Boost_LIBRARIES})

# install
#install(TARGETS revolve_brain_python DESTINATION lib)

# testing
file(COPY test/import_test.py DESTINATION .)
file(COPY test/neuralnetwork_test.py DESTINATION .)
file(COPY test/sensor_test.py DESTINATION .)
file(COPY test/actuator_test.py DESTINATION .)
file(COPY test/rlpower_test.py DESTINATION .)
file(COPY test/supg_test.py DESTINATION .)
add_test(00-Import /usr/bin/env python3 -m unittest import_test.py)
add_test(01-Sensor /usr/bin/env python3 -m unittest sensor_test.py)
add_test(01-Actuator /usr/bin/env python3 -m unittest actuator_test.py)
add_test(03-NeuralNetwork /usr/bin/env python3 -m unittest neuralnetwork_test.py)
add_test(04-RLPower /usr/bin/env python3 -m unittest rlpower_test.py)
add_test(05-SUPG /usr/bin/env python3 -m unittest supg_test.py)
