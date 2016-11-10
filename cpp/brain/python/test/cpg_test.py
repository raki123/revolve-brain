#!/usr/bin/env python3

import revolve_brain_python
from sensor_test import Sensor
from actuator_test import Actuator

import unittest
import random

robot_name = "test_robot"

class Evaluator(revolve_brain_python.Evaluator):
    def __init__(self):
        super().__init__()
        self.i = 0

    def start(self):
        self.i = 0

    def fitness(self):
        self.i += 1
        return random.random()

class CPGTestInstance(unittest.TestCase):

    def test_evaluator(self):
        self.evaluator = Evaluator()
        self.assertIsInstance(self.evaluator, Evaluator)
        self.assertIsInstance(self.evaluator, revolve_brain_python.Evaluator)

        self.evaluator.start()
        self.evaluator.fitness()

    def test_isInstantiated(self):
        self.evaluator = Evaluator()

        self.controller = revolve_brain_python.CPGBrain(robot_name, self.evaluator, 12, 12)
        self.assertIsInstance(self.controller, revolve_brain_python.CPGBrain)
        self.assertIsInstance(self.controller, revolve_brain_python.Brain)

class CPGTestRun(unittest.TestCase):
    def setUp(self):
        # Called before the first testfunction is executed
        self.evaluator = Evaluator()
        self.assertIsInstance(self.evaluator, Evaluator)

        self.actuators = [Actuator() for i in range(0,2)]
        self.sensors = [Sensor(i) for i in range(0,2)]

        self.controller = revolve_brain_python.CPGBrain(
            robot_name,
            self.evaluator,
            12,
            12,
        )
        self.assertIsInstance(self.controller, revolve_brain_python.Brain)

    def test_runsOnce(self):
        self.time = 1
        step = 0.1
        self.controller.update(self.actuators, self.sensors, self.time, step)

    #def test_runsMultiple(self):
        #self.time = 1
        #step = 0.1
        #import time

        #start = time.time()
        #for i in range(0,100000):
            #self.controller.update(self.actuators, self.sensors, self.time, step)
            #self.time += step
        #end = time.time()
        #print("made it in {} seconds".format(end - start))

if __name__ == "__main__":
    unittest.main()
