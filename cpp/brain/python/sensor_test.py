#!/usr/bin/env python3

import unittest
import revolve_brain_python

class Sensor(revolve_brain_python.Sensor):
    def __init__(self):
        super().__init__()
        self.i = 0

    #void read(double * input_vector)
    def read(self, input_vector):
        input_vector[0] = self.i
        self.i += 1

    #virtual unsigned int inputs() const
    def inputs(self):
        return 1


class SensorTest(unittest.TestCase):
    def setUp(self):
        # Called before the first testfunction is executed
        pass

    def test_isInstantiated(self):
        self.sensor = Sensor()
        self.assertIsInstance(self.sensor, Sensor)
        self.assertIsInstance(self.sensor, revolve_brain_python.Sensor)

    def test_methods(self):
        self.sensor = Sensor()
        self.assertEqual(self.sensor.inputs(), 1)
        self.assertEqual(self.sensor.inputs(), 1)

        results = [-100]
        for i in range(0,100):
            self.sensor.read(results)
            self.assertEqual(results[0], i)

    def tearDown(self):
        # Called after the last testfunction was executed
        pass

if __name__ == "__main__":
    unittest.main()
