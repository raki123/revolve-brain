#ifndef REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_NEURON_H_
#define REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_NEURON_H_

#include <map>
#include <string>
#include <vector>

//class containg all information that is evolved for neurons
namespace CPPNEAT {
class Neuron
{

public:
    enum Layer
    {
        INPUT_LAYER,
        OUTPUT_LAYER,
        HIDDEN_LAYER
    };
    enum Ntype
    {
        SIMPLE = 0,
        SIGMOID = 1,
        DIFFERENTIAL_CPG = 2,
        INPUT = 3,
        BIAS = 4,
        OSCILLATOR = 5,
        INPUT_OSCILLATOR = 6
    };
    struct ParamSpec
    {
        std::string name;
        double min_value;
        double max_value;
        bool max_inclusive;
        bool min_inclusive;
        double epsilon;
    };
    struct NeuronTypeSpec
    {
        std::vector<ParamSpec> param_specs;
        std::vector<Layer> possible_layers;
    };

    Neuron(Neuron &copy_of);

    Neuron(std::string neuron_id,
           Layer layer,
           Ntype neuron_type,
           std::map<std::string, double> neuron_params);

    void
    set_neuron_param(double value,
                     ParamSpec param_spec);

    /**
     * tostring     def __str__(self):
     * return "id={0}, layer={1}, type={2}, part={3}".format(self.neuron_id, self.layer, self.neuron_type, self.body_part_id)
     */

    std::string neuron_id;
    Layer layer;
    Ntype neuron_type;
    std::map<std::string, double> neuron_params;


};
}

#endif  // REVOLVEBRAIN_BRAIN_LEARNER_CPPNNEAT_NEURON_H_