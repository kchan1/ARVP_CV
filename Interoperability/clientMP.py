import socket
import time

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#server_address = ('155.246.216.102', 10005)
server_address = ('localhost', 10008)
sock.connect(server_address)

#message = 'Latitude: ' + str(cs.lat) + '\nLongitude: ' + str(cs.lng) + '\nAltitude: ' + str(cs.alt) + '\nRoll: ' + str(cs.roll) + '\nPitch: ' + str(cs.pitch) + '\nYaw: ' + str(cs.yaw) + '\nGroundcourse: ' + str(cs.groundcourse)
for x in range(1,100):
    message = 'latitude=' + str(cs.lat) + '&longitude=' + str(cs.lng) + '&altitude_msl=' + str(cs.altasl) + '&uas_heading=' + str(cs.groundcourse)
    sock.sendall(message)
    time.sleep(0.1)
