#!/usr/bin/env python3

import unittest
import revolve_brain_python

class Evaluator(revolve_brain_python.Evaluator):
    def __init__(self):
        super().__init__()
        self.i = 0

    def start(self):
        self.i = 0

    def fitness(self):
        self.i += 1
        return self.i

class RLPowerTest(unittest.TestCase):
    def setUp(self):
        # Called before the first testfunction is executed
        pass

    def test_evaluator(self):
        self.evaluator = Evaluator()
        self.assertIsInstance(self.evaluator, Evaluator)
        self.assertIsInstance(self.evaluator, revolve_brain_python.Evaluator)

        self.evaluator.start()
        self.evaluator.fitness()

    def test_isInstantiated(self):
        self.evaluator = Evaluator()

        self.controller = revolve_brain_python.RLPower(self.evaluator, 12, 12)
        self.assertIsInstance(self.controller, revolve_brain_python.RLPower)
        self.assertIsInstance(self.controller, revolve_brain_python.Brain)

    def tearDown(self):
        # Called after the last testfunction was executed
        pass

if __name__ == "__main__":
    unittest.main()
