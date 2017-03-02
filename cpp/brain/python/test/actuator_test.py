#!/usr/bin/env python3

import revolve_brain_python
import unittest


class Actuator(revolve_brain_python.Actuator):
    SIZE = 4

    def __init__(self):
        super().__init__()
        self.i = 0

    # virtual void update(double *output_vector, double step)
    def update(self, output_vector, step):
        # print("Advancing of step: {}".format(step))
        variable = None
        for i in range(0, self.SIZE):
            # print("Motor {} value: {}".format(i, output_vector[i]))
            variable = output_vector[i]

    # virtual unsigned int outputs() const
    def outputs(self):
        return self.SIZE


class ActuatorTest(unittest.TestCase):
    def setUp(self):
        # Called before the first testfunction is executed
        pass

    def test_isInstantiated(self):
        self.actuator = Actuator()
        self.assertIsInstance(self.actuator, Actuator)
        self.assertIsInstance(self.actuator, revolve_brain_python.Actuator)

    def test_methods(self):
        self.actuator = Actuator()
        self.assertEqual(self.actuator.outputs(), Actuator.SIZE)
        self.assertEqual(self.actuator.outputs(), Actuator.SIZE)

        results = [i for i in range(0, self.actuator.SIZE)]
        for i in range(0, 100):
            self.actuator.update(results, i)

    def tearDown(self):
        # Called after the last testfunction was executed
        pass


if __name__ == "__main__":
    unittest.main()
