add_library(revolve-brain-controller
    spline_controller.cpp
    policy_controller.cpp
    ext_nn_net.cpp
    ext_nn_weights.cpp
    layered_ext_nn_net.cpp
)

add_subdirectory("extnn")

target_link_libraries(revolve-brain-controller 
	extnn
)
