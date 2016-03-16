import Tkinter
from ScrolledText import *
import os
import time
import threading
import random
import Queue
import socket
import pycurl
import json
from StringIO import StringIO

# Setup
interop_api_url = ''
sock = None

class GuiPart:
    def __init__(self, master, queue, queue2, endCommand):
        self.queue = queue
        self.queue2 = queue2
        self.msg = ""
        self.targetpics = []
        self.currentImage = -1
        
        # Set up the GUI
        console = Tkinter.Button(master, text='Done', command=endCommand)
        console.grid(row=3, column=1, sticky=Tkinter.W)

        #Telemetry Window
        self.tele = ScrolledText(master, width=50, height=10, wrap='word')
        self.tele.insert('end', 'Awaiting Telemetry...\n----------------------\n')
        self.tele.grid(row=0, column=1, columnspan=1, padx=5, pady=5)

        #Receiving Targets Window
        self.rt = ScrolledText(master, width=50, height=10, wrap='word')
        self.rt.insert('end', 'Awaiting Target Data...\n------------------------\n')
        self.rt.grid(row=0, column=3, columnspan=1, padx=5, pady=5)

        #Image
        img = Tkinter.PhotoImage(file="logo.gif")
        logo = Tkinter.Label(master, image = img)
        logo.image=img
        logo.grid(row=0, column=2, padx=5, pady=5)

        #Obstacles Button
        obs_but = Tkinter.Button(master, text='Retreive Obstacles From Server', command=self.getObstacles)
        obs_but.grid(row=1, column=1, sticky=Tkinter.W)

        #General Terminal Button
        gt_but = Tkinter.Button(master, text='Submit Data', command=endCommand)
        gt_but.grid(row=1, column=2, sticky=Tkinter.W)

        #Upload Targets Button
        ut_but = Tkinter.Button(master, text='Upload Target Data', command=endCommand)
        ut_but.grid(row=1, column=3, sticky=Tkinter.W)

        #Receiving Obstacles Window
        self.obs = ScrolledText(master, width=50, height=10, wrap='word')
        self.obs.insert('end', 'Ready for Obstacle Data...\n--------------------------\n')
        self.obs.grid(row=2, column=1, columnspan=1, padx=5, pady=5)

        #General Terminal Window
        self.gt = ScrolledText(master, width=50, height=10, wrap='word')
        self.gt.insert('end', 'Enter in Server Query...\n------------------------\n')
        self.gt.grid(row=2, column=2, columnspan=1, padx=5, pady=5)

        #Upload Targets Window
        self.ut = ScrolledText(master, width=50, height=10, wrap='word')
        self.ut.insert('end', 'Enter in Target Data...\n------------------------\n')
        self.ut.grid(row=2, column=3, columnspan=1, padx=5, pady=5)

        #General Terminal Text Area
        self.gt_text = Tkinter.Entry(master, bd=5, width=70)
        self.gt_text.grid(row=3, column=2, columnspan=1, padx=5, pady=5)

        #Target Picture Buttons
        pane = Tkinter.PanedWindow()
        rti_prev = Tkinter.Button(master, text='Prev Picture', command=self.prevPic)
        pane.add(rti_prev)
        self.rti_label = Tkinter.Label(master, text='Awaiting Target')
        rti_next = Tkinter.Button(master, text='Next Picture', command=self.nextPic)
        pane.add(rti_next)
        pane.add(self.rti_label)
        pane.grid(row=4, column=2, padx=5, pady=5)

        '''
        #Target Images
        rtimg = Tkinter.PhotoImage(file="target.gif")
        self.rti = Tkinter.Label(master, image = rtimg)
        self.rti.image=rtimg
        self.rti.grid(row=5, column=2, columnspan=1, padx=5, pady=5)
        '''

        # User credentials field
        cred_label_text = ['Server IP:    ','Username: ','Password:  ']
        self.cred_text = []
        self.cred_label = []
        credpane = []
        for i in range(0,3):
            credpane.append(Tkinter.PanedWindow())
            self.cred_label.append(Tkinter.Label(master, text=cred_label_text[i]))
            self.cred_text.append((Tkinter.Entry(master, bd=5, width=50)))
            credpane[i].add(self.cred_label[i])
            credpane[i].add(self.cred_text[i])
            credpane[i].grid(row=5+i, column=1, columnspan=1, padx=5, pady=5, sticky=Tkinter.W)
        self.cred_but = Tkinter.Button(master, text='Login', command=self.loginWithCred)
        self.cred_but.grid(row=8, column=1, columnspan=1, padx=5, pady=5)
        # Add more GUI stuff here

    def processIncoming(self):
        """
        Handle all the messages currently in the queue (if any).
        """
        #self.counter = 0
        while self.queue.qsize():
            try:
                self.msg = str(self.queue.get(0)) + '\n'
                # Check contents of message and do what it says
                # As a test, we simply print it
                self.tele.insert('end', self.msg)
                if(float(self.tele.index('end')) > 11.0):
                    self.tele.delete('1.0','end')
                self.msg2 = self.queue2.get(0) + '\n'
                if(self.msg2.find('Target') == 0):
                    #self.targetpics[self.counter] = self.msg2
                    self.targetpics.append(self.msg2)
                    #self.counter += 1
                else:
                    self.rt.insert('end', self.msg2)
            except Queue.Empty:
                pass
    
    def getObstacles(self):
        # GET obstacles request

        c = pycurl.Curl()

        buffer = StringIO()
        c.setopt(pycurl.URL, interop_api_url+'obstacles')
        c.setopt(pycurl.WRITEDATA, buffer)
        c.setopt(pycurl.COOKIEFILE, 'sessionid.txt')
        c.perform()

        obstacles_json = json.loads(buffer.getvalue())

        print("\n\n" + "====Obstacles====")
        obstacles = json.dumps(obstacles_json, sort_keys=True,indent=4, separators=(',', ': '))
        print obstacles
        self.obs.insert('end', obstacles)
    
    def nextPic(self):
        if(self.currentImage < len(self.targetpics) - 1):
            self.currentImage += 1
            self.nextImage = Tkinter.PhotoImage(file='Photos/' + self.targetpics[self.currentImage].strip())
            self.rti.configure(image=self.nextImage)
            self.rti.image = self.nextImage
            self.rti_label.configure(text=self.targetpics[self.currentImage].strip())

    def prevPic(self):
        if(self.currentImage > 0):
            self.currentImage -= 1
            self.nextImage = Tkinter.PhotoImage(file='Photos/' + self.targetpics[self.currentImage].strip())
            self.rti.configure(image=self.nextImage)
            self.rti.image = self.nextImage
            self.rti_label.configure(text=self.targetpics[self.currentImage].strip())

    def loginWithCred(self):
        global interop_api_url
        interop_api_url = "http://"+self.cred_text[0].get()+"/api/"
        print interop_api_url
        c = pycurl.Curl()
        c.setopt(pycurl.URL, interop_api_url+'login')
        c.setopt(pycurl.POSTFIELDS, 'username='+self.cred_text[1].get()+'&password='+self.cred_text[2].get())
        c.setopt(pycurl.COOKIEJAR, 'sessionid.txt')
        c.perform()

