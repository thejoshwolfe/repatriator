import socket
import os

# settings dict.
# name: default
settings = {
    # address to bind the server to
    'HOST': socket.gethostbyname(socket.gethostname()),
    # port to bind the server to
    'PORT': 57051,
    # folder to store data
    'DATA_FOLDER': os.path.join(os.path.dirname(__file__), 'data'),
}
try:
    import config
    for k,v in settings.items():
        if k in config.__dict__:
            settings[k] = config.__dict__[k]
except ImportError:
    pass
