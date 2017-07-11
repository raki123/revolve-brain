//
// Created by Milan Jelisavcic on 8.7.17..
//

#ifndef TOL_REVOLVE_TYPES_H
#define TOL_REVOLVE_TYPES_H

namespace revolve {
namespace brain {

struct CPPNConfig;

class RafCPGController;

typedef boost::shared_ptr<revolve::brain::CPPNConfig> CPPNConfigPtr;
typedef boost::shared_ptr<revolve::brain::RafCPGController> RafCPGControllerPtr;

}
}

#endif //TOL_REVOLVE_TYPES_H
