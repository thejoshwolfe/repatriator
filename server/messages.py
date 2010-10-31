from version import version
import struct
import sys
import logging
from logging import debug, warning, error

__all__ = []

__all__.append('ClientMessage')
class ClientMessage:
    DummyCloseConnection = -1
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

    def _move_pointer(self, amount):
        self._bytes_left -= amount
        self._offset += amount

    def _parse_string(self):
        string_len = self._parse_int32()
        if self._bytes_left < string_len:
            raise self.ParseError("Message is missing string at position {0}".format(self._offset))
        offset = self._offset
        self._move_pointer(string_len)
        return self._raw_data[offset:offset+string_len].decode('utf8')

    def _parse_int(self, byte_count, unpack_str):
        if self._bytes_left < byte_count:
            raise self.ParseError("Message is missing int{0} at position {1}".format(byte_count*8, self._offset))
        n = struct.unpack_from(unpack_str, self._raw_data, self._offset)[0]
        self._move_pointer(byte_count)
        return n

    def _parse_int32(self):
        return self._parse_int(4, '>i')

    def _parse_int64(self):
        return self._parse_int(8, '>q')

    def _parse_bool(self):
        return bool(self._parse_int(1, '>b'))

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

        debug("message identified as a " + MessageClass.__name__)

        msg = MessageClass()
        msg._raw_data = raw_data
        msg._bytes_left = len(raw_data) - 9
        msg._offset = len(raw_data) - msg._bytes_left
        msg.message_type = raw_data[0]
        msg.parse()
        return msg

__all__.append('MagicalRequest')
class MagicalRequest(ClientMessage):
    magical_bytes = bytes([0xd1, 0xb6, 0xd7, 0x92, 0x8a, 0xc5, 0x51, 0xa4])

    def parse(self):
        if MagicalRequest.magical_bytes != self._raw_data[self._offset:]:
            warning("Client message magical bytes didn't match up.")
            raise ClientMessage.ParseError("Magical bytes didn't match up.")

__all__.append('ConnectionRequest')
class ConnectionRequest(ClientMessage):
    def parse(self):
        self.version = [0, 0, 0]
        self.version[0] = self._parse_int32()
        self.version[1] = self._parse_int32()
        self.version[2] = self._parse_int32()
        self.username = self._parse_string()
        self.password = self._parse_string()
        self.hardware_flag = self._parse_bool()

__all__.append('TakePicture')
class TakePicture(ClientMessage):
    def parse(self):
        # nothing to do
        pass

__all__.append('MotorMovement')
class MotorMovement(ClientMessage):
    def parse(self):
        self.motor_a = self._parse_int64()
        self.motor_b = self._parse_int64()
        self.motor_x = self._parse_int64()
        self.motor_y = self._parse_int64()
        self.motor_z = self._parse_int64()

__all__.append('DirectoryListingRequest')
class DirectoryListingRequest(ClientMessage):
    def parse(self):
        # nothing to do
        pass

__all__.append('FileDownloadRequest')
class FileDownloadRequest(ClientMessage):
    def parse(self):
        self.filename = self._parse_string()

__all__.append('FileDeleteRequest')
class FileDeleteRequest(ClientMessage):
    def parse(self):
        self.filename = self._parse_string()

__all__.append('AddUser')
class AddUser(ClientMessage):
    def parse(self):
        self.username = self._parse_string()
        self.password = self._parse_string()

        privilege_count = self._parse_int32()
        self.privileges = set()
        for _ in range(privilege_count):
            self.privileges.add(self._parse_int32())

__all__.append('UpdateUser')
class UpdateUser(ClientMessage):
    def parse(self):
        self.username = self._parse_string()
        self.password = self._parse_string()

        privilege_count = self._parse_int32()
        self.privileges = set()
        for _ in range(privilege_count):
            self.privileges.add(self._parse_int32())

__all__.append('DeleteUser')
class DeleteUser(ClientMessage):
    def parse(self):
        self.username = self._parse_string()

