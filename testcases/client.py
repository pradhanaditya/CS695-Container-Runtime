#!/usr/bin/env python3
import socket
import sys
import time
import random
from _thread import *
from threading import Thread, Lock

# PORT at which the server runs
SERVER_PORT = 1234

# Some of the requests which are CPU intensive and take more time to process at the server end 
load_list = [22,23,24]

# Client thread will execute this function
def client_thread(SERVER_IP):
    # Notifying this thread about all the relevant global variables
    global load_list

    # List of all sockets
    socketsList = []
    
    try:
        # Creating and appending the socket to the sockets list
        socketsList.append(socket.socket(socket.AF_INET, socket.SOCK_STREAM))

        # Connecting to the server
        socketsList[len(socketsList) - 1].connect((SERVER_IP, SERVER_PORT))

        while True:
            # Send requests to server following the round-robin policy
            for i in range(0, len(socketsList)):
                socketsList[i].sendall(str(random.choice(load_list)).encode('utf-8'))
                response = socketsList[i].recv(1024)
                result = int(response.decode('utf-8'))
                print("Received the result : " + str(result))
                time.sleep(1.5)
    except:
        print("Error occurred while creating socket!")

# The main thread will start other threads
def Main():
    print("Server IP : " + str(sys.argv[1]))
    client = Thread(target=client_thread, args=(str(sys.argv[1]), ))
    client.start()
    # The main thread will wait for the client thread to complete its execution
    client.join()

if __name__ == '__main__':
    Main()