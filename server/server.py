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
error("\n\nServer starting\n")

# now import our stuff
from power import set_power_switch
from messages import *
import admin
from admin import Privilege
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

        self.writer_thread = make_thread(self._run_writer, "message writer")
        self.reader_thread = make_thread(self._run_reader, "message reader")

    def open(self):
        self.writer_thread.start()
        self.reader_thread.start()

        if self.on_connection_open is not None:
            self.on_connection_open()

    def send_message(self, message):
        """
        message is a ServerMessage which will be put on the outgoing message queue.
        """
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

def handle_MagicalRequest(msg):
    debug("got a magical request, sending a magical response")
    server.send_message(MagicalResponse())

def handle_ConnectionRequest(msg):
    global user, server, motor_chars

    debug("Got connection request message")
    debug("protocol supported: {0}".format(str(msg.newest_protocol_supported)))

    this_protocol_version = 0
    if msg.newest_protocol_supported < this_protocol_version:
        server.send_message(ConnectionResult(ConnectionResult.UnsupportedProtocol))
        server.close()
        return

    try:
        user = admin.login(msg.username, msg.password)
    except (admin.UserDoesNotExist, admin.BadPassword):
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
    motor_boundaries = [[settings['MOTOR_%s_%s' % (char, setting_name)] for setting_name in ("MIN", "MAX")] for char in motor_chars]
    server.send_message(ConnectionResult(ConnectionResult.Success, this_protocol_version, user.privileges()))
    if msg.hardware_flag:
        server.send_message(InitializationInformation(motor_boundaries, admin.static_bookmarks(), user.bookmarks()))

    if msg.hardware_flag:
        initialize_hardware()

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

def make_thread(target, name, args=[]):
    def exception_catcher():
        try:
            target(*args)
        except:
            error("Fatal exception, killing myself:\n" + traceback.format_exc())
            os._exit(0)
    return threading.Thread(target=exception_catcher, name=name)
silverpak.make_thread = make_thread
edsdk._make_thread = make_thread

@must_have_privilege(Privilege.OperateHardware)
def handle_TakePicture(msg):
    debug("Got take picture message")

    def unique_file(path, filename):
        n = 0
        while True:
            n += 1
            candidate = os.path.join(path, filename.format(n))
            if not os.path.exists(candidate):
                return candidate

    if camera is not None:
        camera.takePicture(unique_file(user.picture_folder(), "img_{0}.jpg"))

@must_have_privilege(Privilege.OperateHardware)
def handle_MotorMovement(msg):
    global motors
    debug("Got motor movement message")

    motor_movement_queue.put(msg.motor_pos)


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
    server.send_message(DirectoryListingResult(user.picture_folder()))

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

# any permissions will do for changing your own password
def handle_ChangePasswordRequest(msg):
    debug("Got change password message")
    try:
        user.change_password(msg.old_password, msg.new_password)
        user.save()
    except admin.BadPassword:
        warning("user {0} tried to change password with invalid old password, sending error message".format(user.username))
        server.send_message(ErrorMessage(ErrorMessage.BadPassword))
        return
    debug("changed password for user {0}".format(user.username))

@must_have_privilege(Privilege.ManageUsers)
def handle_AddUser(msg):
    debug("Got add user message")
    try:
        admin.add_user(msg.username, msg.password, msg.privileges)
    except admin.UserAlreadyExists:
        warning("user {0} already exists, sending error message".format(msg.username))
        server.send_message(ErrorMessage(ErrorMessage.UserAlreadyExists))
        return
    debug("created new user {0}".format(msg.username))

@must_have_privilege(Privilege.ManageUsers)
def handle_UpdateUser(msg):
    debug("Got update user message")
    try:
        target_user = admin.get_user(msg.username)
    except admin.UserDoesNotExist:
        warning("user {0} does not exist, sending error message".format(msg.username))
        server.send_message(ErrorMessage(ErrorMessage.UserDoesNotExist))
        return
    if len(msg.password) > 0:
        target_user.set_password(msg.password)
    target_user.set_privileges(msg.privileges)
    target_user.save()
    debug("updated user {0}".format(msg.username))

