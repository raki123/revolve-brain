//
// Created by matteo on 6/15/17.
//

#ifndef TRIANGLEOFLIFE_PHOTOTAXISLEARNER_H
#define TRIANGLEOFLIFE_PHOTOTAXISLEARNER_H

#include <cmath>
#include "brain/FakeLightSensor.h"
#include "brain/Evaluator.h"
#include "BaseLearner.h"

namespace revolve {
namespace brain {

/**
 * PhototaxisLearner
 *
 * it encapsulate a learner and repeats the experiment 5 times with different light position. Fitness for one brain
 * is then calculated as the sum of the
 * @tparam EncapsulatedLearner
 */
template<typename EncapsulatedLearner>
class PhototaxisLearner : public BaseLearner {
public:
    /**
     * constructor
     *
     * Light sensor constructors are passed the coordinates with already the offset included
     *
     * @param robot_name name of the robot to use in logs
     * @param _encapsulatedLearner encapsulated learner that creates the controllers.
     * @param _light_constructor_left left fakelight factory.
     * @param _light_constructor_right right fakelight factory.
     * @param _light_radius_distance distance of the light from the robot.
     * @param evaluationTime Time each evaulation should last in seconds. Default to 30 seconds.
     * @param maxEvaluations Number of evaluation before the program should exit. If negative, the program will never
     * stop. Default to -1.
     */
    PhototaxisLearner(const std::string &robot_name,
                      std::unique_ptr<EncapsulatedLearner> _encapsulatedLearner,
                      std::function<boost::shared_ptr<FakeLightSensor>(
                              std::vector<float> coordinates)> _light_constructor_left,
                      std::function<boost::shared_ptr<FakeLightSensor>(
                              std::vector<float> coordinates)> _light_constructor_right,
                      std::vector< SensorPtr > &_sensors,
                      double _light_radius_distance,
                      const float evaluationTime = 30,
                      const long maxEvaluations = -1)
            : BaseLearner(robot_name)
              , encapsulatedLearner(std::move(_encapsulatedLearner))
              , phase(END)
              , light_constructor_left(_light_constructor_left)
              , light_constructor_right(_light_constructor_right)
              , sensors(&_sensors)
              , current_light_left(nullptr)
              , current_light_right(nullptr)
              , light_radius_distance(_light_radius_distance)
              , partial_fitness(0)
              , start_eval_time(std::numeric_limits<double>::lowest())
              , generation_counter(0)
              , EVALUATION_TIME(evaluationTime)
              , MAX_EVALUATIONS(maxEvaluations)
    {
        // Inject fake light sensor in sensor list
        this->current_light_left_pos = _sensors.size();
        _sensors.push_back(this->current_light_left);
        this->current_light_right_pos = _sensors.size();
        _sensors.push_back(this->current_light_right);
    }

    BaseController *update(const std::vector<SensorPtr> &sensors, double t, double step) override {

        // Evaluate policy on certain time limit
        if ((t - start_eval_time) > EVALUATION_TIME) {
            // check if to stop the experiment. Negative value for MAX_EVALUATIONS will never stop the experiment
            if (MAX_EVALUATIONS > 0 && generation_counter > MAX_EVALUATIONS) {
                std::cout << "#PhototaxisLearner::update() Max Evaluations (" << MAX_EVALUATIONS << ") reached. stopping now."
                          << std::endl;
                std::exit(0);
            }

            this->learner(t);
        }

        return BaseLearner::update(sensors, t, step);
    }

    virtual ~PhototaxisLearner() {}

protected:
    double getFitness() { return partial_fitness; };

    #define MAX_PHASE_FITNESS 100
    double getPhaseFitness() {
        double left_eye = current_light_left == nullptr ? std::numeric_limits<double>::min() :
                          current_light_left->read();
        double right_eye = current_light_right == nullptr ? std::numeric_limits<double>::min() :
                           current_light_right->read();

        double value = (left_eye + right_eye)
                       - ((left_eye - right_eye) * (left_eye - right_eye));

        //double value = (left_eye + right_eye)
        //               - std::fabs(left_eye - right_eye);

        if (value > MAX_PHASE_FITNESS)
            return MAX_PHASE_FITNESS;

        return value;
    };
    #undef MAX_PHASE_FITNESS

