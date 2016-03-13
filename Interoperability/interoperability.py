# curl --data "username=testadmin&password=testpass" -c sessioncook -X POST http://ipaddr/api/login

import pycurl
import json
from StringIO import StringIO
import socket
import time

# Setup
interop_api_url = 'http://155.246.210.164/api/'

# Send POST login session cookie

c = pycurl.Curl()

c.setopt(pycurl.URL, interop_api_url+'login')
c.setopt(pycurl.POSTFIELDS, 'username=testadmin&password=testpass')
c.setopt(pycurl.COOKIEJAR, 'sessionid.txt')
c.perform()

# GET obstacles request

c = pycurl.Curl()

buffer = StringIO()
c.setopt(pycurl.URL, interop_api_url+'obstacles')
c.setopt(pycurl.WRITEDATA, buffer)
c.setopt(pycurl.COOKIEFILE, 'sessionid.txt')
c.perform()

obstacles_json = json.loads(buffer.getvalue())

print("\n\n" + "====Obstacles====")
print json.dumps(obstacles_json, sort_keys=True,indent=4, separators=(',', ': '))
#print buffer.getvalue()

#TO BE DONE: UPLOAD OBSTACLES TO MP

# Send telemetry to server

import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = ('155.246.216.102',10005)
sock.bind(server_address)

sock.listen(1)

while True:
    print 'Waiting'
    connection, client_address = sock.accept()
    for x in range(1,10):
        data = connection.recv(200)
        
        # Initial server_info GET request

        c = pycurl.Curl()

        buffer = StringIO()
        c.setopt(pycurl.URL, interop_api_url+'server_info')
        c.setopt(pycurl.WRITEDATA, buffer)
        c.setopt(pycurl.COOKIEFILE, 'sessionid.txt')
        c.perform()

        print("====Server Info====")
        print(buffer.getvalue())
        
        c = pycurl.Curl()
        c.setopt(pycurl.URL, interop_api_url+'telemetry')
        #c.setopt(pycurl.POSTFIELDS, 'latitude=38.149&longitude=-76.432&altitude_msl=100&uas_heading=90')
        c.setopt(pycurl.POSTFIELDS, data)
        c.setopt(pycurl.COOKIEFILE, 'sessionid.txt')
        c.perform()
        print '\n' + data + '\n'
    sock.close()
    break

#ALSO TBD: CREATE LOOP TO LISTEN FOR FOUND TARGETS FROM PI
