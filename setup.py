from distutils.core import setup, find_packages

requires = [
    'revolve-communication',
    'revolve-hal'
]

# check links for native code bits of the library
# https://docs.python.org/3.5/extending/building.html#distributing-your-extension-modules
# http://robotics.usc.edu/~ampereir/wordpress/?p=202

setup(
    name='revolve-mating-server',
    version='1.0',
    description='revolve-mating-server',
    author='Panagiotis Eustratiadis',
    author_email='peustratiadis@gmail.com',
    packages=find_packages(),
    install_requires=requires,
    entry_points={
        # TODO: Add endpoints to main() in control_servos.py and main.py
        'console_scripts': [
        ]
    }
)
