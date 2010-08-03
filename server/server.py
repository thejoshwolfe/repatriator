#!/usr/bin/env python3
import socket, socketserver

from communication import *

HOST, PORT = "localhost", 9999

# http://docs.python.org/py3k/library/socket.html#socket-objects
# http://docs.python.org/py3k/library/socketserver.html


def server_main():
    server = socketserver.TCPServer((HOST, PORT), ConnectionHandler)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass

class ConnectionHandler(socketserver.BaseRequestHandler):
    def handle(self):
        connection = self.request
        connection.settimeout(1.0)
        while True:
            try:
                message_bytes = receive_message(connection)
                message_str = bytes_to_str(message_bytes)
                print(message_str)
                send_message(connection, str_to_bytes(message_str.upper()))
            except socket.timeout:
                send_message(connection, str_to_bytes("timeout"))

if __name__ == "__main__":
    server_main()