@must_have_privilege(Privilege.ManageUsers)
def handle_DeleteUser(msg):
    debug("Got delete user message")
    try:
        admin.delete_user(msg.username)
    except admin.UserDoesNotExist:
        warning("user {0} does not exist, sending error message".format(msg.username))
        server.send_message(ErrorMessage(ErrorMessage.UserDoesNotExist))
        return
    debug("deleted user {0}".format(msg.username))

@must_have_privilege(Privilege.ManageUsers)
def handle_ListUserRequest(msg):
    debug("Got list user request message")
    server.send_message(ListUserResult(admin.list_users()))

@must_have_privilege(Privilege.OperateHardware)
def handle_SetAutoFocusEnabled(msg):
    global auto_focus_enabled
    auto_focus_enabled = msg.value
    if auto_focus_enabled:
        camera.setAFMode(edsdk.AFMode.OneShotAF)
    else:
        camera.setAFMode(edsdk.AFMode.ManualFocus)
    
    maybe_auto_focus()

@must_have_privilege(Privilege.ManageUsers)
def handle_SetStaticBookmarks(msg):
    admin.set_static_bookmarks(msg.bookmarks)

@must_have_privilege(Privilege.OperateHardware)
def handle_SetUserBookmarks(msg):
    user.set_bookmarks(msg.bookmarks)
    user.save()

def motorStoppedMovingHandler(reason):
    if reason != silverpak.StoppedMovingReason.Normal:
        return
    
    debug("a motor stopped moving.")
    maybe_auto_focus()

def maybe_auto_focus():
    if auto_focus_enabled and camera is not None:
        debug("camera.autoFocus()")
        camera.autoFocus()

def handle_Ping(msg):
    server.send_message(Pong(msg.ping_id))

@must_have_privilege(Privilege.OperateHardware)
def handle_ChangeFocusLocation(msg):
    if camera is not None:
        w, h = camera.maxZoomPosition()
        x, y = (int(msg.focus_x * w), int(msg.focus_y * h))
        debug("Setting zoom position to {0}, {1}".format(x,y))
        camera.setZoomPosition(x, y)
        if auto_focus_enabled:
            camera.autoFocus()

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
    ClientMessage.Ping: handle_Ping,
    ClientMessage.SetStaticBookmarks: handle_SetStaticBookmarks,
    ClientMessage.SetUserBookmarks: handle_SetUserBookmarks,
    ClientMessage.ChangeFocusLocation: handle_ChangeFocusLocation,
}

def init_state():
    # initialize variables
    global camera
    camera = None

    global finished
    finished = False

    global user
    user = None

    global init_camera_thread
    init_camera_thread = None

    global live_view_thread
    live_view_thread = None

    global message_thread
    message_thread = None

    global motors_thread
    motors_thread = None

    global motor_chars
    motor_chars = ['A', 'B', 'X', 'Y', 'Z']

    global motors
    motors = None

    global motor_movement_queue
    motor_movement_queue = queue.Queue()

    global motor_is_initialized
    motor_is_initialized = {char: False for char in motor_chars}

    global message_queue
    message_queue = queue.Queue()

    global ping_thread
    ping_thread = None

    global auto_focus_enabled
    auto_focus_enabled = True


def start_message_loop():
    global message_thread
    message_thread = make_thread(run_message_loop, "message handler")
    message_thread.start()

