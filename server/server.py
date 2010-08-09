#!/usr/bin/env python3

import struct
import socket, socketserver
import queue

import threading
import time
import pythoncom
import edsdk

camera = None
keep_alive = True
try:
    from config import HOST, PORT
except ImportError:
    HOST = "localhost"
    PORT = 9999
    import os
    f = open(os.path.join(os.path.split(__file__)[0], "config.py"), "w")
    f.write("HOST = {0}\nPORT = {1}\n".format(repr(HOST), repr(PORT)))
    f.close()

class MsgToClient:
    FullUpdate = 0

class MsgToServer:
    TakePicture = 0

def main():
    threads = []
    
    server = socketserver.TCPServer((HOST, PORT), ConnectionHandler)
    threads.append(threading.Thread(target=server.serve_forever, name="socket server"))
    
    def run_camera():
        print("co initializing ex")
        pythoncom.CoInitializeEx(2)
        print("getting camera")
        global camera
        camera = edsdk.getFirstCamera()
        print("got camera. starting live view")
        camera.startLiveView()
        pythoncom.PumpWaitingMessages()
        frame_buffer = camera.liveViewMemoryView()

        while current_connection is None:
            time.sleep(0.20)

        while keep_alive and current_connection is not None:
            pythoncom.PumpWaitingMessages()

            def func():
                if frame_buffer[0] != b'\xff' or frame_buffer[1] != b'\xd8':
                    print("invalid image data, skipping frame")
                    return

                buf = bytearray()
                buf.append(0)
                ff_flag = False
                for b in frame_buffer:
                    buf.append(ord(b))
                    if b == b'\xff':
                        ff_flag = True
                    elif b == b'\xd9' and ff_flag:
                        break
                    else:
                        ff_flag = False
                else:
                    raise Exception("could not find ff flag")

                send_message(current_connection, buf)

            use_connection(func)

            camera.grabLiveViewFrame()
            time.sleep(0.20)

    def handle_message(data):
        assert len(data) >= 1

        if data[0] == MsgToServer.TakePicture:
            print("Taking picture...")
        else:
            raise Exception("bad header from client")
    
    def run_check_messages():
        while current_connection is None:
            time.sleep(0.20)

        while keep_alive and current_connection is not None:
            pythoncom.PumpWaitingMessages()

            def func():
                data = receive_message(current_connection)
                if len(data) > 0:
                    handle_message(data)

            use_connection(func)

    threads.append(threading.Thread(target=run_camera, name="init camera"))
    threads.append(threading.Thread(target=run_check_messages, name="check messages"))
    
    for thread in threads:
        thread.start()
    
    print("pumping messages")
    pythoncom.PumpMessages()
    
    print("shutting down")
    global keep_alive
    keep_alive = False
    
    server.shutdown() # blocks
    for thread in threads:
        thread.join()

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
    try:
        func()
    except socket.error:
        current_connection_broken.set()

size_fmt = "I"
size_size = struct.calcsize(size_fmt)
def send_message(connection, message_bytes):
    size = len(message_bytes)
    size_bytes = struct.pack(size_fmt, size)
    print(str(size) + ": " + " ".join(str(b) for b in (size_bytes + message_bytes[:4])) + " ... " + \
        " ".join(str(b) for b in message_bytes[-4:]))
    connection.sendall(size_bytes)
    connection.sendall(message_bytes)
def receive_message(connection):
    size_bytes = connection.recv(size_size)
    size = struct.unpack(size_fmt, size_bytes)[0]
    message_bytes = connection.recv(size)
    return message_bytes

if __name__ == "__main__":
    main()
