#!/usr/bin/env python3

# set up logging first
from settings import settings
import logging
import os, sys
def set_up_logging():
    log_levels = {
        'DEBUG': logging.DEBUG,
        'WARNING': logging.WARNING,
        'ERROR': logging.ERROR,
    }
    log_level = log_levels[settings['LOG_LEVEL']]
    log_file = os.path.join(settings['DATA_FOLDER'], "server.log")
    log_format = '%(asctime)s %(levelname)s: %(message)s'
    logging.basicConfig(filename=log_file, level=log_level, format=log_format)
set_up_logging()

# now import our stuff
from power import set_power_switch
from messages import *
import auth
from auth import Privilege

# python stuff
import struct
import socket, socketserver
import queue
import threading
import time
from functools import wraps
from logging import debug, warning, error

# dependencies
import pythoncom
import edsdk

class Server:
    def _write_message(self, message):
        """
        actually writes the message to the open connection
        """
        raw_data = message.serialize()
        debug("outgoing message (" + message.__class__.__name__ + ")")
        debug("--------------------------------")
        debug(raw_data)
        debug("--------------------------------")
        self.request.sendall(raw_data)

    def _read_amt(self, byte_count):
        """
        blocks until byte_count bytes have been read
        """
        full_msg = bytearray()
        while len(full_msg) < byte_count:
            block = self.request.recv(byte_count - len(full_msg))
            full_msg.extend(block)
        return full_msg
    
    def _read_message(self):
        """
        actually reads the message from the open connection.
        blocks until one is received.
        """
        header = self._read_amt(9)
        msg_size = struct.unpack_from(">q", header, 1)[0]
        return header + self._read_amt(msg_size - 9)

    def _run_writer(self):
        while not self.finished:
            try:
                debug("writer waiting for message")
                item = self.message_queue.get(block=True)
                debug("writer found a message on the queue")
                self._write_message(item)
            except socket.error:
                warning("socket.error when writing message")
                return

    def _run_reader(self):
        while not self.finished:
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
        debug("init server")
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
        debug("adding message to outgoing queue")
        self.message_queue.put(message)
        
    def close(self):
        self.finished = True
        self.writer_thread.join()
        self.reader_thread.join()

        if self.on_connection_close is not None:
            self.on_connection_close()


# message handlers are guaranteed to be run in the camera thread.
def handle_MagicalRequest(msg):
    global server

    # nothing to do
    debug("got a magical request, sending a magical response")
    server.send_message(MagicalResponse())

def handle_ConnectionRequest(msg):
    global user, server

    debug("Got connection request message")

    user = auth.login(msg.username, msg.password)
    if user is None:
        server.send_message(ConnectionResult(ConnectionResult.InvalidLogin))
        return
    
    # if they request hardware to turn on, make sure they have privileges
    if msg.admin_flag and not user.has_privilege(Privilege.OperateHardware):
        server.send_message(ConnectionResult(ConnectionResult.InsufficientPrivileges, user.privileges()))
        return
        
    server.send_message(ConnectionResult(ConnectionResult.Success, user.privileges()))

    if msg.admin_flag:
        initialize_hardware()

def must_have_privilege(privilege):
    def decorated(function):
        def _wrapped(msg, *args, **kwargs):
            global user
            if user.has_privilege(privilege):
                return function(msg, *args, **kwargs)
            else:
                warning("User doesn't have privilege " + str(privilege) + ": sending authorization denied message")
                return server.send_message(ErrorMessage(ErrorMessage.NotAuthorized))
        return wraps(function)(_wrapped)
    return decorated

@must_have_privilege(Privilege.OperateHardware)
def handle_TakePicture(msg):
    debug("Got take picture message")
    pass

@must_have_privilege(Privilege.OperateHardware)
def handle_MotorMovement(msg):
    debug("Got motor movement message")
    pass

@must_have_privilege(Privilege.OperateHardware)
def handle_DirectoryListingRequest(msg):
    debug("Got directory listing message")
    pass

@must_have_privilege(Privilege.OperateHardware)
def handle_FileDownloadRequest(msg):
    debug("Got file download message")
    pass

@must_have_privilege(Privilege.ManageUsers)
def handle_AddUser(msg):
    debug("Got add user message")
    pass

@must_have_privilege(Privilege.ManageUsers)
def handle_UpdateUser(msg):
    debug("Got update user message")
    pass

@must_have_privilege(Privilege.ManageUsers)
def handle_DeleteUser(msg):
    debug("Got delete user message")
    pass

@must_have_privilege(Privilege.ManageUsers)
def handle_FileDeleteRequest(msg):
    debug("Got file delete request message")
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

need_camera_thread = {
    ClientMessage.MagicalRequest: False,
    ClientMessage.ConnectionRequest: False,
    ClientMessage.TakePicture: True,
    ClientMessage.MotorMovement: True,
    ClientMessage.DirectoryListingRequest: False,
    ClientMessage.FileDownloadRequest: False,
    ClientMessage.AddUser: False,
    ClientMessage.UpdateUser: False,
    ClientMessage.DeleteUser: False,
    ClientMessage.FileDeleteRequest: False,
}

def reset_state():
    global user, message_queue, camera_thread, camera_thread_queue, finished

    # initialize variables
    finished = False
    user = None
    camera_thread = None
    message_queue = queue.Queue()
    camera_thread_queue = queue.Queue()

def start_server():
    reset_state()

    # wait for a connection
    class _Server(socketserver.BaseRequestHandler):
        def handle(self):
            self.server = Server(message_queue.put, on_connection_open, on_connection_close)
            self.server.request = self.request
            self.server.open()

            global server
            server = self.server
        def finish(self):
            self.server.close()

    _server = socketserver.TCPServer((settings['HOST'], settings['PORT']), _Server)
    global server_thread
    server_thread = threading.Thread(target=_server.serve_forever, name="socket server")
    server_thread.start()

    global message_thread
    message_thread = threading.Thread(target=run_message_loop, name="message handler")
    message_thread.start()

    debug("\n\n")
    debug("serving on {0} {1}".format(settings['HOST'], settings['PORT']))

def run_message_loop():
    global message_handlers, message_queue, camera_thread, finished

    debug("entering message loop")
    while not finished:
        msg = message_queue.get(block=True)
        # choose which thread to run the message handler in
        if need_camera_thread[msg.message_type]:
            # transfer to camera thread    
            camera_thread_queue.put(msg)
        else:
            # handle directly
            message_handlers[msg.message_type](msg)

def run_camera():
    global camera, message_handlers, message_queue, finished

    debug("running camera")

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

        # process messages that have to be run in camera thread
        try:
            msg = camera_thread_queue.get(block=False)
            message_handlers[msg.message_type](msg)
        except queue.Empty:
            time.sleep(0.05)
            continue

def on_connection_open():
    debug("on_connection_open")

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
