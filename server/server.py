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
    log_format = '%(asctime)s "%(threadName)s" %(levelname)s: %(message)s'
    logging.basicConfig(filename=log_file, level=log_level, format=log_format)
set_up_logging()
from logging import debug, warning, error
debug("\n\nServer starting\n")

# now import our stuff
from power import set_power_switch
from messages import *
import auth
from auth import Privilege
from thumbnail import make_thumbnail

# python stuff
import struct
import socket, socketserver
import queue
import threading
import time
import traceback
from functools import wraps

# dependencies
import pythoncom
import edsdk
import silverpak


def camera_error_handler(level, msg):
    full_msg = "EDSDK: %s" % msg
    {
        edsdk.ErrorLevel.Debug: debug,
        edsdk.ErrorLevel.Warn: warning,
        edsdk.ErrorLevel.Error: error,
    }[level](full_msg)

edsdk.setErrorLevel(edsdk.ErrorLevel.Debug)
edsdk.setErrorMessageCallback(camera_error_handler)

class Server:
    def _write_message(self, message):
        """
        actually writes the message to the open connection
        """
        raw_data = message.serialize()
        debug("writing outgoing message of type " + message.__class__.__name__)
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
        while True:
            try:
                item = self.message_queue.get(block=True)

                if item.message_type == ServerMessage.DummyCloseConnection:
                    debug("closing connection and exiting thread")
                    self.request.shutdown(2)
                    return
                with self._current_full_update_lock:
                    if item.message_type == ServerMessage.FullUpdate:
                        item = self._current_full_update
                        self._current_full_update = None
                if item != None:
                    self._write_message(item)
            except socket.error:
                error("socket.error when writing message, exiting thread")
                self.close()
                return

    def _run_reader(self):
        while True:
            try:
                data = self._read_message()
            except socket.error:
                debug("socket.error when reading message, exiting thread")
                self.close()
                return
            except:
                error("something bad happened when reading from socket: {0}".format(traceback.format_exc()))
                self.close()
                return

            message = None
            try:
                message = ClientMessage.parse(data)
            except ClientMessage.ParseError as ex:
                error("when parsing client message: " + str(ex))
                message = None

            if message is not None and self.on_message is not None:
                self.on_message(message)

    def __init__(self, on_message=None, on_connection_open=None, on_connection_close=None):
        debug("init server")
        self.on_message = on_message
        self.on_connection_open = on_connection_open
        self.on_connection_close = on_connection_close
        self.message_queue = queue.Queue()
        self._current_full_update_lock = threading.RLock()
        self._current_full_update = None

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
        with self._current_full_update_lock:
            if message.message_type == ServerMessage.FullUpdate:
                self._current_full_update = message
        self.message_queue.put(message)

    def wait(self):
        for thread in (self.writer_thread, self.reader_thread):
            if threading.current_thread != thread:
                thread.join()

    def close(self):
        self.message_queue.put(DummyCloseConnection())


invalid_ntfs_characters = set('\\/:*?"<>|' + "".join(chr(x) for x in range(0x20)))
def valid_filename(filename):
    return not any(c in invalid_ntfs_characters for c in filename)

# message handlers are guaranteed to be run in the camera thread.
def handle_MagicalRequest(msg):
    global server
    debug("got a magical request, sending a magical response")
    server.send_message(MagicalResponse())

def handle_ConnectionRequest(msg):
    global user, server, motor_chars

    debug("Got connection request message")
    debug("version: {0}".format(str(msg.version)))

    user = auth.login(msg.username, msg.password)
    if user is None:
        warning("Invalid login: " + msg.username + ", Password: <hidden>")
        server.send_message(ConnectionResult(ConnectionResult.InvalidLogin))
        server.close()
        return

    # if they request hardware to turn on, make sure they have privileges
    if msg.hardware_flag and not user.has_privilege(Privilege.OperateHardware):
        warning(msg.username + " requested hardware access but doesn't have permission")
        server.send_message(ConnectionResult(ConnectionResult.InsufficientPrivileges, user.privileges()))
        server.close()
        return
    elif not msg.hardware_flag and not user.has_privilege(Privilege.ManageUsers):
        warning(msg.username + " tried to manage users but doesn't have permission")
        server.send_message(ConnectionResult(ConnectionResult.InsufficientPrivileges, user.privileges()))
        server.close()
        return

    debug("Successful login for user " + msg.username)
    motor_boundaries = [[settings['MOTOR_%s_%s' % (char, setting_name)] for setting_name in ("MIN", "MAX", "START_POSITION")] for char in motor_chars]
    server.send_message(ConnectionResult(ConnectionResult.Success, user.privileges()))
    if msg.hardware_flag:
        server.send_message(InitializationInformation(motor_boundaries))

    if msg.hardware_flag:
        initialize_hardware()
    else:
        init_ping_thread()

