//
// Created by matteo on 09/11/16.
//

#include "CPGBrain.h"
#include <random>

using namespace revolve::brain;

CPGBrain::CPGBrain(std::string robot_name,
                   EvaluatorPtr evaluator,
                   size_t n_actuators,
                   size_t n_sensors)
    : Brain()
    , robot_name(robot_name)
    , n_inputs(n_sensors)
    , n_actuators(n_actuators)
    , cpgs(n_actuators, nullptr)
    , connections(n_actuators, std::vector<cpg::CPGNetwork::Weights>(n_actuators))
    , evaluator(evaluator)
    , start_eval_time_(-1)
    , generation_counter_(0)
    // TODO read this values from config file
    , evaluation_rate_(30)
    , max_evaluations_(1000)
    , max_ranked_policies_(10)
    , noise_sigma_(0.1)
{
    size_t n_connections = n_actuators-1;

    for(size_t i=0; i<n_actuators; i++) {
        cpgs[i] = new cpg::CPGNetwork(n_sensors, n_connections);
    }

    for(size_t i=0; i<n_actuators; i++) {
        for(size_t j=0; j<n_actuators; j++) {
            if (i == j) continue;
            cpgs[i]->addConnection(cpgs[j]);
        }
    }


    std::random_device rd;
    std::mt19937 mt(rd());
    std::normal_distribution<float> dist(0, (float) this->noise_sigma_);

    // Init first random controller
    if (not current_policy_)
        current_policy_ = std::make_shared<Policy>(n_actuators);

    // init with random connections
//     for (auto &con_line: connections) {
//         for (cpg::CPGNetwork::Weights &con_elem: con_line) {
//             con_elem.we = dist(mt);
//             con_elem.wf = dist(mt);
//         }
//     }

    for (size_t i = 0; i < n_actuators; ++i) {
        size_t genome_size = 12 + 2*n_connections;
        GenomePtr spline = std::make_shared<Genome>(genome_size, 0);
        for (size_t j = 0; j < genome_size; ++j) {
            spline->at(j) = dist(mt);
        }
        current_policy_->at(i) = spline;
    }
    genomeToPhenotype();
}

CPGBrain::~CPGBrain()
{
    for(cpg::CPGNetwork* ptr : cpgs)
        delete ptr;
}

void CPGBrain::update(const std::vector<ActuatorPtr> &actuators,
                      const std::vector<SensorPtr> &sensors,
                      double t, double step)
{
    this->update<std::vector<ActuatorPtr>, std::vector<SensorPtr>>
            (actuators, sensors, t, step);
}

void CPGBrain::learner(double t)
{
    if (is_offline_) return;

    if (start_eval_time_ < 0)
        start_eval_time_ = t;

    if (not evaluator_started && (t - start_eval_time_) > (evaluation_rate_ / 10)) {
        evaluator->start();
        evaluator_started = true;
    }

    if ((t - start_eval_time_) > evaluation_rate_ && generation_counter_ < max_evaluations_) {
        double fitness = evaluator->fitness();
        //TODO learner

        this->updatePolicy(fitness);

        std::cout << "EVALUATION! fitness = " << fitness << std::endl;
        start_eval_time_ = t;
        //evaluator->start();
        evaluator_started = false;
    }
}