    void learner(double t) {
        // FITNESS update
        if (phase != END) {
            double phase_fitness = getPhaseFitness();
            std::cout << "SUPGBrainPhototaxis::update() - partial fitness[" << phase << "]: " << phase_fitness
                      << std::endl;
            partial_fitness += phase_fitness;
        }

        // Advance Phase
        switch (phase) {
            case CENTER:
                phase = LEFT;
                break;
            case LEFT:
                phase = RIGHT;
                break;
            case RIGHT:
                phase = MORELEFT;
                break;
            case MORELEFT:
                phase = MORERIGHT;
                break;
            case MORERIGHT:
                phase = END;
                break;
            case END:
                std::cout << "PhototaxisLearner::update() - INIT!" << std::endl;
        }

        // If phase is `END`, start a new phase
        if (phase == END) {
            auto fitness = getFitness();
            std::cout << "SUPGBrainPhototaxis::update() - finished with fitness: "
                      << fitness << std::endl;

            generation_counter++;

            // creates new controller from wrapper learner
            BaseController * new_controller = this->create_new_controller(fitness);
            if (new_controller != active_controller.get()) {
                active_controller.reset(new_controller);
            }

            partial_fitness = 0;

            std::cout << "SUPGBrainPhototaxis::update() - NEW BRAIN (generation " << generation_counter << " )"
                      << std::endl;
            phase = CENTER;
        }

        // reposition learner lights
        std::vector<float> relative_coordinates;

        static const double pi = std::acos(-1);
        static const double angle_15 = pi / 12;
        static const double angle_52_5 = 7 * pi / 24;

        const double radius = light_radius_distance;
        const float x_52_5 = (const float) (std::cos(angle_52_5) * radius);
        const float y_52_5 = (const float) (std::sin(angle_52_5) * radius);
        const float x_15 = (const float) (std::cos(angle_15) * radius);
        const float y_15 = (const float) (std::sin(angle_15) * radius);

        switch (phase) {
            case CENTER:
                relative_coordinates = {0, static_cast<float>(radius)};
                break;
            case LEFT:
                relative_coordinates = {-x_52_5, y_52_5};
                break;
            case RIGHT:
                relative_coordinates = {x_52_5, y_52_5};
                break;
            case MORELEFT:
                relative_coordinates = {-x_15, y_15};
                break;
            case MORERIGHT:
                relative_coordinates = {x_15, y_15};
                break;
            case END:
                std::cerr << "#### SUPGBrainPhototaxis::learner - END PHASE SHOULD NOT BE POSSIBLE HERE!" << std::endl;
        }

        //delete current_light_left; //shared pointer takes care of this already
        //delete current_light_right; //shared pointer takes care of this already

        this->current_light_left  = this->light_constructor_left(relative_coordinates);
        this->current_light_right = this->light_constructor_right(relative_coordinates);

        this->sensors->at(this->current_light_left_pos)  = this->current_light_left;
        this->sensors->at(this->current_light_right_pos) = this->current_light_right;

        // evaluation restart
        start_eval_time = t;
    }


    BaseController *create_new_controller(double fitness) override {
        return encapsulatedLearner->create_new_controller(fitness);
    }

protected:
    std::unique_ptr<EncapsulatedLearner> encapsulatedLearner;
    std::vector<SensorPtr> *sensors;

    enum PHASE {
        CENTER = 0,
        LEFT = 1,
        MORELEFT = 2,
        RIGHT = 3,
        MORERIGHT = 4,
        END = 5,
    } phase;

    std::function<boost::shared_ptr<FakeLightSensor>(std::vector<float> coordinates)>
            light_constructor_left,
            light_constructor_right;

    unsigned long current_light_left_pos;
    unsigned long current_light_right_pos;

    boost::shared_ptr<FakeLightSensor> current_light_left,
            current_light_right;

    double light_radius_distance;
    double partial_fitness;

    //
    unsigned int generation_counter;
    double start_eval_time;

    /**
     * Number of evaluations before the program quits. Usefull to do long run
     * tests. If negative (default value), it will never stop.
     *
     * Default value -1
     */
    const long MAX_EVALUATIONS; //= -1; // max number of evaluations

    /**
     * How long should an evaluation lasts (in seconds)
     *
     * 30 seconds is usually a good value
     */
    double EVALUATION_TIME;

    private:
    using BaseLearner::update;
};

}
}


#endif //TRIANGLEOFLIFE_PHOTOTAXISLEARNER_H
