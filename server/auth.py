import json
import string
import random
import hashlib
import threading
from settings import settings
import os, sys
from logging import debug, warning, error

def _random_string(length):
    """
    returns a string of length length with random alphanumeric characters
    """
    chars = string.ascii_letters + string.digits
    code = ""
    for _ in range(length):
        code += chars[random.randint(0, len(chars)-1)]
    return code

def _hash_password(salt, password):
    encoded = (salt + password).encode('utf8')
    return hashlib.sha256(encoded).hexdigest()

def _build_path(filename):
    path, name = os.path.split(filename)
    try:
        os.makedirs(path)
    except WindowsError as ex:
        pass

def _sets_to_lists():
    "convert the sets in auth data to lists"
    for account in _auth_data.values():
        account['privileges'] = list(account['privileges'])

def _lists_to_sets():
    "convert the lists in auth data to sets"
    for account in _auth_data.values():
        account['privileges'] = set(account['privileges'])

def _save_json():
    _lock.acquire()
    _sets_to_lists()

    _out = open(_auth_file, "w")
    json.dumps(_auth_data)
    _out.close()

    _lists_to_sets()
    _lock.release()

class Privilege:
    OperateHardware = 0
    ManageUsers = 1

class UserAlreadyExists(Exception):
    pass

class User:
    def __init__(self, username=None, password=None, privileges=None):
        if privileges is None:
            privileges = set()

        self.attrs = {}
        self.username = username

        if username is not None or password is not None:
            if username in _auth_data:
                raise UserAlreadyExists
            else:
                self.attrs['salt'] = _random_string(32)
                self.attrs['password_hash'] = _hash_password(self.attrs['salt'], password)
                self.attrs['privileges'] = privileges

    def grant_privilege(self, privilege):
        self.attrs['privileges'].add(privilege)

    def revoke_privilege(self, privilege):
        self.attrs['privileges'].remove(privilege)

    def privileges(self):
        return self.attrs['privileges']

    def has_privilege(self, privilege):
        return privilege in self.attrs['privileges']

    def save(self):
        _auth_data[self.username] = self.attrs
        _save_json()

def login(username, password):
    """
    Returns the User object if the authentication succeeded, else None
    """
    if username in _auth_data:
        if _hash_password(_auth_data[username]['salt'], password) == _auth_data[username]['password_hash']:
            user = User()
            user.username = username
            user.attrs = _auth_data[username]
    
    return None

_auth_file = os.path.join(settings['DATA_FOLDER'], "auth.json")
_build_path(_auth_file)
_lock = threading.Lock()

try:
    _in = open(_auth_file, "r")
    try:
        _auth_data = json.loads(_in.read())
        # need to convert the lists in json to sets
        _lists_to_sets()
    except ValueError as ex:
        error("Corrupt auth data, resetting auth database.")
        _in.close()
        raise IOError
    _in.close()
except IOError as ex:
    _auth_data = {}
    default_user = User(username="default_admin", password="temp1234", privileges={Privilege.ManageUsers})
    default_user.save()