def must_have_privilege(privilege):
    def decorated(function):
        def _wrapped(msg, *args, **kwargs):
            global user
            if user.has_privilege(privilege):
                return function(msg, *args, **kwargs)
            else:
                warning("User doesn't have privilege {0}: sending authorization denied message".format(privilege))
                return server.send_message(ErrorMessage(ErrorMessage.NotAuthorized))
        return wraps(function)(_wrapped)
    return decorated

@must_have_privilege(Privilege.OperateHardware)
def handle_TakePicture(msg):
    global camera, user
    debug("Got take picture message")

    def unique_file(path, filename):
        n = 0
        while True:
            n += 1
            candidate = os.path.join(path, filename.format(n))
            if not os.path.exists(candidate):
                return candidate

    camera.takePicture(unique_file(user.picture_folder(), "img_{0}.jpg"))

@must_have_privilege(Privilege.OperateHardware)
def handle_MotorMovement(msg):
    global motors
    debug("Got motor movement message")

    if motors is None:
        warning("Can't move motors yet. they haven't been found")
        return
    for char, motor in motors.items():
        move_motor(motor, msg.motor_pos[char])

def move_motor(motor, intended_position):
    if motor.position() == intended_position:
        return False
    try:
        debug("moving motor " + repr(motor.name) + " from " + repr(motor.position()) + " to " + repr(intended_position))
        motor.goToPosition(intended_position)
    except silverpak.InvalidSilverpakOperationException as e:
        warning("Can't move motor: " + str(e))
        pass
    return True

@must_have_privilege(Privilege.OperateHardware)
def handle_DirectoryListingRequest(msg):
    global server, user
    debug("Got directory listing message")
    send_directory_list()

def send_directory_list():
    files = [os.path.join(user.picture_folder(), f) for f in os.listdir(user.picture_folder()) if f.endswith('.jpg')]
    server.send_message(DirectoryListingResult(files))


@must_have_privilege(Privilege.OperateHardware)
def handle_FileDownloadRequest(msg):
    global server, user
    debug("Got file download message")

    if not valid_filename(msg.filename):
        warning("filename which user supplied to download contained invalid characters: {0}".format(msg.filename))
        server.send_message(ErrorMessage(ErrorMessage.InvalidFilename))
        return

    target_file = os.path.join(user.picture_folder(), msg.filename)
    if not os.path.exists(target_file):
        warning("filename which user supplied to download does not exist: {0}".format(msg.filename))
        server.send_message(ErrorMessage(ErrorMessage.FileDoesNotExist))
        return

    server.send_message(FileDownloadResult(target_file))

@must_have_privilege(Privilege.OperateHardware)
def handle_FileDeleteRequest(msg):
    global server, user
    debug("Got file delete request message")

    if not valid_filename(msg.filename):
        warning("filename which user supplied to delete contained invalid characters: {0}".format(msg.filename))
        server.send_message(ErrorMessage(ErrorMessage.InvalidFilename))
        return

    target_path = os.path.join(user.picture_folder(), msg.filename)
    if not os.path.exists(target_path):
        warning("filename which user supplied to delete does not exist: {0}".format(msg.filename))
        server.send_message(ErrorMessage(ErrorMessage.FileDoesNotExist))
        return

    debug("Removing file {0}".format(target_path))
    try:
        os.remove(target_path)
        os.remove(target_path+".thumb")
    except OSError as ex:
        error("OSError removing file: {0}".format(ex))

    # notify change
    send_directory_list()

