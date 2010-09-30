from version import version
import struct
import sys
import logging
from logging import debug, warning, error

__all__ = []

__all__.append('ClientMessage')
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
    FileDeleteRequest = 9

    class ParseError(Exception):
        pass

    @staticmethod
    def parse(raw_data):
        """
        raw_data is a byte array
        returns a subclass of ClientMessage with the relevant data.
        """
        debug("Incoming client message. raw data:")
        debug("----------------------------------")
        debug(raw_data)
        debug("----------------------------------")

        if len(raw_data) < 9:
            warning("Client message is missing header data")
            raise ClientMessage.ParseError("Message is missing header data.")

        MessageClass = ClientMessage.TypeForId[raw_data[0]]
        msg_length = struct.unpack_from(">q", raw_data, 1)[0]
        debug("Message length: " + str(msg_length) + ", actual length: " + str(len(raw_data)))

        if msg_length != len(raw_data):
            warning("Client message length value is wrong")
            raise ClientMessage.ParseError("Message length value is wrong.")

        msg = MessageClass(raw_data[9:])
        msg.message_type = raw_data[0]
        debug("message identified as a " + str(msg))
        return msg

__all__.append('MagicalRequest')
class MagicalRequest(ClientMessage):
    magical_bytes = bytes([0xd1, 0xb6, 0xd7, 0x92, 0x8a, 0xc5, 0x51, 0xa4])

    def __init__(self, data):
        if MagicalRequest.magical_bytes != data:
            warning("Client message magical bytes didn't match up.")
            raise ClientMessage.ParseError("Magical bytes didn't match up.")

__all__.append('ConnectionRequest')
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
        username_len = struct.unpack_from(">i", data, 4)[0]
        move_pointer(4)
        if bytes_left < username_len:
            raise ClientMessage.ParseError("Message is missing username string.")
        self.username = data[offset:offset+username_len].decode('utf8')
        move_pointer(username_len)
        
        #int32 - length of utf8 password string below
        #<utf8 password string>
        if bytes_left < 4:
            raise ClientMessage.ParseError("Message is missing password data.")
        password_len = struct.unpack_from(">i", data, 4)[0]
        move_pointer(4)
        if bytes_left < password_len:
            raise ClientMessage.ParseError("Message is missing password string.")
        self.password = data[offset:offset+password_len].decode('utf8')
        move_pointer(password_len)

        #int8 - boolean - do you want the hardware to turn on? False if you're only doing admin.
        if bytes_left < 1:
            raise ClientMessage.ParseError("Message is missing admin flag.")
        self.admin_flag = data[offset]

__all__.append('TakePicture')
class TakePicture(ClientMessage):
    def __init__(self, data, size):
        pass
__all__.append('MotorMovement')
class MotorMovement(ClientMessage):
    def __init__(self, data, size):
        pass
__all__.append('DirectoryListingRequest')
class DirectoryListingRequest(ClientMessage):
    def __init__(self, data, size):
        pass
__all__.append('FileDownloadRequest')
class FileDownloadRequest(ClientMessage):
    def __init__(self, data, size):
        pass
__all__.append('FileDeleteRequest')
class FileDeleteRequest(ClientMessage):
    def __init__(self, data, size):
        pass
__all__.append('AddUser')
class AddUser(ClientMessage):
    def __init__(self, data, size):
        pass
__all__.append('UpdateUser')
class UpdateUser(ClientMessage):
    def __init__(self, data, size):
        pass
__all__.append('DeleteUser')
class DeleteUser(ClientMessage):
    def __init__(self, data, size):
        pass

__all__.append('ServerMessage')
class ServerMessage:
    MagicalResponse = 0
    ConnectionResult = 1
    FullUpdate = 2
    DirectoryListingResult = 3
    FileDownloadResult = 4
    ErrorMessage = 5

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

__all__.append('MagicalResponse')
class MagicalResponse(ServerMessage):
    def __init__(self,):
        self.message_type = ServerMessage.MagicalResponse

    def _serialize(self):
        return bytes([0xd1, 0xb6, 0xd7, 0x92, 0x8a, 0xc5, 0x51, 0xa4])

