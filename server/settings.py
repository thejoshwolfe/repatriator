import socket
import os, sys

# settings dict.
# name: default
settings = {
    # address to bind the server to
    'HOST': socket.gethostbyname(socket.gethostname()),
    # port to bind the server to
    'PORT': 57051,
    # folder to store data
    'DATA_FOLDER': os.path.join(os.path.dirname(__file__), 'data'),
    # log level: 'DEBUG', 'WARNING', 'ERROR'
    'LOG_LEVEL': 'ERROR',
    # set to true to not actually use the hardware
    'FAKE_MOTOR': False,
    'FAKE_CAMERA_IMAGE_PATH': None,
}
try:
    import config
    for k,v in settings.items():
        if k in config.__dict__:
            settings[k] = config.__dict__[k]
except ImportError:
    pass
