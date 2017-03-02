add_library(revolve-brain-controller
            SplineController.cpp
            PolicyController.cpp
            ExtendedANN.cpp
            ExtendedANNWeights.cpp
            LayeredExtendedANN.cpp
            )

add_subdirectory("extnn")

target_link_libraries(revolve-brain-controller
                      extnn
                      )
