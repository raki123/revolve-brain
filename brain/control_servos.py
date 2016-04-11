import logging
from hal.outputs.servo import Servo

logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)


def center_servo(pin):
    servo = Servo(pin)
    servo.center()
    return servo


def off_servo(pin):
    servo = Servo(pin)
    servo.off()
    return servo


COMMANDS = {
    'center': center_servo,
    'off': off_servo
}


def main(command, pins):
    # TODO: Arguments should be handled with sys.argv
    if not command or pins:
        logger.error('Incorrect usage.')
        logger.error('Proper usage is: {} [pins...]'.format(list(COMMANDS)))
        return

    try:
        operation = COMMANDS[command]
    except KeyError:
        logger.error('Command %s not available.')
        logger.error('Available commands: {}'.format(list(COMMANDS)))

    for pin in pins:
        operation(int(pin))
