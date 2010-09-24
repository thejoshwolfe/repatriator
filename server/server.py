#!/usr/bin/env python3

from power import set_power_switch
from settings import settings
from messages import *
import auth

import struct
import socket, socketserver
import queue
import os, sys
import threading
import time
import pythoncom
import edsdk

class Server:
    def _write_message(self, message):
        """
        actually writes the message to the open connection
        """
        self.request.sendall(message.serialize())
    
    def _read_message(self):
        """
        actually reads the message from the open connection.
        blocks until one is received.
        """
        header = self.request.recv(9)
        msg_size = struct.unpack_from(">q", header, 1)[0]
        return header + self.request.recv(msg_size - 9)

    def _run_writer(self):
        while not self.finished:
            try:
                item = self.message_queue.get(block=False)
                self._write_message(item)
            except queue.Empty:
                time.sleep(0.05)
            except socket.error:
                return

    def _run_reader(self):
        try:
            data = self._read_message()
        except socket.error:
            return

        message = None
        try:
            message = ClientMessage.parse(data)
        except ClientMessage.ParseError as ex:
            sys.stderr.write(str(ex) + "\n")

        if message is not None and self.on_message is not None:
            self.on_message(message)

    def __init__(self, on_message=None, on_connection_open=None, on_connection_close=None):
        print("init server")
        self.on_message = on_message
        self.on_connection_open = on_connection_open
        self.on_connection_close = on_connection_close
        self.message_queue = queue.Queue()
        self.finished = False # set to true to tell threads to stop running

        self.writer_thread = threading.Thread(target=self._run_writer, name="message writer")
        self.reader_thread = threading.Thread(target=self._run_reader, name="message reader")

    def open(self):
        self.writer_thread.start()
        self.reader_thread.start()

        if self.on_connection_open is not None:
            self.on_connection_open()

    def send_message(self, message):
        """
        message is a ServerMessage which will be put on the outgoing message queue.
        """
        self.message_queue.put(message)
        
    def close(self):
        self.finished = True
        self.writer_thread.join()
        self.reader_thread.join()

        if self.on_connection_close is not None:
            self.on_connection_close()


# message handlers are guaranteed to be run in the camera thread.
def handle_MagicalRequest(msg):
    # nothing to do
    print("got a magical request, doing nothing")
    pass

def handle_ConnectionRequest(msg):
    global user

    print("Got connection request message")

    user = auth.login(msg.username, msg.password)
    if user is None:
        server.send_message(ConnectionResult(ConnectionResult.InvalidLogin))
        return
    
    # if they request hardware to turn on, make sure they have privileges
    if msg.admin_flag and not user.has_privilege(auth.Privilege.OperateHardware):
        server.send_message(ConnectionResult(ConnectionResult.InsufficientPrivileges, user.privileges()))
        return
        
    server.send_message(ConnectionResult(ConnectionResult.Success, user.privileges()))

    if msg.admin_flag:
        initialize_hardware()

def handle_TakePicture(msg):
    print("Got take picture message")
    pass

def handle_MotorMovement(msg):
    print("Got motor movement message")
    pass

def handle_DirectoryListingRequest(msg):
    print("Got directory listing message")
    pass

def handle_FileDownloadRequest(msg):
    print("Got file dowrload message")
    pass

def handle_AddUser(msg):
    print("Got add user message")
    pass

def handle_UpdateUser(msg):
    print("Got update user message")
    pass

def handle_DeleteUser(msg):
    print("Got tdelete user message")
    pass

def handle_FileDeleteRequest(msg):
    print("Got file delete request message")
    pass

message_handlers = {
    ClientMessage.MagicalRequest: handle_MagicalRequest,
    ClientMessage.ConnectionRequest: handle_ConnectionRequest,
    ClientMessage.TakePicture: handle_TakePicture,
    ClientMessage.MotorMovement: handle_MotorMovement,
    ClientMessage.DirectoryListingRequest: handle_DirectoryListingRequest,
    ClientMessage.FileDownloadRequest: handle_FileDownloadRequest,
    ClientMessage.AddUser: handle_AddUser,
    ClientMessage.UpdateUser: handle_UpdateUser,
    ClientMessage.DeleteUser: handle_DeleteUser,
    ClientMessage.FileDeleteRequest: handle_FileDeleteRequest,
}

def reset_state():
    global user, message_queue, camera_thread

    # initialize variables
    user = None
    camera_thread = None
    message_queue = queue.Queue()

def start_server():
    global user, server, server_thread, message_queue, camera_thread

    reset_state()

    # wait for a connection
    class _Server(socketserver.BaseRequestHandler):
        def handle(self):
            self.server = Server(message_queue.put, on_connection_open, on_connection_close)
            self.server.request = self.request
            self.server.open()
        def finish(self):
            self.server.close()

    server = socketserver.TCPServer((settings['HOST'], settings['PORT']), _Server)
    server_thread = threading.Thread(target=server.serve_forever, name="socket server")
    server_thread.start()
    print("serving on {0} {1}".format(settings['HOST'], settings['PORT']))

def run_camera():
    global camera, message_handlers, message_queue

    print("running camera")

    pythoncom.CoInitializeEx(2)
    camera = edsdk.getFirstCamera()
    camera.startLiveView()

    next_frame = time.time()

    print("entering message loop")
    while not finished:
        pythoncom.PumpWaitingMessages()

        # if it's time, send a live view frame
        now = time.time()
        if now > next_frame:
            server.send_message(FullUpdate(camera))
            camera.grabLiveViewFrame()
            next_frame = now + 0.20

        # process messages
        try:
            msg = message_queue.get(block=False)
            message_handlers[msg.message_type](msg)
        except queue.Empty:
            time.sleep(0.05)
            continue

def on_connection_open():
    print("on_connection_open")
    pass

def on_connection_close():
    global finished, camera_thread, server_thread

    # clean up
    finished = True
    if camera_thread is not None:
        camera_thread.join()
    set_power_switch(on=False)

    reset_state()

def initialize_hardware():
    global finished, camera_thread
    set_power_switch(on=True)

    finished = False
    camera_thread = threading.Thread(target=run_camera, name="camera")
    camera_thread.start()



if __name__ == "__main__":
    start_server()
