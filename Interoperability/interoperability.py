# curl --data "username=testadmin&password=testpass" -c sessioncook -X POST http://ipaddr/api/login

import pycurl
import json
from StringIO import StringIO

# Setup
interop_api_url = 'http://155.246.210.164/api/'

# Send POST login session cookie

c = pycurl.Curl()

c.setopt(pycurl.URL, interop_api_url+'login')
c.setopt(pycurl.POSTFIELDS, 'username=testadmin&password=testpass')
c.setopt(pycurl.COOKIEFILE, 'sessionid.txt')
c.perform()

# Initial server_info GET request

c = pycurl.Curl()

buffer = StringIO()
c.setopt(pycurl.URL, interop_api_url+'server_info')
c.setopt(pycurl.WRITEDATA, buffer)
c.setopt(pycurl.COOKIEFILE, 'sessionid.txt')
c.perform()

print("\n\n" + "====Server Info====")
print(buffer.getvalue())

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

# Send POST target 

test_target_data_string = { \
    "type": "standard", \
    "latitude": 38.1478, \
    "longitude": -76.4275, \
    "orientation": "n", \
    "shape": "star", \
    "background_color": "orange", \
    "alphanumeric": "C", \
    "alphanumeric_color": "black", \
}

test_target_data = json.dumps(test_target_data_string)

c = pycurl.Curl()

c.setopt(pycurl.URL, interop_api_url+'targets')
c.setopt(pycurl.POSTFIELDS, test_target_data)
c.setopt(pycurl.COOKIEFILE, 'sessionid.txt')
c.perform()

# GET targets request

c = pycurl.Curl()

buffer = StringIO()
c.setopt(pycurl.URL, interop_api_url+'targets')
c.setopt(pycurl.WRITEDATA, buffer)
c.setopt(pycurl.COOKIEFILE, 'sessionid.txt')
c.perform()

obstacles_json = json.loads(buffer.getvalue())

print("\n\n" + "====Targets====")
print json.dumps(obstacles_json, sort_keys=True,indent=4, separators=(',', ': '))

# Send telemetry to server
c = pycurl.Curl()

c.setopt(pycurl.URL, interop_api_url+'telemetry')
c.setopt(pycurl.POSTFIELDS, 'latitude=38.149&longitude=-76.432&altitude_msl=100&uas_heading=90')
c.setopt(pycurl.COOKIEFILE, 'sessionid.txt')
c.perform()
