from version import version
import struct
import os, sys
import logging
from logging import debug, warning, error

__all__ = []

__all__.append('ClientMessage')
class ClientMessage:
    DummyAutoFocus = -2
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
    ChangePasswordRequest = 10
    ListUserRequest = 11
    SetAutoFocusEnabled = 12
    Ping = 13
    SetStaticBookmarks = 14
    SetUserBookmarks = 15

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

    def _parse_bool(self):
        return bool(self._parse_int8())

    def _parse_int8(self):
        return self._parse_int(1, '>b')

    def _parse_int32(self):
        return self._parse_int(4, '>i')

    def _parse_int64(self):
        return self._parse_int(8, '>q')

    @staticmethod
    def parse(raw_data):
        """
        raw_data is a byte array
        returns a subclass of ClientMessage with the relevant data.
        """
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

__all__.append('DummyAutoFocus')
class DummyAutoFocus():
    message_type = ClientMessage.DummyAutoFocus

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
        self.newest_protocol_supported = self._parse_int32()
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
        self.motor_pos = {}
        self.motor_pos['A'] = self._parse_int64()
        self.motor_pos['B'] = self._parse_int64()
        self.motor_pos['X'] = self._parse_int64()
        self.motor_pos['Y'] = self._parse_int64()
        self.motor_pos['Z'] = self._parse_int64()

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

__all__.append('ChangePasswordRequest')
class ChangePasswordRequest(ClientMessage):
    def parse(self):
        self.old_password = self._parse_string()
        self.new_password = self._parse_string()

__all__.append('AddUser')
class AddUser(ClientMessage):
    def parse(self):
        self.username = self._parse_string()
        self.password = self._parse_string()

        privilege_count = self._parse_int32()
        self.privileges = []
        for _ in range(privilege_count):
            self.privileges.append(self._parse_int32())

__all__.append('UpdateUser')
class UpdateUser(ClientMessage):
    def parse(self):
        self.username = self._parse_string()
        self.password = self._parse_string()

        privilege_count = self._parse_int32()
        self.privileges = []
        for _ in range(privilege_count):
            self.privileges.append(self._parse_int32())

__all__.append('DeleteUser')
class DeleteUser(ClientMessage):
    def parse(self):
        self.username = self._parse_string()

__all__.append('ListUserRequest')
class ListUserRequest(ClientMessage):
    def parse(self):
        # nothing to do
        pass

__all__.append('SetAutoFocusEnabled')
class SetAutoFocusEnabled(ClientMessage):
    def parse(self):
        self.value = self._parse_bool()

__all__.append('Ping')
class Ping(ClientMessage):
    def parse(self):
        self.ping_id = self._parse_int32()

__all__.append('SetStaticBookmarks')
class SetStaticBookmarks(ClientMessage):
    def parse(self):
        self.bookmarks = []
        bookmarks_len = self._parse_int32()
        for _ in range(bookmarks_len):
            name = self._parse_string()
            motors = []
            for _ in range(5):
                motors.append(self._parse_int64())
            auto_focus = self._parse_int8()
            bookmark = [name, motors, auto_focus]
            self.bookmarks.append(bookmark)

__all__.append('SetUserBookmarks')
class SetUserBookmarks(SetStaticBookmarks):
    # same as static bookmarks
    pass

__all__.append('ServerMessage')
class ServerMessage:
    DummyCloseConnection = -1
    MagicalResponse = 0
    ConnectionResult = 1
    FullUpdate = 2
    DirectoryListingResult = 3
    FileDownloadResult = 4
    ErrorMessage = 5
    ListUserResult = 6
    Pong = 7
    InitializationInformation = 8

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

    def _serialize_string(self, buf, string):
        buf.extend(struct.pack(">i", len(string)))
        buf.extend(string.encode('utf8'))

__all__.append('DummyCloseConnection')
class DummyCloseConnection:
    message_type = ServerMessage.DummyCloseConnection

__all__.append('Pong')
class Pong(ServerMessage):
    def __init__(self, ping_id):
        self.message_type = ServerMessage.Pong
        self.ping_id = ping_id

    def _serialize(self):
        buf = bytearray()
        buf.extend(struct.pack(">i", self.ping_id))
        return buf

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
    UnsupportedProtocol = 3

    def __init__(self, status, privileges=None):
        if privileges is None:
            privileges = []

        self.message_type = ServerMessage.ConnectionResult
        self.privileges = privileges
        self.status = status

    def _serialize(self):
        server_name = "SuperWolfe RepatriatorServer 0.0.0"

        buf = bytearray()
        buf.extend(struct.pack(">i", self.protocol))
        buf.extend(struct.pack(">i", len(server_name)))
        buf.extend(server_name.encode('utf8'))
        buf.extend(struct.pack(">i", self.status))
        buf.extend(struct.pack(">i", len(self.privileges)))
        for privilege in self.privileges:
            buf.extend(struct.pack(">i", privilege))
        return buf

__all__.append('InitializationInformation')
class InitializationInformation(ServerMessage):
    def __init__(self, motor_boundaries, static_bookmarks, user_bookmarks):
        self.message_type = ServerMessage.InitializationInformation
        self.motor_boundaries = motor_boundaries
        self.static_bookmarks = static_bookmarks
        self.user_bookmarks = user_bookmarks

    def _serialize(self):
        buf = bytearray()
        for min_value, max_value in self.motor_boundaries:
            buf.extend(struct.pack(">q", min_value))
            buf.extend(struct.pack(">q", max_value))
        for bookmark_list in (self.static_bookmarks, self.user_bookmarks):
            buf.extend(struct.pack(">i", len(bookmark_list)))
            for name, motor_positions, auto_focus in bookmark_list:
                self._serialize_string(buf, name)
                for motor_position in motor_positions:
                    buf.extend(struct.pack(">q", motor_position))
                buf.extend(struct.pack(">b", auto_focus))
        return buf

