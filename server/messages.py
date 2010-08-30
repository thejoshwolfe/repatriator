from version import version
import struct

class ClientMessage:
    MagicalRequest = 0
    ConnectionRequest = 1
    TakePicture = 2
    MotorMovement = 3
    DirectoryListingRequest = 4
    FileDownloadRequest = 5
    AddUser = 6
    UpdateUser = 7
    DeleteUser = 8

    TypeForId = {
        ClientMessage.MagicalRequest: MagicalRequest,
        ClientMessage.ConnectionRequest: ConnectionRequest,
        ClientMessage.TakePicture: TakePicture,
        ClientMessage.MotorMovement: MotorMovement,
        ClientMessage.DirectoryListingRequest: DirectoryListingRequest,
        ClientMessage.FileDownloadRequest: FileDownloadRequest,
        ClientMessage.AddUser: AddUser,
        ClientMessage.UpdateUser: UpdateUser,
        ClientMessage.DeleteUser: DeleteUser,
    }

    class ParseError(Exception):
        pass

    @staticmethod
    def parse(raw_data):
        """
        raw_data is a byte array
        returns a subclass of ClientMessage with the relevant data.
        """
        if len(raw_data) < 9:
            raise ClientMessage.ParseError("Message is missing header data.")

        MessageClass = ClientMessage.TypeForId[raw_data[0]]
        msg_length = struct.unpack(raw_data[1:9], ">q")

        if msg_length != len(raw_data):
            raise ClientMessage.ParseError("Message length value is wrong.")

        return MessageClass(raw_data[9:], msg_length-9)

class MagicalRequest(ClientMessage):
    magical_bytes = [0xd1, 0xb6, 0xd7, 0x92, 0x8a, 0xc5, 0x51, 0xa4]

    def __init__(self, data):
        if MagicalRequest.magical_bytes != data:
            raise ClientMessage.ParseError("Magical bytes didn't match up.")

class ConnectionRequest(ClientMessage):
    def __init__(self, data):
        bytes_left = len(data)
        offset = len(data) - bytes_left

        def move_pointer(amount):
            bytes_left -= offset
            offset += amount

        #int8 - major version number of client
        #int8 - minor version number of client
        #int8 - revision version number of client
        #int8 - reserved
        if bytes_left < 4:
            raise ClientMessage.ParseError("Message is missing version data.")
        self.version = [x for x in data[offset:offset+3]]
        if data[offset+3] != 0:
            raise ClientMessage.ParseError("Reserved field should be zero.")
        move_pointer(4)

        #int32 - length of utf8 username string below
        #<utf8 username string>
        if bytes_left < 4:
            raise ClientMessage.ParseError("Message is missing username data.")
        username_len = struct.unpack(data[4:8], ">i")
        move_pointer(4)
        if bytes_left < username_len:
            raise ClientMessage.ParseError("Message is missing username string.")
        self.username = data[offset:offset+username_len].decode('utf8')
        move_pointer(username_len)
        
        #int32 - length of utf8 password string below
        #<utf8 password string>
        if bytes_left < 4:
            raise ClientMessage.ParseError("Message is missing password data.")
        password_len = struct.unpack(data[4:8], ">i")
        move_pointer(4)
        if bytes_left < password_len:
            raise ClientMessage.ParseError("Message is missing password string.")
        self.password = data[offset:offset+password_len].decode('utf8')
        move_pointer(password_len)

        #int8 - boolean - do you want the hardware to turn on? False if you're only doing admin.
        if bytes_left < 1:
            raise ClientMessage.ParseError("Message is missing admin flag.")
        self.admin_flag = data[offset]

class TakePicture(ClientMessage):
    def __init__(self, data, size):
        pass
class MotorMovement(ClientMessage):
    def __init__(self, data, size):
        pass
class DirectoryListingRequest(ClientMessage):
    def __init__(self, data, size):
        pass
class FileDownloadRequest(ClientMessage):
    def __init__(self, data, size):
        pass
class AddUser(ClientMessage):
    def __init__(self, data, size):
        pass
class UpdateUser(ClientMessage):
    def __init__(self, data, size):
        pass
class DeleteUser(ClientMessage):
    def __init__(self, data, size):
        pass

class ServerMessage:
    MagicalResponse = 0
    ConnectionResult = 1
    FullUpdate = 2
    DirectoryListingResult = 3
    FileDownloadResult = 4

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

class MagicalResponse(Message):
    def _serialize(self):
        return bytearray([0xd1, 0xb6, 0xd7, 0x92, 0x8a, 0xc5, 0x51, 0xa4])

class ConnectionResult(Message):
    InvalidLogin = 0 
    # for example, requested operating hardware but don't have Privilege.OperateHardware
    InsufficientPrivileges = 1 
    Success = 2

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
