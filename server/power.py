import logging
from logging import debug, warning, error

def set_power_switch(on):
    """
    if on is True, turns the power strip on.
    if on is False, turns the power strip off.
    """
    status = {True: 'on', False: 'off'}
    warning("(fake) turning {0} power strip".format(status[on]))
