#!/usr/bin/env python3

from power import set_power_switch
from settings import settings
from messages import *
import auth

import struct
import socket, socketserver
import queue
import os
import threading
import time
import pythoncom
import edsdk

class Server(socketserver.BaseRequestHandler):
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
        msg_size = struct.unpack(header[1:], ">q")
        return header + self.request.recv(msg_size - 9)

    def _run_writer(self):
        while not finished:
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

        if message is not None and self.on_message:
            self.on_message(message)

    def __init__(self, on_message=None, on_connection_open=None, on_connection_close=None):
        self.on_message = on_message
        self.on_connection_open = on_connection_open
        self.on_connection_close = on_connection_close
        self.message_queue = queue.Queue()
        self.finished = False # set to true to tell threads to stop running

        self.writer_thread = threading.Thread(target=self._run_writer, name="message writer")
        self.reader_thread = threading.Thread(target=self._run_reader, name="message reader")
        self.writer_thread.start()
        self.reader_thread.start()

    def send_message(self, message):
        """
        message is a ServerMessage which will be put on the outgoing message queue.
        """
        self.message_queue.put(message)
        
    def handle(self):
        if on_connection_open is not None:
            on_connection_open()

        self.writer_thread

    def finish(self):
        if on_connection_close is not None:
            on_connection_close()

        self.finished = True
        self.writer_thread.join()
        self.reader_thread.join()



# message handlers are guaranteed to be run in the camera thread.
def handle_MagicalRequest(msg):
    # nothing to do
    pass

def handle_ConnectionRequest(msg):
    global user

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
    pass

def handle_MotorMovement(msg):
    pass

def handle_DirectoryListingRequest(msg):
    pass

def handle_FileDownloadRequest(msg):
    pass

def handle_AddUser(msg):
    pass

def handle_UpdateUser(msg):
    pass

def handle_DeleteUser(msg):
    pass

def handle_FileDeleteRequest(msg):
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

def start_server():
    global user, server, server_thread

    # initialize variables
    user = None

    # wait for a connection
    server = socketserver.TCPServer((settings['HOST'], settings['PORT']), Server)
    server.on_message = queue.Queue().put
    server.on_connection_open = on_connection_open
    server.on_connection_close = on_connection_close
    server_thread = threading.Thread(target=server.serve_forever, name="socket server")
    server_thread.start()

def run_camera():
    global camera, message_handlers

    pythoncom.CoInitializeEx(2)
    camera = edsdk.getFirstCamera()
    camera.startLiveView()

    next_frame = time.time()

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
    pass

def on_connection_close():
    global finished, camera_thread, server_thread

    # clean up
    finished = True
    camera_thread.join()
    server_thread.join()
    del server
    set_power_switch(on=False)

    # get ready to listen for next time
    start_server()

def initialize_hardware():
    global finished, camera_thread
    set_power_switch(on=True)

    finished = False
    camera_thread = threading.Thread(target=run_camera, name="camera")
    camera_thread.start()



if __name__ == "__main__":
    start_server()
