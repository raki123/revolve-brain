add_library(revolve-brain-learner
            BaseLearner.cpp
            NEATLearner.cpp
            RLPowerLearner.cpp
            AccNEATLearner.cpp
            HyperAccNEATLearner_CPGController.cpp
            )

add_subdirectory("cppneat")

target_link_libraries(revolve-brain-learner
                      cppneat
                      )
