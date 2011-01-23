import socket
import os, sys

# settings dict.
# name: default
# use a file called config.py to override these settings
settings = {
    # address to bind the server to
    'HOST': socket.gethostbyname(socket.gethostname()),
    # port to bind the server to
    'PORT': 57051,
    # folder to store data
    'DATA_FOLDER': os.path.join(os.path.dirname(__file__), 'data'),
    # log level: 'DEBUG', 'WARNING', 'ERROR'
    'LOG_LEVEL': 'ERROR',
    # set to a jpeg image path to turn on fake camera
    'FAKE_CAMERA_IMAGE_PATH': None,

    # motor config. You'll need to set these in config.py
    'MOTOR_A_BAUD_RATE': 9600,
    'MOTOR_A_VELOCITY': 300000,
    'MOTOR_A_ACCELERATION': 500,
    'MOTOR_A_DRIVER_ADDRESS': 9,    # used to identify the motor on the serial port
    'MOTOR_A_ENABLE_LIMIT_SWITCHES': False,
    'MOTOR_A_ENABLE_POSITION_CORRECTION': False,
    'MOTOR_A_SET_HOLDING_CURRENT': True,
    'MOTOR_A_FAKE': False,          # use debug motor which does not use real hardware

    'MOTOR_B_BAUD_RATE': 9600,
    'MOTOR_B_VELOCITY': 30000,
    'MOTOR_B_ACCELERATION': 50,
    'MOTOR_B_DRIVER_ADDRESS': 7,
    'MOTOR_B_ENABLE_LIMIT_SWITCHES': True,
    'MOTOR_B_ENABLE_POSITION_CORRECTION': False,
    'MOTOR_B_SET_HOLDING_CURRENT': False,
    'MOTOR_B_FAKE': False,

    'MOTOR_X_BAUD_RATE': 9600,
    'MOTOR_X_VELOCITY': 30000,
    'MOTOR_X_ACCELERATION': 50,
    'MOTOR_X_DRIVER_ADDRESS': 3,
    'MOTOR_X_ENABLE_LIMIT_SWITCHES': True,
    'MOTOR_X_ENABLE_POSITION_CORRECTION': True,
    'MOTOR_X_SET_HOLDING_CURRENT': True,
    'MOTOR_X_FAKE': False,

    'MOTOR_Y_BAUD_RATE': 9600,
    'MOTOR_Y_VELOCITY': 30000,
    'MOTOR_Y_ACCELERATION': 50,
    'MOTOR_Y_DRIVER_ADDRESS': 5,
    'MOTOR_Y_ENABLE_LIMIT_SWITCHES': True,
    'MOTOR_Y_ENABLE_POSITION_CORRECTION': True,
    'MOTOR_Y_SET_HOLDING_CURRENT': True,
    'MOTOR_Y_FAKE': False,

    'MOTOR_Z_BAUD_RATE': 9600,
    'MOTOR_Z_VELOCITY': 30000,
    'MOTOR_Z_ACCELERATION': 50,
    'MOTOR_Z_DRIVER_ADDRESS': 1,
    'MOTOR_Z_ENABLE_LIMIT_SWITCHES': True,
    'MOTOR_Z_ENABLE_POSITION_CORRECTION': True,
    'MOTOR_Z_SET_HOLDING_CURRENT': True,
    'MOTOR_Z_FAKE': False,

    # how many seconds to wait for a message from the client
    # before giving up and closing the connection
    'CLIENT_IDLE_TIMEOUT': 15,
    # motors never take longer than this number of seconds to make one motion
    'MOTOR_MOVEMENT_TIMEOUT': 60,
}
try:
    import config
    for k,v in settings.items():
        if k in config.__dict__:
            settings[k] = config.__dict__[k]
except ImportError:
    pass