def handle_ChangePasswordRequest(msg):
    global server
    debug("Got change password message")

    global user
    try:
        user.change_password(msg.old_password, msg.new_password)
        user.save()
    except auth.BadPassword:
        warning("user {0} tried to change password with invalid old password, sending error message".format(msg.username))
        server.send_message(ErrorMessage(ErrorMessage.BadPassword))
        return

@must_have_privilege(Privilege.ManageUsers)
def handle_AddUser(msg):
    global server
    debug("Got add user message")

    try:
        new_user = auth.User(msg.username, msg.password, msg.privileges)
        new_user.save()
        debug("created new user {0}".format(msg.username))
    except auth.UserAlreadyExists:
        warning("user {0} already exists, sending error message".format(msg.username))
        server.send_message(ErrorMessage(ErrorMessage.UserAlreadyExists))
        return

@must_have_privilege(Privilege.ManageUsers)
def handle_UpdateUser(msg):
    global server
    debug("Got update user message")

    target_user = auth.get_user(msg.username)
    if target_user is None:
        warning("user {0} does not exist, sending error message".format(msg.username))
        server.send_message(ErrorMessage(ErrorMessage.UserDoesNotExist))
        return

    debug("updating user {0} and saving to disk".format(msg.username))
    if len(msg.password) > 0:
        target_user.attrs['password'] = msg.password
    target_user.attrs['privileges'] = msg.privileges
    target_user.save()

@must_have_privilege(Privilege.ManageUsers)
def handle_DeleteUser(msg):
    global server
    debug("Got delete user message")

    try:
        auth.delete_user(msg.username)
        debug("deleted {0}".format(msg.username))
    except auth.UserDoesNotExist:
        warning("user {0} does not exist, sending error message".format(msg.username))
        server.send_message(ErrorMessage(ErrorMessage.UserDoesNotExist))
        return

@must_have_privilege(Privilege.ManageUsers)
def handle_ListUserRequest(msg):
    global server
    debug("Got list user request message")
    server.send_message(ListUserResult(auth.list_users()))

@must_have_privilege(Privilege.OperateHardware)
def handle_SetAutoFocusEnabled(msg):
    global auto_focus_enabled
    auto_focus_enabled = msg.value
    maybe_trigger_auto_focus()

@must_have_privilege(Privilege.OperateHardware)
def motorStoppedMovingHandler(reason):
    if reason != silverpak.StoppedMovingReason.Normal:
        return
    global need_to_auto_focus, camera_thread_queue
    need_to_auto_focus = True
    debug("a motor stopped moving. setting auto focus flag.")
    camera_thread_queue.put(DummyAutoFocus())

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
    ClientMessage.ChangePasswordRequest: handle_ChangePasswordRequest,
    ClientMessage.ListUserRequest: handle_ListUserRequest,
    ClientMessage.SetAutoFocusEnabled: handle_SetAutoFocusEnabled,
}

need_camera_thread = {
    ClientMessage.TakePicture: True,
    ClientMessage.SetAutoFocusEnabled: True,

    ClientMessage.MotorMovement: False,
    ClientMessage.MagicalRequest: False,
    ClientMessage.ConnectionRequest: False,
    ClientMessage.DirectoryListingRequest: False,
    ClientMessage.FileDownloadRequest: False,
    ClientMessage.AddUser: False,
    ClientMessage.UpdateUser: False,
    ClientMessage.DeleteUser: False,
    ClientMessage.FileDeleteRequest: False,
    ClientMessage.ChangePasswordRequest: False,
    ClientMessage.ListUserRequest: False,
}

def init_state():
    global user, message_thread, message_queue, camera_thread, camera_thread_queue, finished, motor_chars, motors, motor_is_initialized, ping_thread, auto_focus_enabled, need_to_auto_focus

    # initialize variables
    finished = False
    user = None
    camera_thread = None
    message_thread = None
    motor_chars = ['A', 'B', 'X', 'Y', 'Z']
    motors = None
    motor_is_initialized = {char: False for char in motor_chars}
    camera_thread_queue = queue.Queue()
    message_queue = queue.Queue()
    ping_thread = None
    auto_focus_enabled = True
    need_to_auto_focus = False


