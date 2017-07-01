from setuptools import setup, find_packages

# Left for later inclusion
# requires = [
#    'revolve-communication',
#    'revolve-hal'
# ]

# check links for native code bits of the library
# https://docs.python.org/3.5/extending/building.html#distributing-your-extension-modules
# http://robotics.usc.edu/~ampereir/wordpress/?p=202

setup(
    name='revolve-brain',
    version='1.0',
    description='revolve-brain',
    author='Matteo De Carlo',
    author_email='matteo.dek@gmail.com',
    packages=find_packages(),
    install_requires='', # requires,
    entry_points={
        # TODO: Add endpoints to main() in control_servos.py and main.py
        'console_scripts': [
        ]
    }
)
