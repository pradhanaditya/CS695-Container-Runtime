#!/usr/bin/env python3
import socket

IP = '0.0.0.0'

# Server listens on this port
PORT = 1234

def fibonacci(num):
    if (num == 0):
        return 0
    elif (num == 1):
        return 1
    else:
        return fibonacci(num - 1) + fibonacci(num - 2)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind((IP, PORT))
    s.listen()
    while True:
        conn, addr = s.accept()
        with conn:
            print("Connected by: " + repr(addr))
            while True:
                requestData = conn.recv(1024)
                if not requestData:
                    break
                num = int(requestData.decode('utf-8'))
                conn.sendall(str(fibonacci(num)).encode('utf-8'))

