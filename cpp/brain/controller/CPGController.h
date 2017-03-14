//
// Created by matteo on 3/13/17.
//

#ifndef TRIANGLEOFLIFE_CPGCONTROLLER_H
#define TRIANGLEOFLIFE_CPGCONTROLLER_H

#include "brain/cpg/CPGNetwork.h"
#include "BaseController.h"

namespace revolve { namespace brain {

    class CPGController : public BaseController {
    public:
        CPGController(size_t n_inputs, size_t n_outputs);

        virtual ~CPGController();

        void update(const std::vector<ActuatorPtr> &actuators,
                    const std::vector<SensorPtr> &sensors,
                    double t, double step) override;

        std::vector<cpg::CPGNetwork *>::iterator beginCPGNetwork()
        { return cpgs.begin(); }

        std::vector<cpg::CPGNetwork *>::iterator endCPGNetwork()
        { return cpgs.end(); }

    protected:
        void initRandom(float sigma);

    protected:
        size_t n_inputs, n_outputs;

        //list of cpgs
        std::vector<cpg::CPGNetwork *> cpgs;

        /** Connection matrix between the different servos
         * First is start of the connections, second is end.
         * Example: connections[0][1].we is the connection starting from servo 0
         * and reacing servo 1 for the RythmGenerationNeurons E
         */
        std::vector<std::vector<cpg::CPGNetwork::Weights>> connections;

        // CACHING VECTORS
        double *inputs_vector,
            *outputs_vector;
    };

}}


#endif //TRIANGLEOFLIFE_CPGCONTROLLER_H
