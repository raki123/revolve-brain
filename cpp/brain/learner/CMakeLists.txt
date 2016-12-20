add_library(revolve-brain-learner
    base_learner.cpp
    neat_learner.cpp
    rlpower_learner.cpp
)

add_subdirectory("cppneat")

target_link_libraries(revolve-brain-learner.h
	cppneat
)
