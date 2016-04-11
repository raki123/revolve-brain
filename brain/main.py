import logging
import signal

from robot_brain import RobotBrain

# TODO: Rename this file to something that actually makes sense

# Hint: Don't claim authorship when your code needs 60% refactoring
__author__ = 'matteo'
logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)
logger.setFormatter(logging.Formatter(
    ("%(asctime)-15s:%(levelname)-8s:%(threadName)s:"
     "%(filename)s:%(funcName)s:%(message)s")))

INTERRUPT_MESSAGE = "Press 'Q' to exit, ENTER to continue."

controller = None  # Keeping this global is bad, consider making a class


def noop_interrupt_handler(signum, frame):
    print("\n" + INTERRUPT_MESSAGE)


def interrupt_handler(signum, frame):
    signal.signal(signal.SIGINT, noop_interrupt_handler)
    logging.info("changing evaluation")
    command = input(INTERRUPT_MESSAGE + "\n")
    if command.lower() == 'q':
        controller.suicide()
    controller.stop_current_evaluation()
    signal.signal(signal.SIGINT, interrupt_handler)


def main(config):
    # TODO: Arguments should be handled with sys.argv
    logger.info("Starting robot controller...")

    # catch signal to interrupt current evaluation
    signal.signal(signal.SIGINT, interrupt_handler)

    controller = RobotBrain(config)
    controller.live()

    logging.info("Terminating robot controller...")
