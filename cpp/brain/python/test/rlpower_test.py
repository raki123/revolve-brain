#!/usr/bin/env python3

import random
import revolve_brain_python
import unittest

from actuator_test import Actuator
from sensor_test import Sensor

robot_name = "unit_test_robot"


class RLPowerConf:
    def __init__(self):
        self.algorithm_type = None
        self.evaluation_rate = None
        self.interpolation_spline_size = None
        self.max_evaluations = None
        self.max_ranked_policies = None
        self.noise_sigma = None
        self.sigma_tau_correction = None
        self.source_y_size = None
        self.update_step = None


class Evaluator(revolve_brain_python.Evaluator):
    def __init__(self):
        super().__init__()
        self.i = 0

    def start(self):
        self.i = 0

    def fitness(self):
        self.i += 1
        return random.random()


class RLPowerTestInstance(unittest.TestCase):
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

        self.controller = revolve_brain_python.RLPower(robot_name, RLPowerConf(), self.evaluator, 12, 12)
        self.assertIsInstance(self.controller, revolve_brain_python.RLPower)
        self.assertIsInstance(self.controller, revolve_brain_python.Brain)

    def tearDown(self):
        # Called after the last testfunction was executed
        pass


class RLPowerTestRun(unittest.TestCase):
    def setUp(self):
        # Called before the first testfunction is executed
        self.evaluator = Evaluator()
        self.assertIsInstance(self.evaluator, Evaluator)

        self.actuators = [Actuator() for i in range(0, 2)]
        self.sensors = [Sensor(i) for i in range(0, 2)]

        self.controller = revolve_brain_python.RLPower(
                robot_name,
                RLPowerConf(),
                self.evaluator,
                len(self.actuators) * Actuator.SIZE,
                len(self.sensors) * Sensor.SIZE
        )
        self.assertIsInstance(self.controller, revolve_brain_python.Brain)

    def test_runsOnce(self):
        self.time = 1
        step = 0.1
        self.controller.update(self.actuators, self.sensors, self.time, step)

    def test_runsMultiple(self):
        self.time = 1
        step = 0.1
        import time

        start = time.time()
        for i in range(0, 100000):
            self.controller.update(self.actuators, self.sensors, self.time, step)
            self.time += step
        end = time.time()
        print("made it in {} seconds".format(end - start))

    def tearDown(self):
        # Called after the last testfunction was executed
        pass


if __name__ == "__main__":
    unittest.main()
