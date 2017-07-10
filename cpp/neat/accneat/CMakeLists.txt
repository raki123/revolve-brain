cmake_minimum_required(VERSION 3.0)

project(accneat)

set(CMAKE_CXX_STANDARD 11)

if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  find_package(OpenMP REQUIRED)
else ()
  message("BUILDING WITHOUT OPENMP SUPPORT")
endif ()

# yaml cpp
#find_package(yaml-cpp REQUIRED)

#TODO enable cuda

set(accneat_sources
#    src/experiments/experiment.cpp
#    src/experiments/maze/maze.cpp
#    src/experiments/static/cfg.cpp
#    src/experiments/static/regex.cpp
#    src/experiments/static/sequence.cpp
#    src/experiments/static/xor.cpp
    src/genomemanager.cpp
    src/innovgenome/innovation.cpp
    src/innovgenome/innovgenome.cpp
    src/innovgenome/innovgenomemanager.cpp
    src/innovgenome/innovlinkgene.cpp
    src/innovgenome/innovnodegene.cpp
    src/innovgenome/trait.cpp
    # src/multiinnovgenome/multiinnovgenome.cpp
    # src/main.cpp
    src/neat.cpp
    src/network/cpu/cpunetwork.cpp
    src/organism.cpp
    src/population.cpp
    src/species/species.cpp
    src/species/speciesorganism.cpp
    src/species/speciespopulation.cpp
    src/multinnspecies/multinnspecies.cpp
    src/multinnspecies/multinnspeciesorganism.cpp
    src/multinnspecies/multinnspeciespopulation.cpp
    src/util/map.cpp
    src/util/resource.cpp
    src/util/rng.cpp
    src/util/timer.cpp
    src/util/util.cpp
    )

#set(accneat_cxx_sources
#    src/experiments/static/staticevaluator.cxx
#    src/experiments/maze/mazeevaluator.cxx
#    )

include_directories("src")

add_library(accneat ${accneat_sources}) # ${accneat_cxx_sources})
target_link_libraries( accneat ${YAML_CPP_LIBRARIES})

if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  target_compile_definitions(accneat PRIVATE WITH_OPENMP)
  target_link_libraries(accneat "gomp")
  set_target_properties(accneat PROPERTIES COMPILE_FLAGS "${OpenMP_CXX_FLAGS}")
endif ()

#add_executable(neat src/main.cpp)
#target_link_libraries(neat accneat)
