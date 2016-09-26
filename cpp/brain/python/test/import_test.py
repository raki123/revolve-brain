#!/usr/bin/env python3

import unittest
import revolve_brain_python

class ImportTest(unittest.TestCase):
    def test_import(self):
        self.assertIsNotNone(revolve_brain_python.Evaluator)
        self.assertIsNotNone(revolve_brain_python.Brain)
        self.assertIsNotNone(revolve_brain_python.NeuralNetwork)
        self.assertIsNotNone(revolve_brain_python.RLPower)


if __name__ == "__main__":
    unittest.main()
