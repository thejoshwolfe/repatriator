import json
import string
import random
import hashlib
import threading

def _random_string(length):
    """
    returns a string of length length with random alphanumeric characters
    """
    chars = string.letters + string.digits
    code = ""
    for _ in range(length):
        code += chars[random.randint(0, len(chars)-1)]
    return code

def _hash_password(salt, password):
    return hashlib.sha256(salt + password).hexdigest()

def _save_json():
    _lock.acquire()

    _out = open("auth.json", "r")
    json.dumps(_auth_data)
    _out.close()

    _lock.release()

class Privilege:
    OperateHardware = 0
    ManageUsers = 1

class UserAlreadyExists:
    pass

class User:
    def __init__(self, username=None, password=None, privileges=set()):
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

_lock = threading.Lock()

try:
    _in = open("auth.json", "r")
    _auth_data = json.loads(_in.read())
    _in.close()
except IOError as ex:
    _auth_data = {}
    default_user = User(username="default_admin", password="temp1234", privileges=set(Privilege.ManageUsers))
    default_user.save()