def start_message_loop():
    global message_thread
    message_thread = threading.Thread(target=run_message_loop, name="message handler")
    message_thread.start()

def start_server():
    init_state()
    set_power_switch(False)

    # wait for a connection
    class _Server(socketserver.StreamRequestHandler):
        def handle(self):
            debug("_Server.handle called")
            self.server = Server(message_queue.put, on_connection_open, on_connection_close)
            self.server.request = self.request
            self.server.open()
            global server
            server = self.server

            self.server.wait()
        def finish(self):
            if on_connection_close:
                on_connection_close()

    _server = socketserver.TCPServer((settings['HOST'], settings['PORT']), _Server)
    global server_thread
    server_thread = threading.Thread(target=_server.serve_forever, name="socket server")
    server_thread.start()

    start_message_loop()
    debug("serving on {0} {1}".format(settings['HOST'], settings['PORT']))

def run_message_loop():
    global message_handlers, message_queue, camera_thread, finished

    debug("entering message loop")
    while not finished:
        msg = message_queue.get(block=True)

        # check for message indicating thread is done
        if msg.message_type == ClientMessage.DummyCloseConnection:
            return

        # choose which thread to run the message handler in
        if need_camera_thread[msg.message_type]:
            # transfer to camera thread    
            camera_thread_queue.put(msg)
        else:
            # handle directly
            message_handlers[msg.message_type](msg)

def run_ping():
    global finished

    debug("running ping thread")

    next_frame = time.time()
    while not finished:
        # if it's time, send a ping
        now = time.time()
        if now > next_frame:
            server.send_message(Ping())
            next_frame = now + 1.00

def run_camera():
    global camera, message_handlers, message_queue, finished

    debug("running camera")

    pythoncom.CoInitializeEx(2)

    # try for 15 seconds to get the camera
    fakeCameraImagePath = settings['FAKE_CAMERA_IMAGE_PATH']
    global finished
    if fakeCameraImagePath != None:
        debug("using fake camera")
        camera = edsdk.getFakeCamera(fakeCameraImagePath)
    else:
        debug("getting first camera")
        tries = 0
        while tries < 15:
            tries += 1
            try:
                camera = edsdk.getFirstCamera()
                break
            except edsdk.CppCamera.error:
                debug("unable to get camera, waiting a second and trying again")
                time.sleep(1)
                if finished:
                    return
        else:
            error("Unable to get camera.")
            return

    def takePictureCallback(pic_file):
        # create a thumbnail
        make_thumbnail(pic_file, pic_file+".thumb", 90)
        # notify change
        send_directory_list()
    camera.setPictureCompleteCallback(takePictureCallback)

    debug("starting live view")
    camera.startLiveView()

    next_frame = time.time()

    debug("entering loop to send live view frames")
    while not finished:
        pythoncom.PumpWaitingMessages()

        # if it's time, send a live view frame
        now = time.time()
        if now > next_frame:
            if motors is None:
                motor_positions = {char: 0 for char in motor_chars}
                motor_states = {char: 0 for char in motor_chars}
            else:
                motor_positions = {char: motor.position() for char, motor in motors.items()}
                motor_states = {char: int(motor_is_initialized[char]) for char, motor in motors.items()}
            server.send_message(FullUpdate(camera.liveViewMemoryView(), motor_positions, motor_states))
            camera.grabLiveViewFrame()
            next_frame = now + 0.20

        # process messages that have to be run in camera thread
        try:
            msg = camera_thread_queue.get(block=False)
            if msg.message_type == ClientMessage.DummyAutoFocus:
                maybe_trigger_auto_focus()
            else:
                message_handlers[msg.message_type](msg)
        except queue.Empty:
            time.sleep(0.05)
            continue

    # shut down the camera
    del camera
    camera = None
    edsdk.terminate()

def maybe_trigger_auto_focus():
    global need_to_auto_focus
    if not (auto_focus_enabled and need_to_auto_focus):
        return
    debug("Telling camera to auto focus")
    camera.autoFocus()
    need_to_auto_focus = False

def on_connection_open():
    debug("connection opening")

