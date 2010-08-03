
import struct

# http://code.activestate.com/recipes/531824-chat-server-client-using-selectselect/


_size_fmt = "I"
def send_message(connection, message_bytes):
    size = len(message_bytes)
    size_bytes = struct.pack(_size_fmt, size)
    connection.sendall(size_bytes)
    connection.sendall(message_bytes)
def receive_message(connection):
    size_size = struct.calcsize(_size_fmt)
    size_bytes = connection.recv(size_size)
    size = struct.unpack(_size_fmt, size_bytes)[0]
    message_bytes = connection.recv(size)
    return message_bytes

_encoding = "utf8"
def bytes_to_str(bytes):
    return bytes.decode(_encoding)
def str_to_bytes(string):
    return string.encode(_encoding)