__all__.append('ConnectionResult')
class ConnectionResult(ServerMessage):
    InvalidLogin = 0 
    # for example, requested operating hardware but don't have Privilege.OperateHardware
    InsufficientPrivileges = 1 
    Success = 2

    def __init__(self, status, privileges=None):
        if privileges is None:
            privileges = set()

        self.message_type = ServerMessage.ConnectionResult
        self.privileges = privileges
        self.status = status
        
    def _serialize(self):
        magic = bytes([0xb5, 0xac, 0x71, 0x2a, 0x08, 0x3d, 0xe5, 0x07])
        major, minor, revision = version

        buf = bytearray()
        buf.append(magic)
        buf.append(struct.pack(">i", major))
        buf.append(struct.pack(">i", minor))
        buf.append(struct.pack(">i", revision))
        buf.append(struct.pack(">i", self.status))
        buf.append(struct.pack(">i", len(self.privileges)))
        for privilege in self.privileges:
            buf.append(struct.pack(">i", privilege))

        return buf

__all__.append('FullUpdate')
class FullUpdate(ServerMessage):
    def __init__(self, camera, motor_a_pos=0, motor_b_pos=0, motor_x_pos=0, motor_y_pos=0, motor_z_pos=0):
        self.message_type = ServerMessage.FullUpdate
        self.motor_a_pos = motor_a_pos
        self.motor_b_pos = motor_b_pos
        self.motor_x_pos = motor_x_pos
        self.motor_y_pos = motor_y_pos
        self.motor_z_pos = motor_z_pos

        frame_buffer = camera.liveViewMemoryView()
        self.jpeg = bytearray()

        # get live view frame
        if frame_buffer[0] != b'\xff' or frame_buffer[1] != b'\xd8':
            raise IOError("bad image data: invalid header")

        ff_flag = False
        for b in frame_buffer:
            self.jpeg.append(ord(b))
            if b == b'\xff':
                ff_flag = True
            elif b == b'\xd9' and ff_flag:
                break
            else:
                ff_flag = False
        else:
            self.jpeg = bytearray()
            raise IOError("bad image data: could not find ff flag")

    def _serialize(self):
        buf = bytearray()
        #int64 - position of motor A
        buf.append(struct.pack(">q", self.motor_a_pos))
        #int64 - position of motor B
        buf.append(struct.pack(">q", self.motor_b_pos))
        #int64 - position of motor X
        buf.append(struct.pack(">q", self.motor_x_pos))
        #int64 - position of motor Y
        buf.append(struct.pack(">q", self.motor_y_pos))
        #int64 - position of motor Z
        buf.append(struct.pack(">q", self.motor_z_pos))
        #int64 - byte count for following JPEG image
        buf.append(struct.pack(">q", len(self.jpeg)))
        #<jpeg format image>
        buf.append(self.jpeg)
        return buf

__all__.append('DirectoryListingResult')
class DirectoryListingResult(ServerMessage):
    pass

__all__.append('FileDownloadResult')
class FileDownloadResult(ServerMessage):
    pass

__all__.append('ErrorMessage')
class ErrorMessage(ServerMessage):
    NotAuthorized = 0

    descriptions = {
        NotAuthorized: "Not authorized to perform this operation",
    }

    def __init__(self, code, description=None):
        self.code = code
        if description is None:
            self.description = self.descriptions[code]
        else:
            self.description = description

    def _serialize(self):
        buf = bytearray()
        buf.append(struct.pack(">i", self.code))
        buf.append(struct.pack(">i", len(self.description)))
        buf.append(self.description.encode())
        return buf


ClientMessage.TypeForId = {
    ClientMessage.MagicalRequest: MagicalRequest,
    ClientMessage.ConnectionRequest: ConnectionRequest,
    ClientMessage.TakePicture: TakePicture,
    ClientMessage.MotorMovement: MotorMovement,
    ClientMessage.DirectoryListingRequest: DirectoryListingRequest,
    ClientMessage.FileDownloadRequest: FileDownloadRequest,
    ClientMessage.AddUser: AddUser,
    ClientMessage.UpdateUser: UpdateUser,
    ClientMessage.DeleteUser: DeleteUser,
    ClientMessage.FileDeleteRequest: FileDeleteRequest,
}

