//
// Created by matteo on 3/13/17.
//

#ifndef REVOLVE_BRAIN_ACCNEATCPPNCONTROLLER_H
#define REVOLVE_BRAIN_ACCNEATCPPNCONTROLLER_H

#include "network/cpu/cpunetwork.h"
#include "BaseController.h"

namespace revolve { namespace brain {

    class AccNEATCPPNController : public BaseController {
    public:
        AccNEATCPPNController(size_t n_inputs, size_t n_outputs);

        virtual ~AccNEATCPPNController();


        /**
         * @brief Update the controller to the next step and sends signals
         * to the actuators
         *
         * @param actuators outputs of the controller
         * @param sensors inputs of the controller
         * @param t global time reference
         * @param step time since last update
         */
        virtual void
        update(const std::vector<ActuatorPtr> &actuators,
               const std::vector<SensorPtr> &sensors,
               double t,
               double step) override;

        void activate(const double *const inputs);

        NEAT::real_t *getOutputs() {
          return cppn->get_outputs();
        }

        void setCPPN(NEAT::CpuNetwork *cppn);

    protected:
        NEAT::CpuNetwork *cppn;

        const size_t n_inputs, n_outputs;

    private:
        double *inputs_vector;
        double *outputs_vector;
    };

}}


#endif //REVOLVE_BRAIN_ACCNEATCPPNCONTROLLER_H
