# curl --data "username=testadmin&password=testpass" -c sessioncook -X POST http://ipaddr/api/login

import pycurl
from StringIO import StringIO

c = pycurl.Curl()

# Send POST login session cookie
c.setopt(pycurl.URL, 'http://155.246.210.164/api/login')
c.setopt(pycurl.POSTFIELDS, 'username=testadmin&password=testpass')
c.setopt(pycurl.COOKIEFILE, 'sessionid.txt')
c.perform()

# Initial GET request

c = pycurl.Curl()

buffer = StringIO()
c.setopt(pycurl.URL, 'http://155.246.210.164/api/server_info')
c.setopt(pycurl.WRITEDATA, buffer)
c.setopt(pycurl.COOKIEFILE, 'sessionid.txt')
c.perform()

test = buffer.getvalue()

print("\n"+test)
