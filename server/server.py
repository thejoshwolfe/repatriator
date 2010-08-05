#!/usr/bin/env python3

import struct
import socket, socketserver
import queue

import pythoncom
import edsdk

camera = None

keep_alive = True
HOST, PORT = "localhost", 9999


def main():
    threads = []
    
    server = socketserver.TCPServer((HOST, PORT), ConnectionHandler)
    threads.append(threading.Thread(target=server.serve_forever, name="socket server"))
    
    def init_camera():
        global camera
        status("getting camera")
        camera = edsdk.getFirstCamera()
        status("got camera")
        camera.setLiveViewCallback(live_view_handler)
    threads.append(threading.Thread(target=init_camera, name="init camera"))
    
    for thread in threads:
        thread.start()
    
    status("pumping messages")
    pythoncom.PumpMessages()
    
    status("shutting down")
    global keep_alive
    keep_alive = False
    
    server.shutdown() # blocks
    for thread in threads:
        thread.join()

def status(message):
    print(message)

current_connection = None
current_connection_lock = threading.RLock()
current_connection_broken = threading.Event()
class ConnectionHandler(socketserver.BaseRequestHandler):
    def handle(self):
        global current_connection
        with current_connection_lock:
            if current_connection != None:
                return
            current_connection = self.request
            current_connection_broken.clear()
        current_connection_broken.wait()
        with current_connection_lock:
            current_connection = None
def use_connection(func):
    with current_connection_lock:
        try:
            func()
        except socket.error:
            current_connection_broken.set()

live_view_message_header = b""
def live_view_handler(image_bytes):
    def func():
        if current_connection == None:
            return
        send_message(current_connection, live_view_message_header, image_bytes)
    use_connection(func)


size_fmt = "I"
size_size = struct.calcsize(size_fmt)
def send_message(connection, *message_bytes_parts):
    size = sum(len(message_bytes) for message_bytes in message_bytes_parts)
    size_bytes = struct.pack(size_fmt, size)
    connection.sendall(size_bytes)
    for message_bytes in message_bytes_parts:
        connection.sendall(message_bytes)
def receive_message(connection):
    size_bytes = connection.recv(size_size)
    size = struct.unpack(size_fmt, size_bytes)[0]
    message_bytes = connection.recv(size)
    return message_bytes

if __name__ == "__main__":
    main()
