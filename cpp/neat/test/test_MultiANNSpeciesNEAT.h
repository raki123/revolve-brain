#ifndef REVOLVE_NEAT_TEST_MULTIANNSPECIESNEAT_H_
#define REVOLVE_NEAT_TEST_MULTIANNSPECIESNEAT_H_

class TestMultiNNSpeciesNeat
{
public:
    TestMultiNNSpeciesNeat();

    ~TestMultiNNSpeciesNeat();

    /**
     * Runs all tests. Returns false if one of the tests fails.
     */
    bool
    test();

private:
    /**
     * test if the algorithm is able to resolve the XOR problem
     */
    bool
    testXOR();

    const int MAX_EVALUATIONS = 9999;
};

#endif // REVOLVE_NEAT_TEST_MULTIANNSPECIESNEAT_H_
