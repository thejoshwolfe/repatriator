import logging
from logging import debug, warning, error
import powerusb
import sys

if powerusb.initialize():
    debug("Initialized PowerUSB")
else
    error("Fatal: Unable to initialize PowerUSB")
    sys.exit(-1)

def set_power_switch(on):
    """
    if on is True, turns the power strip on.
    if on is False, turns the power strip off.
    """
    status = {True: 'on', False: 'off'}[on]
    debug("turning {0} power strip".format(status))
    powerusb.set_state(on, on)