void CPGBrain::updatePolicy(double curr_fitness) {
    size_t source_y_size = 12; //TODO

    // Insert ranked policy in list
    PolicyPtr policy_copy = std::make_shared<Policy>(n_actuators);
    for (size_t i = 0; i < n_actuators; i++) {
        GenomePtr genome = current_policy_->at(i);
        policy_copy->at(i) = std::make_shared<Genome>(genome->begin(), genome->end());
    }
    ranked_policies_.insert({curr_fitness, policy_copy});

    // Remove worst policies
    while (ranked_policies_.size() > max_ranked_policies_) {
        auto last = std::prev(ranked_policies_.end());
        ranked_policies_.erase(last);
    }

    // Print-out current status to the terminal
    std::cout << robot_name << ":" << generation_counter_ << " ranked_policies_:";
    for (auto const &it : ranked_policies_) {
        double fitness = it.first;
        std::cout << " " << fitness;
    }
    std::cout << std::endl;

    // Write fitness and genomes log to output files
//     this->writeCurrent();
//     this->writeElite();

    // Update generation counter and check is it finished
    generation_counter_++;
    if (generation_counter_ == max_evaluations_) {
        std::exit(0);
    }

    /// Actual policy generation

    /// Determine which mutation operator to use
    /// Default, for algorithms A and B, is used standard normal distribution with decaying sigma
    /// For algorithms C and D, is used normal distribution with self-adaptive sigma
    std::random_device rd;
    std::mt19937 mt(rd());

    if (algorithm_type_ == 'C' || algorithm_type_ == 'D') {
        // uncorrelated mutation with one step size
        std::mt19937 sigma_mt(rd());
        std::normal_distribution<double> sigma_dist(0, 1);
        noise_sigma_ = noise_sigma_ * std::exp(sigma_tau_correction_ * sigma_dist(sigma_mt));
    } else {
        // Default is decaying sigma
        if (ranked_policies_.size() >= max_ranked_policies_)
            noise_sigma_ *= SIGMA_DECAY_SQUARED;
    }
    std::normal_distribution<double> dist(0, noise_sigma_);

    /// Determine which selection operator to use
    /// Default, for algorithms A and C, is used ten parent crossover
    /// For algorithms B and D, is used two parent crossover with binary tournament selection
    if (ranked_policies_.size() < max_ranked_policies_) {
        // Generate random policy if number of stored policies is less then 'max_ranked_policies_'
        for (size_t i = 0; i < n_actuators; i++) {
            for (size_t j = 0; j < source_y_size; j++) {
                current_policy_->at(i)->at(j) = dist(mt) + .5;
            }
        }
    } else {
        // Generate new policy using weighted crossover operator
        double total_fitness = 0;
        if (algorithm_type_ == 'B' || algorithm_type_ == 'D') {
            // k-selection tournament
            auto parent1 = binarySelection();
            auto parent2 = parent1;
            while (parent2 == parent1) {
                parent2 = binarySelection();
            }

            double fitness1 = parent1->first;
            double fitness2 = parent2->first;

            PolicyPtr policy1 = parent1->second;
            PolicyPtr policy2 = parent2->second;

            // TODO: Verify what should be total fitness in binary
            total_fitness = fitness1 + fitness2;

            // For each spline
            for (size_t i = 0; i < n_actuators; i++) {
                // And for each control point
                for (size_t j = 0; j < source_y_size; j++) {
                    // Apply modifier
                    double param_point = 0;
                    param_point += ((policy1->at(i)->at(j) - current_policy_->at(i)->at(j))) * (fitness1 / total_fitness);
                    param_point += ((policy2->at(i)->at(j) - current_policy_->at(i)->at(j))) * (fitness2 / total_fitness);

                    // Add a mutation + current
                    // TODO: Verify do we use current in this case
                    param_point += dist(mt) + current_policy_->at(i)->at(j);

                    // Set a newly generated point as current
                    current_policy_->at(i)->at(j) = param_point;
                }
            }
        } else {
            // Default is all parents selection

            // Calculate first total sum of fitnesses
            for (auto const &it : ranked_policies_) {
                double fitness = it.first;
                total_fitness += fitness;
            }

            // For each spline
            // TODO: Verify that this should is correct formula
            for (size_t i = 0; i < n_actuators; i++) {
                // And for each control point
                for (size_t j = 0; j < source_y_size; j++) {

                    // Apply modifier
                    double spline_point = 0;
                    for (auto const &it : ranked_policies_) {
                        double fitness = it.first;
                        PolicyPtr policy = it.second;

                        spline_point += ((policy->at(i)->at(j) - current_policy_->at(i)->at(j))) * (fitness / total_fitness);
                    }

                    // Add a mutation + current
                    // TODO: Verify do we use 'current_policy_' in this case
                    spline_point += dist(mt) + current_policy_->at(i)->at(j);

                    // Set a newly generated point as current
                    current_policy_->at(i)->at(j) = spline_point;
                }
            }
        }
    }

    for (size_t i = 0; i < n_actuators; i++) {
        // And for each control point
        cpg::CPGNetwork* cpg = cpgs[i];
        std::vector<cpg::CPGNetwork::Limit> limits = cpg->get_genome_limits();
        for (size_t j = 0; j < source_y_size; j++) {
            cpg::real_t &value = current_policy_->at(i)->at(j);
            cpg::CPGNetwork::Limit limit = limits[j];
//             if (value < limit.lower)
//                 value = limit.lower;
//             else if (value > limit.upper)
//                 value = limit.upper;
            if (value < 0)
                value = 0;
            else if (value > 1)
                value = 1;
        }
    }

    // update the new parameters in the cpgs
    genomeToPhenotype();
}

std::map<double, CPGBrain::PolicyPtr>::iterator CPGBrain::binarySelection() {
    std::random_device rd;
    std::mt19937 umt(rd());
    std::uniform_int_distribution<size_t> udist(0, max_ranked_policies_ - 1);

    // Select two different numbers from uniform distribution U(0, max_ranked_policies_ - 1)
    size_t pindex1, pindex2;
    pindex1 = udist(umt);
    do {
        pindex2 = udist(umt);
    } while (pindex1 == pindex2);

    // Set iterators to begin of the 'ranked_policies_' map
    auto individual1 = ranked_policies_.begin();
    auto individual2 = ranked_policies_.begin();

    // Move iterators to indices positions
    std::advance(individual1, pindex1);
    std::advance(individual2, pindex2);

    double fitness1 = individual1->first;
    double fitness2 = individual2->first;

    return fitness1 > fitness2 ? individual1 : individual2;
}

void CPGBrain::genomeToPhenotype()
{
    // update the new parameters in the cpgs
    for (size_t i=0; i<n_actuators; ++i) {
        GenomePtr genome = current_policy_->at(i);
        cpgs[i]->set_genome(*genome);
    }
}


void CPGBrain::setConnections(std::vector<std::vector<cpg::CPGNetwork::Weights> > connections)
{
    assert(connections.size() == this->connections.size());
    for (size_t i = 0; i < connections.size(); ++i) {
        assert(connections[i].size() == this->connections[i].size());
    }

    this->connections = connections;
    this->connectionsToGenotype();
}

void CPGBrain::connectionsToGenotype()
{
    for (size_t i = 0; i < connections.size(); ++i) {
        GenomePtr genome = current_policy_->at(i);
        const auto &conn_line = connections[i];

        for (size_t j = 0; j < conn_line.size(); ++i) {
            const cpg::CPGNetwork::Weights &connection = conn_line[j];

            // check revolve::brain::cpg::CPGNetwork::update_genome for hardcoded values
            if (j == i) { // self weight
                (*genome)[0] = connection.we;
                (*genome)[4] = connection.wf;
            } else { // connection weight
                (*genome)[12 + 2*j] = connection.we;
                (*genome)[12 + 2*j + 1] = connection.wf;
            }
        }
    }
}


const double CPGBrain::SIGMA_DECAY_SQUARED = 0.98; // sigma decay
