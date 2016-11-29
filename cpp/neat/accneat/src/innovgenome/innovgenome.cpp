/*
  Copyright 2001 The University of Texas at Austin

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/
#include "innovgenome.h"
#include "protoinnovlinkgene.h"
#include "recurrencychecker.h"
#include "util/util.h"
#include <assert.h>
#include <cstring>

#include "brain/extnn/extended_neural_network.h"

using namespace NEAT;
using namespace std;

void InnovGenome::reset() {
    traits.clear();
    nodes.clear();
    links.clear();
}

InnovGenome::InnovGenome()
    : node_lookup(nodes) {
}

InnovGenome::InnovGenome(rng_t rng_,
                         size_t ntraits,
                         size_t ninputs,
                         size_t noutputs,
                         size_t nhidden)
    : InnovGenome() {

    rng = rng_;
    ntraits = noutputs + nhidden;
    for(size_t i = 0; i < ntraits; i++) {
        traits.emplace_back(i + 1,
                            rng.prob(),
                            rng.prob(),
                            rng.prob(),
                            rng.prob(),
                            rng.prob(),
                            rng.prob(),
                            rng.prob(),
                            rng.prob(),
                            rng.prob());
    }

    {
        int node_id = 1;

        //Bias node
        add_node(nodes, InnovNodeGene(NT_HIDDEN, node_id++, BIAS));
	nodes[nodes.size()-1].set_trait_id(node_id);

        //Sensor nodes
        for(size_t i = 0; i < ninputs; i++) {
            add_node(nodes, InnovNodeGene(NT_SENSOR, node_id++, INPUT));
        }

        //Output nodes
        for(size_t i = 0; i < noutputs; i++) {
            add_node(nodes, InnovNodeGene(NT_OUTPUT, node_id++, SIGMOID));
	    nodes[nodes.size()-1].set_trait_id(node_id);
        }

        //Hidden nodes
        for(size_t i = 0; i < nhidden; i++) {
            add_node(nodes, InnovNodeGene(NT_HIDDEN, node_id++, SIGMOID));
	    nodes[nodes.size()-1].set_trait_id(node_id);
        }
    }

    const int node_id_bias = 1;
    const int node_id_input = node_id_bias + 1;
    const int node_id_output = node_id_input + ninputs;
    const int node_id_hidden = node_id_output + noutputs;

    assert(nhidden > 0);

    int innov = 1;

    //Create links from Bias to all hidden
    for(size_t i = 0; i < nhidden; i++) {
        add_link( links, InnovLinkGene(rng.element(traits).trait_id,
                                       rng.prob(),
                                       node_id_bias,
                                       i + node_id_hidden,
                                       false,
                                       innov++,
                                       0.0) );
    }

    //Create links from all inputs to all hidden
    for(size_t i = 0; i < ninputs; i++) {
        for(size_t j = 0; j < nhidden; j++) {
            add_link( links, InnovLinkGene(rng.element(traits).trait_id,
                                           rng.prob(),
                                           i + node_id_input,
                                           j + node_id_hidden,
                                           false,
                                           innov++,
                                           0.0));
        }
    }

    //Create links from all hidden to all output
    for(size_t i = 0; i < nhidden; i++) {
        for(size_t j = 0; j < noutputs; j++) {
            add_link( links, InnovLinkGene(rng.element(traits).trait_id,
                                           rng.prob(),
                                           i + node_id_hidden,
                                           j + node_id_output,
                                           false,
                                           innov++,
                                           0.0));
        }
    }
}

Genome &InnovGenome::operator=(const Genome &other) {
    return *this = dynamic_cast<const InnovGenome &>(other);
}

InnovGenome::~InnovGenome() {
}

void InnovGenome::verify() {
#ifdef NDEBUG
    return;
#else

    //Check for InnovNodeGenes being out of order
    for(size_t i = 1, n = nodes.size(); i < n; i++) {
        assert( nodes[i-1].node_id < nodes[i].node_id );
    }

    {
        //Check links reference valid nodes.
        for(InnovLinkGene &gene: links) {
            assert( get_node(gene.in_node_id()) );
            assert( get_node(gene.out_node_id()) );
        }
    }

    //Make sure there are no duplicate genes
    for(InnovLinkGene &gene: links) {
        for(InnovLinkGene &gene2: links) {
            if(&gene != &gene2) {
                assert( (gene.is_recurrent() != gene2.is_recurrent())
                        || (gene2.in_node_id() != gene.in_node_id())
                        || (gene2.out_node_id() != gene.out_node_id()) );
            }
        }
    }
#endif
}

Genome::Stats InnovGenome::get_stats() {
    return {nodes.size(), links.size()};
}



int InnovGenome::get_last_node_id() {
    return nodes.back().node_id + 1;
}

real_t InnovGenome::get_last_gene_innovnum() {
    return links.back().innovation_num + 1;
}

void InnovGenome::duplicate_into(InnovGenome *offspring) const {
    offspring->traits = traits;
    offspring->links = links;
    offspring->nodes = nodes;
}

InnovGenome &InnovGenome::operator=(const InnovGenome &other) {
    rng = other.rng;
    genome_id = other.genome_id;
    nodes = other.nodes;
    links = other.links;
    return *this;
}

void InnovGenome::mutate_random_trait() {
    rng.element(traits).mutate(rng);
}

void InnovGenome::mutate_link_weights(real_t power,real_t rate,mutator mut_type) {
    //Go through all the InnovLinkGenes and perturb their link's weights

    real_t num = 0.0; //counts gene placement
    real_t gene_total = (real_t)links.size();
    real_t endpart = gene_total*0.8; //Signifies the last part of the genome
    real_t powermod = 1.0; //Modified power by gene number
    //The power of mutation will rise farther into the genome
    //on the theory that the older genes are more fit since
    //they have stood the test of time

    bool severe = rng.prob() > 0.5;  //Once in a while really shake things up

    //Loop on all links  (ORIGINAL METHOD)
    for(InnovLinkGene &gene: links) {

        //The following if determines the probabilities of doing cold gaussian
        //mutation, meaning the probability of replacing a link weight with
        //another, entirely random weight.  It is meant to bias such mutations
        //to the tail of a genome, because that is where less time-tested links
        //reside.  The gausspoint and coldgausspoint represent values above
        //which a random float will signify that kind of mutation.

        //Don't mutate weights of frozen links
        if (!(gene.frozen)) {
            real_t gausspoint;
            real_t coldgausspoint;

            if (severe) {
                gausspoint=0.3;
                coldgausspoint=0.1;
            }
            else if ((gene_total>=10.0)&&(num>endpart)) {
                gausspoint=0.5;  //Mutate by modification % of connections
                coldgausspoint=0.3; //Mutate the rest by replacement % of the time
            }
            else {
                //Half the time don't do any cold mutations
                if (rng.prob()>0.5) {
                    gausspoint=1.0-rate;
                    coldgausspoint=1.0-rate-0.1;
                }
                else {
                    gausspoint=1.0-rate;
                    coldgausspoint=1.0-rate;
                }
            }

            //Possible methods of setting the perturbation:
            real_t randnum = rng.posneg()*rng.prob()*power*powermod;
            if (mut_type==GAUSSIAN) {
                real_t randchoice = rng.prob();
                if (randchoice > gausspoint)
                    gene.weight()+=randnum;
                else if (randchoice > coldgausspoint)
                    gene.weight()=randnum;
            }
            else if (mut_type==COLDGAUSSIAN)
                gene.weight()=randnum;

            //Cap the weights at 8.0 (experimental)
            if (gene.weight() > 8.0) gene.weight() = 8.0;
            else if (gene.weight() < -8.0) gene.weight() = -8.0;

            //Record the innovation
            gene.mutation_num = gene.weight();

            num+=1.0;
        }

    } //end for loop
}

void InnovGenome::add_link(vector<InnovLinkGene> &llist, const InnovLinkGene &l) {
    auto it = std::upper_bound(llist.begin(), llist.end(), l, linklist_cmp);
    llist.insert(it, l);
}

void InnovGenome::add_node(vector<InnovNodeGene> &nlist, const InnovNodeGene &n) {
    auto it = std::upper_bound(nlist.begin(), nlist.end(), n, nodelist_cmp);
    nlist.insert(it, n);
}

void InnovGenome::mate(InnovGenome *genome1,
                       InnovGenome *genome2,
                       InnovGenome *offspring,
                       real_t fitness1,
                       real_t fitness2) {

        InnovGenome::mate_multipoint(genome1,
                                     genome2,
                                     offspring,
                                     fitness1,
                                     fitness2);

}

// todo: use NodeLookup for newnodes instead of linear search!
void InnovGenome::mate_multipoint(InnovGenome *genome1,
                                  InnovGenome *genome2,
                                  InnovGenome *offspring,
                                  real_t fitness1,
                                  real_t fitness2) {
    rng_t &rng = offspring->rng;
    vector<InnovLinkGene> &links1 = genome1->links;
    vector<InnovLinkGene> &links2 = genome2->links;

    //The baby InnovGenome will contain these new Traits, InnovNodeGenes, and InnovLinkGenes
    offspring->reset();
    vector<Trait> &newtraits = offspring->traits;
    vector<InnovNodeGene> &newnodes = offspring->nodes;
    vector<InnovLinkGene> &newlinks = offspring->links;

    vector<InnovLinkGene>::iterator curgene2;  //Checks for link duplication

    //iterators for moving through the two parents' traits
    vector<Trait*>::iterator p1trait;
    vector<Trait*>::iterator p2trait;

    //iterators for moving through the two parents' links
    vector<InnovLinkGene>::iterator p1gene;
    vector<InnovLinkGene>::iterator p2gene;
    real_t p1innov;  //Innovation numbers for links inside parents' InnovGenomes
    real_t p2innov;
    vector<InnovNodeGene>::iterator curnode;  //For checking if InnovNodeGenes exist already

    bool disable;  //Set to true if we want to disabled a chosen gene

    disable=false;
    InnovLinkGene newgene;

    bool p1better; //Tells if the first genome (this one) has better fitness or not

    bool skip;

    //First, average the Traits from the 2 parents to form the baby's Traits
    //It is assumed that trait lists are the same length
    //In the future, may decide on a different method for trait mating
    assert(genome1->traits.size() == genome2->traits.size());
    for(size_t i = 0, n = genome1->traits.size(); i < n; i++) {
        newtraits.emplace_back(genome1->traits[i], genome2->traits[i]);
    }

    //Figure out which genome is better
    //The worse genome should not be allowed to add extra structural baggage
    //If they are the same, use the smaller one's disjoint and excess genes only
    if (fitness1>fitness2)
        p1better=true;
    else if (fitness1==fitness2) {
        if (links1.size()<(links2.size()))
            p1better=true;
        else p1better=false;
    }
    else
        p1better=false;

    //Make sure all sensors and outputs are included
    for(InnovNodeGene &node: genome1->nodes) {
        if(node.type != NT_HIDDEN) {
            //Add the new node
            add_node(newnodes, node);
        } else {
            break;
        }
    }

    //Now move through the InnovLinkGenes of each parent until both genomes end
    p1gene = links1.begin();
    p2gene = links2.begin();
    while( !((p1gene==links1.end()) && (p2gene==(links2).end())) ) {
        ProtoInnovLinkGene protogene;

        skip=false;  //Default to not skipping a chosen gene

        if (p1gene==links1.end()) {
            protogene.set_gene(genome2, &*p2gene);
            ++p2gene;
            if (p1better) skip=true;  //Skip excess from the worse genome
        } else if (p2gene==(links2).end()) {
            protogene.set_gene(genome1, &*p1gene);
            ++p1gene;
            if (!p1better) skip=true; //Skip excess from the worse genome
        } else {
            //Extract current innovation numbers
            p1innov=p1gene->innovation_num;
            p2innov=p2gene->innovation_num;

            if (p1innov==p2innov) {
                if (rng.prob()<0.5) {
                    protogene.set_gene(genome1, &*p1gene);
                } else {
                    protogene.set_gene(genome2, &*p2gene);
                }

                //If one is disabled, the corresponding gene in the offspring
                //will likely be disabled
                if (((p1gene->enable)==false)||
                    ((p2gene->enable)==false))
                    if (rng.prob()<0.75) disable=true;

                ++p1gene;
                ++p2gene;
            } else if (p1innov < p2innov) {
                protogene.set_gene(genome1, &*p1gene);
                ++p1gene;

                if (!p1better) skip=true;

            } else if (p2innov<p1innov) {
                protogene.set_gene(genome2, &*p2gene);
                ++p2gene;

                if (p1better) skip=true;
            }
        }

        //Check to see if the protogene conflicts with an already chosen gene
        //i.e. do they represent the same link
        curgene2=newlinks.begin();
        while ((curgene2!=newlinks.end())&&
               (!((curgene2->in_node_id()==protogene.gene()->in_node_id())&&
                  (curgene2->out_node_id()==protogene.gene()->out_node_id())&&(curgene2->is_recurrent()== protogene.gene()->is_recurrent()) ))&&
               (!((curgene2->in_node_id()==protogene.gene()->out_node_id())&&
                  (curgene2->out_node_id()==protogene.gene()->in_node_id())&&
                  (!(curgene2->is_recurrent()))&&
                  (!(protogene.gene()->is_recurrent())) )))
        {
            ++curgene2;
        }

        if (curgene2!=newlinks.end()) skip=true;  //Links conflicts, abort adding

        if (!skip) {
            //Now add the gene to the baby
            InnovNodeGene new_inode;
            InnovNodeGene new_onode;

            //Next check for the nodes, add them if not in the baby InnovGenome already
            InnovNodeGene *inode = protogene.in();
            InnovNodeGene *onode = protogene.out();

            //Check for inode in the newnodes list
            if (inode->node_id<onode->node_id) {
                //inode before onode

                //Checking for inode's existence
                curnode=newnodes.begin();
                while((curnode!=newnodes.end())&&
                      (curnode->node_id!=inode->node_id))
                    ++curnode;

                if (curnode==newnodes.end()) {
                    //Here we know the node doesn't exist so we have to add it
                    new_inode = *inode;
                    add_node(newnodes,new_inode);

                }
                else {
                    new_inode=*curnode;

                }

                //Checking for onode's existence
                curnode=newnodes.begin();
                while((curnode!=newnodes.end())&&
                      (curnode->node_id!=onode->node_id))
                    ++curnode;
                if (curnode==newnodes.end()) {
                    //Here we know the node doesn't exist so we have to add it
                    new_onode = *onode;
                    add_node(newnodes,new_onode);

                }
                else {
                    new_onode=*curnode;
                }

            }
            //If the onode has a higher id than the inode we want to add it first
            else {
                //Checking for onode's existence
                curnode=newnodes.begin();
                while((curnode!=newnodes.end())&&
                      (curnode->node_id!=onode->node_id))
                    ++curnode;
                if (curnode==newnodes.end()) {
                    //Here we know the node doesn't exist so we have to add it
                    new_onode = *onode;
                    //newnodes.push_back(new_onode);
                    add_node(newnodes,new_onode);

                }
                else {
                    new_onode=*curnode;

                }

                //Checking for inode's existence
                curnode=newnodes.begin();
                while((curnode!=newnodes.end())&&
                      (curnode->node_id!=inode->node_id))
                    ++curnode;
                if (curnode==newnodes.end()) {
                    //Here we know the node doesn't exist so we have to add it
                    new_inode = *inode;
                    add_node(newnodes,new_inode);
                }
                else {
                    new_inode=*curnode;

                }

            } //End InnovNodeGene checking section- InnovNodeGenes are now in new InnovGenome

            //Add the InnovLinkGene
            newgene = InnovLinkGene(protogene.gene(),
                                    protogene.gene()->trait_id(),
                                    new_inode.node_id,
                                    new_onode.node_id);
            if (disable) {
                newgene.enable=false;
                disable=false;
            }
            newlinks.push_back(newgene);
        }

    }
}

real_t InnovGenome::compatibility(InnovGenome *g) {
    vector<InnovLinkGene> &links1 = this->links;
    vector<InnovLinkGene> &links2 = g->links;


    //Innovation numbers
    real_t p1innov;
    real_t p2innov;

    //Intermediate value
    real_t mut_diff;

    //Set up the counters
    real_t num_disjoint=0.0;
    real_t num_excess=0.0;
    real_t mut_diff_total=0.0;
    real_t num_matching=0.0;  //Used to normalize mutation_num differences

    //Now move through the InnovLinkGenes of each potential parent
    //until both InnovGenomes end
    vector<InnovLinkGene>::iterator p1gene = links1.begin();
    vector<InnovLinkGene>::iterator p2gene = links2.begin();

    while(!((p1gene==links1.end())&&
            (p2gene==links2.end()))) {

        if (p1gene==links1.end()) {
            ++p2gene;
            num_excess+=1.0;
        }
        else if (p2gene==links2.end()) {
            ++p1gene;
            num_excess+=1.0;
        }
        else {
            //Extract current innovation numbers
            p1innov = p1gene->innovation_num;
            p2innov = p2gene->innovation_num;

            if (p1innov==p2innov) {
                num_matching+=1.0;
                mut_diff = p1gene->mutation_num - p2gene->mutation_num;
                if (mut_diff<0.0) mut_diff=0.0-mut_diff;
                mut_diff_total+=mut_diff;

                ++p1gene;
                ++p2gene;
            }
            else if (p1innov<p2innov) {
                ++p1gene;
                num_disjoint+=1.0;
            }
            else if (p2innov<p1innov) {
                ++p2gene;
                num_disjoint+=1.0;
            }
        }
    } //End while

    //Return the compatibility number using compatibility formula
    //Note that mut_diff_total/num_matching gives the AVERAGE
    //difference between mutation_nums for any two matching InnovLinkGenes
    //in the InnovGenome

    //Normalizing for genome size
    //return (disjoint_coeff*(num_disjoint/max_genome_size)+
    //  excess_coeff*(num_excess/max_genome_size)+
    //  mutdiff_coeff*(mut_diff_total/num_matching));


    //Look at disjointedness and excess in the absolute (ignoring size)

    return (env->disjoint_coeff*(num_disjoint/1.0)+
            env->excess_coeff*(num_excess/1.0)+
            env->mutdiff_coeff*(mut_diff_total/num_matching));
}

real_t InnovGenome::trait_compare(Trait *t1,Trait *t2) {

    int id1=t1->trait_id;
    int id2=t2->trait_id;
    int count;
    real_t params_diff=0.0; //Measures parameter difference

    //See if traits represent different fundamental types of connections
    if ((id1==1)&&(id2>=2)) {
        return 0.5;
    }
    else if ((id2==1)&&(id1>=2)) {
        return 0.5;
    }
    //Otherwise, when types are same, compare the actual parameters
    else {
        if (id1>=2) {
            for (count=0;count<=2;count++) {
                params_diff += fabs(t1->params[count]-t2->params[count]);
            }
            return params_diff/4.0;
        }
        else return 0.0; //For type 1, params are not applicable
    }

}

void InnovGenome::randomize_traits() {
    for(InnovNodeGene &node: nodes) {
        node.set_trait_id(1 + rng.index(traits));
    }

    for(InnovLinkGene &gene: links) {
        gene.set_trait_id(1 + rng.index(traits));
    }
}

inline Trait &get_trait(vector<Trait> &traits, int trait_id) {
    Trait &t = traits[trait_id - 1];
    assert(t.trait_id == trait_id);
    return t;
}

Trait &InnovGenome::get_trait(const InnovNodeGene &node) {
    return ::get_trait(traits, node.get_trait_id());
}

Trait &InnovGenome::get_trait(const InnovLinkGene &gene) {
    return ::get_trait(traits, gene.trait_id());
}

void InnovGenome::init_phenotype(revolve::brain::ExtNNController::ExtNNConfig &config) {
    size_t nnodes = nodes.size();
    assert(nnodes <= NODES_MAX);

    config.numHiddenNeurons_ = 0;
    config.numInputNeurons_ = 0;
    config.numOutputNeurons_ = 0;
    for(size_t i = 0; i < nnodes; i++) {
        InnovNodeGene &node = nodes[i];
	std::string neuronId = std::to_string(i);
	std::map<std::string, double> params;
	//get parameters from trait of neuron
	if(traits.size() != 0) {
	Trait neuronParams = traits[node.get_trait_id()];
	params["rv:bias"] = neuronParams.params[0];
	params["rv:tau"] = neuronParams.params[1];
	params["rv:gain"] = neuronParams.params[2];
	params["rv:period"] = neuronParams.params[3];
	params["rv:phase_offset"] = neuronParams.params[4];
	params["rv:amplitude"] = neuronParams.params[5];
	params["rv:alpha"] = neuronParams.params[6];
	params["rv:energy"] = neuronParams.params[7];
	} else {
	  	params["rv:bias"] =1;
	params["rv:tau"] = 1;
	params["rv:gain"] = 1;
	params["rv:period"] = 1;
	params["rv:phase_offset"] = 1;
	params["rv:amplitude"] = 1;
	params["rv:alpha"] = 1;
	params["rv:energy"] = 1;
	}
	revolve::brain::NeuronPtr newNeuron; 
// 	std::cout << neuronType + " " + neuronId  + " was added in"+ " "+ neuronLayer << std::endl;
	switch(node.type) {
	    case NT_SENSOR: {
	    	newNeuron.reset(new revolve::brain::InputNeuron(neuronId, params));

		config.inputNeurons_.push_back(newNeuron);
		config.inputPositionMap_[newNeuron] = i;
		config.numInputNeurons_++;
		break;
	    }
	    case NT_HIDDEN: {
		switch(node.neuron_type) {
		     case SIGMOID: {
			newNeuron.reset(new revolve::brain::SigmoidNeuron(neuronId, params));
			break;
		     }
		     case SIMPLE: {
			newNeuron.reset(new revolve::brain::LinearNeuron(neuronId, params));
			break;
		     }
		     case BIAS:{
			newNeuron.reset(new revolve::brain::BiasNeuron(neuronId, params));
			break;
		     }
		     case DIFFERENTIAL_CPG: {
		     	newNeuron.reset(new revolve::brain::DifferentialCPG(neuronId, params));
			break;
		     }
		     default: {
		       	    std::cout << "i get here 7.311" << std::endl;
			throw std::runtime_error("Robot brain error"); 
		     }
		}
		config.hiddenNeurons_.push_back(newNeuron);
		config.numHiddenNeurons_++;
		break;
	    }
	    case NT_OUTPUT: {
// 		std::cout << (node.neuron_type == INPUT?"input":"wtf") << std::endl;
		switch(node.neuron_type) {
		     case SIGMOID: {
			newNeuron.reset(new revolve::brain::SigmoidNeuron(neuronId, params));
			break;
		     }
		     case SIMPLE: {
			newNeuron.reset(new revolve::brain::LinearNeuron(neuronId, params));
			break;
		     }
		     case BIAS:{
			newNeuron.reset(new revolve::brain::BiasNeuron(neuronId, params));
			break;
		     }
		     case DIFFERENTIAL_CPG: {
		     	newNeuron.reset(new revolve::brain::DifferentialCPG(neuronId, params));
			break;
		     }
		     default: {
		       std::cout << node.neuron_type << " " << BIAS << " " << INPUT << " " << SIGMOID << " " << SIMPLE << " " << DIFFERENTIAL_CPG << std:: endl;
		       	    std::cout << "i get here 7.312" << std::endl;
			throw std::runtime_error("Robot brain error"); 
		     }
		}
		    config.outputNeurons_.push_back(newNeuron);
		    config.outputPositionMap_[newNeuron] = i;
		    config.numOutputNeurons_++;
		break;
	    }
	    default: {
		  std::cout << "i get here 7.313" << std::endl;
		throw std::runtime_error("programmer is idiot error"); 
	    }
	}
	config.allNeurons_.push_back(newNeuron);
	config.idToNeuron_[neuronId] = newNeuron;
    }
    std::cout << config.numInputNeurons_ << std::endl;
    std::cout << config.numOutputNeurons_ << std::endl;
    
    config.inputs_ = new double[config.numInputNeurons_];
    config.outputs_ = new double[config.numOutputNeurons_];
	    std::cout << "i get here 7.32 " << config.inputs_ << " " << config.inputs_[0] << std::endl;
    size_t nlinks = 0;
    for(InnovLinkGene &link: links) {
        if(link.enable) {
	    nlinks++;
	    revolve::brain::NeuronPtr dst = config.allNeurons_[get_node_index(link.out_node_id())];
	    revolve::brain::NeuralConnectionPtr newConnection(new revolve::brain::NeuralConnection(
		config.allNeurons_[get_node_index(link.in_node_id())],
		dst,
		link.weight()));
	    // Add reference to this connection to the destination neuron
	    dst->AddIncomingConnection(dst->GetSocketId(), newConnection);
	    config.connections_.push_back(newConnection);
        }
    }
	    std::cout << "i get here 7.33" << std::endl;
    assert(nlinks <= LINKS_MAX);
    
}
void InnovGenome::init_phenotype(Network &net) {
    size_t nnodes = nodes.size();
    assert(nnodes <= NODES_MAX);

    //---
    //--- Count how many of each type of node.
    //---
    NetDims dims;
    std::memset(&dims, 0, sizeof(dims));

    for(size_t i = 0; i < nnodes; i++) {
        InnovNodeGene &node = nodes[i];

        switch(node.type) {
        case NT_BIAS:
            dims.nnodes.bias++;
            break;
        case NT_SENSOR:
            dims.nnodes.sensor++;
            break;
        case NT_OUTPUT:
            dims.nnodes.output++;
            break;
        case NT_HIDDEN:
            dims.nnodes.hidden++;
            break;
        default:
            panic();
        }
    }
    dims.nnodes.all = nnodes;
    dims.nnodes.input = dims.nnodes.bias + dims.nnodes.sensor;
    dims.nnodes.noninput = dims.nnodes.output + dims.nnodes.hidden;

    //---
    //--- Create unsorted array of links, converting node ID to index in process.
    //---
    NetLink netlinks[links.size()];
    size_t nlinks = 0;
    size_t node_nlinks[nnodes];
    memset(node_nlinks, 0, sizeof(size_t) * nnodes);

    for(InnovLinkGene &link: links) {
        if(link.enable) {
            NetLink &netlink = netlinks[nlinks++];

            netlink.weight = link.weight();
            netlink.in_node_index = get_node_index(link.in_node_id());
            netlink.out_node_index = get_node_index(link.out_node_id());

            node_nlinks[netlink.out_node_index]++;
        }
    }
    assert(nlinks <= LINKS_MAX);

    dims.nlinks = nlinks;

    //---
    //--- Determine layout of links for each node in sorted array
    //---
    NetNode netnodes[nnodes];
    netnodes[0].incoming_start = 0;
    netnodes[0].incoming_end = node_nlinks[0];
    for(size_t i = 1; i < nnodes; i++) {
        NetNode &prev = netnodes[i-1];
        NetNode &curr = netnodes[i];

        curr.incoming_start = prev.incoming_end;
        curr.incoming_end = curr.incoming_start + node_nlinks[i];
    }
    assert(netnodes[nnodes - 1].incoming_end == nlinks);

    //---
    //--- Create sorted links
    //---
    memset(node_nlinks, 0, sizeof(size_t) * nnodes);
    NetLink netlinks_sorted[nlinks];
    for(size_t i = 0; i < nlinks; i++) {
        NetLink &netlink = netlinks[i];
        size_t inode = netlink.out_node_index;
        size_t isorted = netnodes[inode].incoming_start + node_nlinks[inode]++;
        netlinks_sorted[isorted] = netlink;
    }

    //---
    //--- Configure the net
    //---
    net.configure(dims, netnodes, netlinks_sorted);
}

InnovLinkGene *InnovGenome::find_link(int in_node_id, int out_node_id, bool is_recurrent) {
    for(InnovLinkGene &g: links) {
        if( (g.in_node_id() == in_node_id)
            && (g.out_node_id() == out_node_id)
            && (g.is_recurrent() == is_recurrent) ) {

            return &g;
        }
    }

    return nullptr;
}

InnovNodeGene *InnovGenome::get_node(int id) {
    return node_lookup.find(id);
}

node_size_t InnovGenome::get_node_index(int id) {
    node_size_t i = get_node(id) - nodes.data();
    assert(nodes[i].node_id == id);
    return i;
}

void InnovGenome::print(std::ostream &out) {
    out<<"genomestart "<<genome_id<<std::endl;

    //Output the traits
    for(auto &t: traits)
        t.print_to_file(out);

    //Output the nodes
    for(auto &n: nodes)
        n.print_to_file(out);

    //Output the genes
    for(auto &g: links)
        g.print_to_file(out);

    out << "genomeend " << genome_id << std::endl;
}