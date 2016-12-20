add_library(revolve-brain-controller
    spline_controller.cpp
    ext_nn_net.cpp
    ext_nn_weights.cpp
)

add_subdirectory("extnn")

target_link_libraries(revolve-brain-controller 
	extnn
)
