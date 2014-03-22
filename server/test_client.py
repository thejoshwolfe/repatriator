import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(('localhost', 57051))
s.send(b'Hello')
data = s.recv(1024)
s.close()
print("Received " + repr(data))