__all__.append('FullUpdate')
class FullUpdate(ServerMessage):
    def __init__(self, frame_buffer, motor_positions, motor_states):
        """
        frame_buffer is a memoryview of the live view picture
        motor_positions is a dictionary of the motor character to position.
        """
        self.message_type = ServerMessage.FullUpdate
        self.motor_positions = motor_positions
        self.motor_states = motor_states

        self.jpeg = bytearray()

        # get live view frame
        if frame_buffer[0] != b'\xff' or frame_buffer[1] != b'\xd8':
            warning("bad image data: invalid header")
            self.jpeg = bytearray()
        else:
            just_saw_ff = False
            for b in frame_buffer:
                self.jpeg.append(ord(b))
                if b == b'\xff':
                    just_saw_ff = True
                elif just_saw_ff and b == b'\xd9':
                    break
                else:
                    just_saw_ff = False
            else:
                warning("bad image data: could not find end of image (0xffd9)")
                self.jpeg = bytearray()

    def _serialize(self):
        buf = bytearray()
        for char in ['A', 'B', 'X', 'Y', 'Z']:
            buf.extend(struct.pack(">b", self.motor_states[char]))
            buf.extend(struct.pack(">q", self.motor_positions[char]))
        buf.extend(struct.pack(">q", len(self.jpeg)))
        buf.extend(self.jpeg)
        return buf

__all__.append('DirectoryListingResult')
class DirectoryListingResult(ServerMessage):
    def __init__(self, folder_path):
        self.message_type = ServerMessage.DirectoryListingResult
        self.folder_path = folder_path

    def _serialize(self):
        file_list = [os.path.join(self.folder_path, f) for f in os.listdir(self.folder_path) if f.endswith('.jpg')]
        buf = bytearray()
        buf.extend(struct.pack(">i", len(file_list)))
        for file_path in file_list:
            try:
                file_size = os.path.getsize(file_path)
                path, filename = os.path.split(file_path)
                buf.extend(struct.pack(">q", file_size))
                buf.extend(struct.pack(">i", len(filename)))
                buf.extend(filename.encode('utf8'))

                try:
                    thumb_path = file_path + ".thumb"
                    thumb_size = os.path.getsize(thumb_path)
                    with open(thumb_path, "rb") as f:
                        buf.extend(struct.pack(">q", thumb_size))
                        buf.extend(f.read())
                except (OSError, IOError):
                    buf.extend(struct.pack(">q", 0))
                    error("error accessing thumbnail for {0}".format(file_path))
            except (OSError, IOError):
                buf.extend(struct.pack(">q", 0))
                buf.extend(struct.pack(">i", 0))
                error("error getting file size for {0}".format(file_path))
            
        return buf

__all__.append('FileDownloadResult')
class FileDownloadResult(ServerMessage):
    def __init__(self, file_path):
        self.message_type = ServerMessage.FileDownloadResult
        self.file_path = file_path
        self.file_title = os.path.split(file_path)[-1]

    def _serialize(self):
        buf = bytearray()

        buf.extend(struct.pack(">i", len(self.file_title)))
        buf.extend(self.file_title.encode('utf8'))

        try:
            file_size = os.path.getsize(self.file_path)
            with open(self.file_path, "rb") as f:
                buf.extend(struct.pack(">q", file_size))
                buf.extend(f.read())
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
    BadPassword = 7

    descriptions = {
        NotAuthorized: "Not authorized to perform this operation.",
        HardwareInUse: "The hardware is already in use - wait till someone logs off.",
        FileDoesNotExist: "The file does not exist.",
        UserAlreadyExists: "The user you are trying to add already exists.",
        UserDoesNotExist: "The user you are trying to update does not exist.",
        OverwritingOtherUser: "You're trying to change a username in a way that would overwrite another user.",
        InvalidFilename: "The filename you supplied is invalid.",
        BadPassword: "Invalid password.",
    }

    def __init__(self, code, description=None):
        self.message_type = ServerMessage.ErrorMessage
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

__all__.append('ListUserResult')
class ListUserResult(ServerMessage):
    def __init__(self, user_list):
        self.message_type = ServerMessage.ListUserResult
        self.user_list = user_list

    def _serialize(self):
        buf = bytearray()
        buf.extend(struct.pack(">i", len(self.user_list)))
        for username, data in self.user_list.items():
            buf.extend(struct.pack(">i", len(username)))
            buf.extend(username.encode('utf8'))

            buf.extend(struct.pack(">i", len(data['privileges'])))
            for perm in data['privileges']:
                buf.extend(struct.pack(">i", perm))
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
    ClientMessage.ChangePasswordRequest: ChangePasswordRequest,
    ClientMessage.ListUserRequest: ListUserRequest,
    ClientMessage.SetAutoFocusEnabled: SetAutoFocusEnabled,
    ClientMessage.Ping: Ping,
    ClientMessage.SetStaticBookmarks: SetStaticBookmarks,
    ClientMessage.SetUserBookmarks: SetUserBookmarks,
}