__all__.append('ServerMessage')
class ServerMessage:
    DummyCloseConnection = -1
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
        out = bytearray()
        out.extend(struct.pack('>b', self.message_type))
        out.extend(struct.pack('>q', message_length))
        out.extend(buf)
        return out

__all__.append('DummyCloseConnection')
class DummyCloseConnection:
    message_type = ServerMessage.DummyCloseConnection

__all__.append('MagicalResponse')
class MagicalResponse(ServerMessage):
    def __init__(self):
        self.message_type = ServerMessage.MagicalResponse

    def _serialize(self):
        return bytes([0xb5, 0xac, 0x71, 0x2a, 0x08, 0x3d, 0xe5, 0x07])

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
        major, minor, revision = version

        buf = bytearray()
        buf.extend(struct.pack(">i", major))
        buf.extend(struct.pack(">i", minor))
        buf.extend(struct.pack(">i", revision))
        buf.extend(struct.pack(">i", self.status))
        buf.extend(struct.pack(">i", len(self.privileges)))
        for privilege in self.privileges:
            buf.extend(struct.pack(">i", privilege))

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
        buf.extend(struct.pack(">q", self.motor_a_pos))
        buf.extend(struct.pack(">q", self.motor_b_pos))
        buf.extend(struct.pack(">q", self.motor_x_pos))
        buf.extend(struct.pack(">q", self.motor_y_pos))
        buf.extend(struct.pack(">q", self.motor_z_pos))
        buf.extend(struct.pack(">q", len(self.jpeg)))
        buf.extend(self.jpeg)
        return buf

__all__.append('DirectoryListingResult')
class DirectoryListingResult(ServerMessage):
    def __init__(self, file_list):
        self.file_list = file_list

    def _serialize(self):
        buf = bytearray()
        buf.extend(struct.pack(">i", len(self.file_list)))
        for file_path in self.file_list:
            path, filename = os.path.split(file_path)
            buf.extend(struct.pack(">i", len(filename)))
            buf.extend(filename.encode('utf8'))

            try:
                thumb_size = os.path.getsize(file_path+'.thumb')
                f = open(file_path, 'rb')
                buf.extend(struct.pack(">q", thumb_size))
                buf.extend(f.read())
                f.close()
            except (OSError, IOError):
                buf.extend(struct.pack(">q", 0))
                error("error accessing thumbnail for {0}".file_path)
            
        return buf

__all__.append('FileDownloadResult')
class FileDownloadResult(ServerMessage):
    def __init__(self, file_path):
        self.file_path = file_path

    def _serialize(self):
        buf = bytearray()
        try:
            file_size = os.path.getsize(self.file_path)
            f = open(self.file_path)
            buf.extend(file_size)
            buf.extend(f.read())
            f.close()
        except (OSError, IOError):
            buf.extend(struct.pack(">q", 0))

        return buf

__all__.append('ErrorMessage')
class ErrorMessage(ServerMessage):
    NotAuthorized = 0
    HardwareInUse = 1
    FileDoesNotExist = 2
    UserAlreadyExists = 3
    UserDoesNotExist = 4
    OverwritingOtherUser = 5
    InvalidFilename = 6

    descriptions = {
        NotAuthorized: "Not authorized to perform this operation.",
        HardwareInUse: "The hardware is already in use - wait till someone logs off.",
        FileDoesNotExist: "The file does not exist.",
        UserAlreadyExists: "The user you are trying to add already exists.",
        UserDoesNotExist: "The user you are trying to update does not exist.",
        OverwritingOtherUser: "You're trying to change a username in a way that would overwrite another user.",
        InvalidFilename: "The filename you supplied is invalid.",
    }

    def __init__(self, code, description=None):
        self.code = code
        if description is None:
            self.description = self.descriptions[code]
        else:
            self.description = description

    def _serialize(self):
        buf = bytearray()
        buf.extend(struct.pack(">i", self.code))
        buf.extend(struct.pack(">i", len(self.description)))
        buf.extend(self.description.encode('utf8'))
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

