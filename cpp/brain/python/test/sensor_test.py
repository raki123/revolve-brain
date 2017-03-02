#!/usr/bin/env python3

import revolve_brain_python
import unittest


class Sensor(revolve_brain_python.Sensor):
    SIZE = 4

    def __init__(self, id):
        super().__init__()
        self.i = 0
        self.id = id

    # void read(double * input_vector)
    def read(self, input_vector):
        for i in range(0, self.SIZE):
            input_vector[i] = self.i
            self.i += 1

    # virtual unsigned int inputs() const
    def inputs(self):
        return self.SIZE

    def sensorId(self):
        return "sensor[{}]".format(self.id)


class SensorTest(unittest.TestCase):
    def setUp(self):
        # Called before the first testfunction is executed
        pass

    def test_isInstantiated(self):
        self.sensor = Sensor(4)
        self.assertIsInstance(self.sensor, Sensor)
        self.assertIsInstance(self.sensor, revolve_brain_python.Sensor)

    def test_methods(self):
        self.sensor = Sensor(5)
        self.assertEqual(self.sensor.inputs(), Sensor.SIZE)
        self.assertEqual(self.sensor.inputs(), Sensor.SIZE)

        results = [-100 for i in range(0, Sensor.SIZE)]
        for i in range(0, 100):
            self.sensor.read(results)
            for j in range(0, Sensor.SIZE):
                self.assertEqual(results[j], i * Sensor.SIZE + j)

    def tearDown(self):
        # Called after the last testfunction was executed
        pass


if __name__ == "__main__":
    unittest.main()
