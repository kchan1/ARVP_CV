import Tkinter
from ScrolledText import *
import time
import threading
import random
import Queue
import socket
import pycurl
import json
from StringIO import StringIO

# Setup
interop_api_url = 'http://155.246.210.164/api/'

class GuiPart:
    def __init__(self, master, queue, endCommand):
        self.queue = queue
        self.msg = ""
        
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
        obs_but = Tkinter.Button(master, text='Retreive Obstacles From Server', command=getObstacles)
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
        
        # Add more GUI stuff here

    def processIncoming(self):
        """
        Handle all the messages currently in the queue (if any).
        """
        while self.queue.qsize():
            try:
                self.msg = str(self.queue.get(0)) + '\n'
                # Check contents of message and do what it says
                # As a test, we simply print it
                self.tele.insert('end', self.msg)
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
        obstacles = json.dumps(obstacles_json, sort_keys=True,indent=4, separators=(',', ': ')
        print obstacles
        self.obs.insert('end', obstacles)
        

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

        # Set up the GUI part
        self.gui = GuiPart(master, self.queue, self.endApplication)

        # Set up the thread to do asynchronous I/O
        # More can be made if necessary
        self.running = 1
        self.thread1 = threading.Thread(target=self.workerThread1)
        self.thread1.start()

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
            time.sleep(rand.random() * 0.3)
            msg = rand.random()
            self.queue.put(msg)
            '''sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server_address = ('localhost', 10005)
            sock.bind(server_address)

            sock.listen(1)
            while self.running:
                print 'Waiting for connection'
                connection, client_address = sock.accept()
                while self.running:
                    data = connection.recv(200)
                    self.queue.put(data)
                    #print data
                    sock.close()'''
    
    def endApplication(self):
        self.running = 0

# Send POST login session cookie

c = pycurl.Curl()

c.setopt(pycurl.URL, interop_api_url+'login')
c.setopt(pycurl.POSTFIELDS, 'username=testadmin&password=testpass')
c.setopt(pycurl.COOKIEJAR, 'sessionid.txt')
c.perform()

rand = random.Random()
root = Tkinter.Tk()

client = ThreadedClient(root)
root.mainloop()
