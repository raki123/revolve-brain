#ifndef REVOLVE_NEAT_TEST_ASYNCNEAT_H_
#define REVOLVE_NEAT_TEST_ASYNCNEAT_H_

/**
 * Test class for AsyncNeat class
 */
class TestAsyncNeat
{
public:
    TestAsyncNeat();

    ~TestAsyncNeat();

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

#endif // REVOLVE_NEAT_TEST_ASYNCNEAT_H_
