import json
import string
import random
import hashlib
import threading
import shutil
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

def _build_path(dirname):
    try:
        os.makedirs(dirname)
        debug("made path %s" % dirname)
    except WindowsError as ex:
        pass

def _save_json():
    _lock.acquire()

    with open(_auth_file, "w") as out:
        out.write(json.dumps(_auth_data))

    _lock.release()

class Privilege:
    OperateHardware = 0
    ManageUsers = 1

class UserAlreadyExists(Exception):
    pass

class UserDoesNotExist(Exception):
    pass

class BadPassword(Exception):
    pass

class User:
    def __init__(self, username=None, password=None, privileges=None):
        """
        creates a new user and returns it, or raises UserAlreadyExists.
        """
        if privileges is None:
            privileges = []

        self.username = username
        self.attrs = {}

        if username is not None or password is not None:
            if username in _auth_data:
                raise UserAlreadyExists
            else:
                self.attrs['salt'] = _random_string(32)
                self.attrs['password_hash'] = _hash_password(self.attrs['salt'], password)
                self.attrs['privileges'] = privileges
                _build_path(self.picture_folder())

    def picture_folder(self):
        if self._picture_folder is None:
            self._picture_folder = os.path.join(
                settings['DATA_FOLDER'],
                hashlib.md5(self.username.encode('utf8')).hexdigest()
            )
        return self._picture_folder

    def grant_privilege(self, privilege):
        privileges = self.attrs['privileges']
        if privileges.find(privilege) == -1:
            privilege.append(privilege)

    def revoke_privilege(self, privilege):
        self.attrs['privileges'].remove(privilege)

    def privileges(self):
        return self.attrs['privileges']

    def has_privilege(self, privilege):
        return privilege in self.attrs['privileges']

    def save(self):
        _auth_data[self.username] = self.attrs
        _save_json()

    def change_password(self, old_password, new_password):
        if _hash_password(self.attrs['salt'], old_password) != self.attrs['password_hash']:
            raise BadPassword
        self.attrs['salt'] = _random_string(32)
        self.attrs['password_hash'] = _hash_password(self.attrs['salt'], new_password)

def get_user(username):
    if username in _auth_data:
        user = User()
        user.username = username
        user.attrs = _auth_data[username]
        return user

    return None

def login(username, password):
    """
    Returns the User object if the authentication succeeded, else None
    """
    user = get_user(username)
    if user is not None and _hash_password(user.attrs['salt'], password) == user.attrs['password_hash']:
        return user
    
    return None

def delete_user(username):
    user = get_user(username)
    if user is None:
        raise UserDoesNotExist

    if os.path.exists(user.picture_folder()):
        shutil.rmtree(user.picture_folder())
    del _auth_data[username]
    _save_json()

def list_users():
    return _auth_data

_auth_file = os.path.join(settings['DATA_FOLDER'], "auth.json")
_build_path(settings['DATA_FOLDER'])
_lock = threading.Lock()

try:
    with open(_auth_file, "r") as f:
        try:
            _auth_data = json.loads(f.read())
        except ValueError as ex:
            error("Corrupt auth data, resetting auth database.")
            raise IOError
except IOError as ex:
    _auth_data = {}
    default_user = User(username="default_admin", password="temp1234", privileges=[Privilege.ManageUsers])
    default_user.save()

