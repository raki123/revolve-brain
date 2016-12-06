#ifndef NEAT_GENE_H_
#define NEAT_GENE_H_


//class containg meta information about a gene
namespace CPPNEAT {
class Gene {
public:
	enum GeneType {
		NEURON_GENE,
		CONNECTION_GENE
	};
	Gene(int innov_number = 0, bool enabled = true) : innov_number(innov_number), enabled(enabled) {}
	virtual inline int getInnovNumber() { return innov_number; }
	virtual inline bool isEnabled() { return enabled; }
	GeneType gene_type;
private:
	int innov_number;
	bool enabled;

}; 
}

#endif //NEAT_GENE_H_