import logging
from logging import debug, warning, error
import powerusb
import sys

try:
    powerusb.initialize():
    debug("Initialized PowerUSB")
except powerusb.error:
    error("Unable to initialize PowerUSB")

def set_power_switch(on):
    """
    if on is True, turns the power strip on.
    if on is False, turns the power strip off.
    """
    status = {True: 'on', False: 'off'}[on]
    debug("turning {0} power strip".format(status))
    try:
        powerusb.set_state(on, on)
    except powerusb.error:
        error("Unable to set PowerUSB state.")
