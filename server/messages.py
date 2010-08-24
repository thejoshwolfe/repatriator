from version import version
import struct

class Message:
    ConnectionResult = 0
    FullUpdate = 1
    DirectoryListingResult = 2
    FileDownloadResult = 3

    ConnectionRequest = 100
    TakePicture = 101
    MotorMovement = 102
    DirectoryListingRequest = 103
    FileDownloadRequest = 104
    AddUser = 105
    UpdateUser = 106
    DeleteUser = 107

    def serialize(self):
        """
        converts message to a byte stream so that you can send it.
        calls child object's _serialize to get the message data.
        returns a bytearray.
        """
        buf = self._serialize()
        message_length = 1 + 8 + len(buf)
        buf.insert(struct.pack('>q', message_length))
        buf.insert(struct.pack('>b', self.message_type))
        return buf


class ConnectionResult(Message):
    InvalidLogin = 0 
    # for example, requested operating hardware but don't have Privilege.OperateHardware
    InsufficientPrivileges = 1 

    Success = 100

    def __init__(self, status, privileges=set())
        self.message_type = Message.ConnectionResult
        
    def _serialize(self):
        magic = bytes([0xb5, 0xac, 0x71, 0x2a, 0x08, 0x3d, 0xe5, 0x07])
        major, minor, revision = version

        buf = bytearray()
        buf.append(magic)
        buf.append(struct.pack(">b", major))
        buf.append(struct.pack(">b", minor))
        buf.append(struct.pack(">b", revision))
        buf.append(struct.pack(">b", 0))
        buf.append(struct.pack(">i", status))
        buf.append(struct.pack(">i", len(privileges)))
        for privilege in privileges:
            buf.append(struct.pack(">i", privilege))

        return buf

class FullUpdate(Message):
    pass
class DirectoryListingResult(Message):
    pass
class FileDownloadResult(Message):
    pass

class ConnectionRequest(Message):
    pass
class TakePicture(Message):
    pass
class MotorMovement(Message):
    pass
class DirectoryListingRequest(Message):
    pass
class FileDownloadRequest(Message):
    pass
class AddUser(Message):
    pass
class UpdateUser(Message):
    pass
class DeleteUser(Message):
    pass
