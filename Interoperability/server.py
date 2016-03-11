import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = ('155.246.216.102',10005)
sock.bind(server_address)

sock.listen(1)

while True:
    print 'Waiting'
    connection, client_address = sock.accept()
    while True:
        data = connection.recv(200)
        print data
        sock.close()