def on_connection_close():
    global finished, camera_thread, server_thread, ping_thread

    debug("connection closing")

    # send motors home
    if motors is not None:
        for char, motor in motors.items():
            motor.stoppedMovingHandlers.remove(motorStoppedMovingHandler)
            if motor.fancy:
                end_position = settings['MOTOR_%s_END_POSITION' % char]
                debug("sending motor " + repr(motor.name) + " home to " + repr(end_position))
                move_motor(motor, end_position)

    # clean up
    finished = True

    if camera_thread is not None:
        debug("waiting for camera thread to join")
        camera_thread.join()

    if ping_thread is not None:
        debug("waiting for ping thread to join")
        ping_thread.join()

    if message_thread is not None:
        debug("waiting for message thread to join")
        message_queue.put(DummyCloseConnection())
        message_thread.join()

    if motors is not None:
        debug("shutting down motors")
        motor_disposer_threads = []
        def shutdown_motor(motor):
            while not motor.isReady():
                debug("waiting for motor " + repr(motor.name) + " to stop moving")
                time.sleep(1)
            debug("disposing motor " + repr(motor.name))
            motor.dispose()
        for motor in motors.values():
            thread = threading.Thread(name="motor " + repr(motor.name) + " disposer", target=shutdown_motor, args=[motor])
            thread.start()
            motor_disposer_threads.append(thread)
        for thread in motor_disposer_threads:
            thread.join()

    set_power_switch(on=False)

    debug("done with life. comitting seppuku")
    os._exit(0)

def init_ping_thread():
    global ping_thread, finished

    finished = False
    ping_thread = threading.Thread(target=run_ping, name="ping")
    ping_thread.start()

def initialize_hardware():
    global finished, camera_thread
    set_power_switch(on=True)

    finished = False
    camera_thread = threading.Thread(target=run_camera, name="camera")
    camera_thread.start()

    def create_motor(char):
        motor = silverpak.Silverpak()
        motor.baudRate = settings['MOTOR_%s_BAUD_RATE' % char]
        motor.driverAddress = settings['MOTOR_%s_DRIVER_ADDRESS' % char]
        motor.fancy = settings['MOTOR_%s_FANCY' % char]
        motor.velocity = settings['MOTOR_%s_VELOCITY' % char]
        motor.acceleration = settings['MOTOR_%s_ACCELERATION' % char]
        motor.maxPosition = settings['MOTOR_%s_MAX' % char]
        if settings['MOTOR_%s_FAKE' % char]:
            motor.setFake()
        motor.name = char
        motor.stoppedMovingHandlers.append(motorStoppedMovingHandler)
        return motor

    global motors
    motors = {char: create_motor(char) for char in motor_chars}

    found = {char: False for char in motor_chars}
    threads = []
    for char, motor in motors.items():
        def find_motor(char, motor):
            def done_initializing_handler(*args):
                motor.stoppedMovingHandlers.remove(done_initializing_handler)
                motor_is_initialized[char] = True
            def go_to_startup_handler(*args):
                motor.stoppedMovingHandlers.remove(go_to_startup_handler)
                motor.stoppedMovingHandlers.append(done_initializing_handler)
                if not move_motor(motor, settings['MOTOR_%s_START_POSITION' % char]):
                    # no initial position. just call the final step
                    done_initializing_handler()
            for _ in range(15):
                found[char] = motor.findAndConnect()
                if not found[char]:
                    warning("Unable to find and connect to silverpak motor '{0}', waiting a sec and trying again".format(char))
                    time.sleep(1)
                    if finished:
                        # never mind
                        return
                    # try again
                    continue
                # success
                debug("found motor " + repr(char))
                motor.stoppedMovingHandlers.append(go_to_startup_handler)
                motor.fullInit()
                # wait for initialization
                while not motor_is_initialized[char]:
                    debug("waiting for motor " + repr(char) + " to initialize")
                    time.sleep(1)
                debug("motor " + repr(char) + " is done initializing")
                return
        thread = threading.Thread(target=find_motor, name="motor " + repr(char) + " finder", args=[char, motor])
        thread.start()
        threads.append(thread)
    for thread in threads:
        thread.join()
    if not all(found.values()):
        error("Unable to find and connect to all motors.")
        # leave motors None
        return

if __name__ == "__main__":
    start_server()