def start_server():
    init_state()
    set_power_switch(False)

    def handle_message(msg):
        global last_client_message_time
        last_client_message_time = time.time()
        message_queue.put(msg)

    # wait for a connection
    class _Server(socketserver.StreamRequestHandler):
        def handle(self):
            debug("_Server.handle called")
            global server
            server = Server(handle_message, on_connection_open, on_connection_close)
            self.server = server
            self.server.request = self.request
            self.server.open()

            self.server.wait()
        def finish(self):
            if on_connection_close:
                on_connection_close()

    _server = socketserver.TCPServer((settings['HOST'], settings['PORT']), _Server)

    global server_thread
    server_thread = make_thread(_server.serve_forever, "socket server")
    server_thread.start()

    start_message_loop()
    debug("serving on {0} {1}".format(settings['HOST'], settings['PORT']))

def run_message_loop():
    global message_handlers, message_queue, finished

    debug("entering message loop")
    while not finished:
        msg = message_queue.get(block=True)

        # check for message indicating thread is done
        if msg.message_type == ClientMessage.DummyCloseConnection:
            return

        message_handlers[msg.message_type](msg)

def run_connection_monitor():
    global last_client_message_time
    global finished
    debug("running connection monitor thread")

    last_client_message_time = time.time()
    while not finished:
        now = time.time()
        if now - last_client_message_time > settings['CLIENT_IDLE_TIMEOUT']:
            warning("Haven't heard from the client in {0} sec. Killing connection.".format(settings['CLIENT_IDLE_TIMEOUT']))
            finished = True
            global server
            server.close()
            return
        time.sleep(1)

def run_init_camera():
    # try for 10 seconds to get the camera
    fakeCameraImagePath = settings['FAKE_CAMERA_IMAGE_PATH']
    if fakeCameraImagePath != None:
        debug("Using fake camera")
        global camera
        camera = edsdk.getFakeCamera(fakeCameraImagePath)
    else:
        debug("Finding camera")
        tries_left = 10
        def found_camera(cam):
            if cam is None:
                # try again
                nonlocal tries_left
                tries_left -= 1
                if tries_left >= 1:
                    debug("Unable to find camera, waiting a second and trying again")
                    time.sleep(1)
                    edsdk.getFirstCamera(found_camera)
                else:
                    debug("Unable to find camera, giving up.")
                    return
            else:
                global camera
                camera = cam

                def takePictureCallback(pic_file):
                    debug("got picture callback")
                    # create a thumbnail
                    make_thumbnail(pic_file, pic_file+".thumb", 90)
                    # notify change
                    send_directory_list()
                camera.setPictureCompleteCallback(takePictureCallback)

                camera.setMeteringMode(edsdk.MeteringMode.SpotMetering)
                camera.setDriveMode(edsdk.DriveMode.SingleFrameShooting)

                global live_view_thread
                live_view_thread = make_thread(run_live_view_thread, "live_view")
                live_view_thread.start()

        edsdk.getFirstCamera(found_camera)

def run_live_view_thread():
    debug("camera.startLiveView()")
    global camera
    camera.startLiveView()
    camera.grabLiveViewFrame()

    debug("entering loop to send live view frames")
    global finished
    while not finished:
        if motors is None:
            motor_positions = {char: 0 for char in motor_chars}
            motor_states = {char: 0 for char in motor_chars}
        else:
            motor_positions = {char: motor.position() for char, motor in motors.items()}
            motor_states = {char: int(motor_is_initialized[char]) for char, motor in motors.items()}
        server.send_message(FullUpdate(camera.liveViewMemoryView(), motor_positions, motor_states))
        camera.grabLiveViewFrame()
        time.sleep(0.20)

    # shut down the camera
    camera.disconnect()
    camera = None
    edsdk.terminate()

def on_connection_open():
    debug("connection opening")
    global finished
    finished = False
    init_monitor_thread()