'''
        try:
            print "segsegs"
            #f = open('sessionid.txt')
            #f.close()
        except IOError, OSError:
            self.cred_but.text("Failed")
'''     
class ThreadedClient:
    """
    Launch the main part of the GUI and the worker thread. periodicCall and
    endApplication could reside in the GUI part, but putting them here
    means that you have all the thread controls in a single place.
    """
    def __init__(self, master):
        """
        Start the GUI and the asynchronous threads. We are in the main
        (original) thread of the application, which will later be used by
        the GUI. We spawn a new thread for the worker.
        """
        self.master = master

        # Create the queue
        self.queue = Queue.Queue()
        self.queue2 = Queue.Queue()

        # Set up the GUI part
        self.gui = GuiPart(master, self.queue, self.queue2, self.endApplication)

        # Set up the thread to do asynchronous I/O
        # More can be made if necessary
        self.running = 1
        self.thread1 = threading.Thread(target=self.workerThread1)
        self.thread1.daemon = True
        self.thread1.start()

        self.thread2 = threading.Thread(target=self.workerThread2)
        self.thread2.daemon = True
        self.thread2.start()
        
        # Start the periodic call in the GUI to check if the queue contains
        # anything
        self.periodicCall()

    def periodicCall(self):
        """
        Check every 100 ms if there is something new in the queue.
        """
        self.gui.processIncoming()
        if not self.running:
            # This is the brutal stop of the system. You may want to do
            # some cleanup before actually shutting it down.
            import sys
            print 'Quitting'
            self.master.destroy()
            sys.exit(1)
        self.master.after(100, self.periodicCall)

    def workerThread1(self):
        """
        This is where we handle the asynchronous I/O. For example, it may be
        a 'select()'.
        One important thing to remember is that the thread has to yield
        control.
        """
        while self.running:
            # To simulate asynchronous I/O, we create a random number at
            # random intervals. Replace the following 2 lines with the real
            # thing.

            #try:
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                server_address = ('localhost', 10008)
                sock.bind(server_address)
                sock.listen(1)
                #print interop_api_url
                while self.running:
                    #print interop_api_url
                    self.queue.put('Waiting for connection')
                    connection, client_address = sock.accept()
                    self.queue.put('Connected to Mission Planner!')
                    while self.running:
                        # Initial server_info GET request
                        c = pycurl.Curl()
                        buffer = StringIO()
                        c.setopt(pycurl.URL, interop_api_url+'server_info')
                        c.setopt(pycurl.WRITEDATA, buffer)
                        c.setopt(pycurl.COOKIEFILE, 'sessionid.txt')
                        c.perform()
                        self.queue.put('Server Time:\n' + buffer.getvalue())
                        
                        data = connection.recv(200)
                        #print str(len(data)) + '\n'
                        if(len(data) > 75):
                            data = data[:data.find('latitude',25)]
                            self.queue.put('*****Telemetry Overflow*****')
                        
                        # Send telemetry to server
                        
                        c = pycurl.Curl()
                        c.setopt(pycurl.URL, interop_api_url+'telemetry')
                        c.setopt(pycurl.POSTFIELDS, data)
                        c.setopt(pycurl.COOKIEFILE, 'sessionid.txt')
                        c.perform()
                        
                        self.queue.put(data)
                        #print data
                        sock.close()
            #except:
               # self.queue.put("Connection failure")

    def workerThread2(self):
        s = socket.socket()         # Create a socket object
        host = socket.gethostname() # Get local machine name
        port = 12352                # Reserve a port for your service.
        s.bind((host, port))        # Bind to the port

        s.listen(5)                 # Now wait for client connection.
        counter = 0
        #c, addr = s.accept()
        for x in range(1,3):        # Establish connection with client.
            c, addr = s.accept()
           #print c.recv(256)
            msg = c.recv(256).split(',')
            message = '[Target ' + str(counter + 1) + ']:\nType: ' + msg[0] + '\nLatitude: ' + msg[1] + '\nLongitude: ' + msg[2] + '\nOreintation: ' + msg[3] + '\nShape: ' + msg[4] + '\nBackground Color: ' + msg[5] + '\nAlphanumeric: ' + msg[6] + '\nAlphanumeric Color: ' + msg[7] + '\n'
            self.queue2.put(message)
            c.close()
           
            c, addr = s.accept()
            print ('Got connection from', addr)
            counter += 1
            fname = 'Photos/Target' + str(counter) + '.gif'
            self.queue2.put(fname[7:])
            fp = open(fname,'wb')
            while True:
                strng = c.recv(512)
                if not strng:
                    break
                fp.write(strng)
            fp.close()
            print "Data Received successfully"
            #print (c.recv(1024))
            c.close()                # Close the connection
        s.close()
    
    def endApplication(self):
        self.running = 0

# Send POST login session cookie

'''c = pycurl.Curl()
c.setopt(pycurl.URL, interop_api_url+'login')
c.setopt(pycurl.POSTFIELDS, 'username=testadmin&password=testpass')
c.setopt(pycurl.COOKIEJAR, 'sessionid.txt')
c.perform()'''

#rand = random.Random()
root = Tkinter.Tk()

client = ThreadedClient(root)
root.mainloop()
