add_library(revolve-brain-controller
            SplineController.cpp
            PolicyController.cpp
            RafCPGController.cpp
            ExtCPPNWeights.cpp
            LayeredExtCPPN.cpp
            AccNEATCPPNController.cpp
            CPGController.cpp
            )

add_subdirectory("extnn")

target_link_libraries(revolve-brain-controller
                      extnn
                      )