def on_connection_close():
    debug("connection closing")

    # send motors home
    if motors is not None:
        for char, motor in motors.items():
            motor.stoppedMovingHandlers.remove(motorStoppedMovingHandler)
            end_position = settings['MOTOR_%s_END_POSITION' % char]
            if end_position is not None:
                debug("sending motor " + repr(motor.name) + " home to " + repr(end_position))
                move_motor(motor, end_position)

    # clean up
    global finished
    finished = True

    if live_view_thread is not None:
        debug("waiting for live view thread to join")
        live_view_thread.join()

    if motors_thread is not None:
        debug("waiting for motors thread to join")
        motor_movement_queue.put({})
        motors_thread.join()

    if monitor_thread is not None:
        debug("waiting for monitor thread to join")
        monitor_thread.join()

    if message_thread is not None:
        debug("waiting for message thread to join")
        message_queue.put(DummyCloseConnection())
        message_thread.join()

    if motors is not None:
        debug("shutting down motors")
        motor_disposer_threads = []
        def shutdown_motor(motor):
            debug("waiting for motor " + repr(motor.name) + " to stop moving")
            for _ in range(settings['MOTOR_MOVEMENT_TIMEOUT']):
                if motor.isReady():
                    break
                time.sleep(1)
            else:
                error("timed out waiting for motor " + repr(motor.name) + " to stop moving")
            debug("disposing motor " + repr(motor.name))
            motor.dispose()
        for motor in motors.values():
            thread = make_thread(shutdown_motor, "motor " + repr(motor.name) + " disposer", args=[motor])
            thread.start()
            motor_disposer_threads.append(thread)
        for thread in motor_disposer_threads:
            thread.join()

    set_power_switch(on=False)

    debug("done with life. comitting seppuku")
    os._exit(0)

def init_monitor_thread():
    global monitor_thread

    monitor_thread = make_thread(run_connection_monitor, "monitor")
    monitor_thread.start()

def initialize_hardware():
    set_power_switch(on=True)

    global motors_thread
    motors_thread = make_thread(run_motors, "motors")
    motors_thread.start()

    global init_camera_thread
    init_camera_thread = make_thread(run_init_camera, "camera")
    init_camera_thread.start()

def run_motors():
    def create_motor(char):
        motor = silverpak.Silverpak()
        motor.baudRate = settings['MOTOR_%s_BAUD_RATE' % char]
        motor.driverAddress = settings['MOTOR_%s_DRIVER_ADDRESS' % char]
        motor.enableLimitSwitches = settings['MOTOR_%s_ENABLE_LIMIT_SWITCHES' % char]
        motor.enablePositionCorrection = settings['MOTOR_%s_ENABLE_POSITION_CORRECTION' % char]
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
            for _ in range(15):
                found[char] = motor.findAndConnect()
                if found[char]:
                    break
                warning("Unable to find and connect to silverpak motor '{0}', waiting a sec and trying again".format(char))
                time.sleep(1)
                if finished:
                    # never mind
                    return
            else:
                error("Unable to find and connect to silverpak motor " + repr(char))
                return
            # success
            debug("found motor " + repr(char))
            def done_initializing_handler(*args):
                motor.stoppedMovingHandlers.remove(done_initializing_handler)
                motor_is_initialized[char] = True
            motor.stoppedMovingHandlers.append(done_initializing_handler)
            motor.fullInit()
            # wait for initialization
            debug("waiting for motor " + repr(char) + " to initialize")
            for _ in range(settings['MOTOR_MOVEMENT_TIMEOUT']):
                if motor_is_initialized[char]:
                    break
                time.sleep(1)
            else:
                error("timed out waiting for motor " + repr(char) + " to initialize")
                return
            debug("motor " + repr(char) + " is done initializing")
        thread = make_thread(find_motor, "motor " + repr(char) + " finder", args=[char, motor])
        thread.start()
        threads.append(thread)
    for thread in threads:
        thread.join()
    if not all(found.values()):
        error("Unable to find and connect to all motors.")
        return

    # success
    debug("entering motor movement queue loop")
    while not finished:
        motor_positions = motor_movement_queue.get()
        debug("moving {0} motors".format(len(motor_positions)))
        for char, position in motor_positions.items():
            move_motor(motors[char], position)

if __name__ == "__main__":
    start_server()

