import socket
import time

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

client.connect(("127.0.0.1", 9999))

client.send(b"JOIN\0\0\n")

try:
    client.settimeout(1.0)
    data = client.recv(1024)
    print(data)
except:
    print("No data received")

time.sleep(1)

try:
    client.settimeout(1.0)
    data = client.recv(1024)
    print(data)
except:
    print("No data received")

time.sleep(1)

client.send(b"QUIT\0\0\n")

try:
    client.settimeout(1.0)
    data = client.recv(1024)
    print(data)
except:
    print("No data received")

client.close()