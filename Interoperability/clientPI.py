#!/usr/bin/python           # This is client.py file

import socket               # Import socket module
import time
import Queue

s = socket.socket()         # Create a socket object
host = '155.246.216.102' # Get local machine name
port = 12352                # Reserve a port for your service.

#Queue to hold found targets
queue = Queue.Queue()
queue.put('standard,38.1478,-76.4275,n,star,orange,C,black,HHD00051.gif')
queue.put('standard2,38.1478,-76.4275,n,star,orange,C,black,HHD00096.gif')

for x in range(1,3):
    s = socket.socket() 
    s.connect((host, port))
    try:
        data = queue.get(0)

        s.send(data)
        s.close()

        pic = data.split(',')
        
        f = open(pic[8], "rb")
        s = socket.socket() 
        s.connect((host, port))
        while True:
            veri = f.readline(512)
            if not veri:
                break
            s.send(veri)

        f.close()
        s.shutdown(socket.SHUT_WR)
        #s.send(raw_input('Test'))
        s.close                     # Close the socket when done

        time.sleep(3)
    except Queue.Empty:
        pass
