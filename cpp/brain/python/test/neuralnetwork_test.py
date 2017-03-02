#!/usr/bin/env python3

import revolve_brain_python
import unittest


class NeuralNetworkTest(unittest.TestCase):
    def setUp(self):
        # Called before the first testfunction is executed
        pass

    def test_isInstantiated(self):
        self.nn = revolve_brain_python.NeuralNetwork()
        self.assertIsInstance(self.nn, revolve_brain_python.NeuralNetwork)
        self.assertIsInstance(self.nn, revolve_brain_python.Brain)

    def tearDown(self):
        # Called after the last testfunction was executed
        pass


if __name__ == "__main__":
    unittest.main()
