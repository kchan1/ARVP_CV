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
import urllib2
import datetime
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
        self.rt.grid(row=0, column=3, columnspan=2, padx=5, pady=5)

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
        ut_but = Tkinter.Button(master, text='Upload Target Data', command=self.uploadTarget)
        ut_but.grid(row=1, column=3, columnspan=2, sticky=Tkinter.W)

        #Receiving Obstacles Window
        self.obs = ScrolledText(master, width=50, height=10, wrap='word')
        self.obs.insert('end', 'Ready for Obstacle Data...\n--------------------------\n')
        self.obs.grid(row=2, column=1, columnspan=1, padx=5, pady=5)

        #General Terminal Window
        self.gt = ScrolledText(master, width=50, height=10, wrap='word')
        self.gt.insert('end', 'Enter in Server Query...\n------------------------\n')
        self.gt.grid(row=2, column=2, columnspan=1, padx=5, pady=5)

        #Upload Targets Window
        #self.ut = ScrolledText(master, width=50, height=10, wrap='word')
        #self.ut.insert('end', 'Enter in Target Data...\n------------------------\n')
        
        target_label_text = ["Target Type:    ","Latitude: ","Longitude:  ","Orientation:", \
                           "Shape: ", "BG Color: ", "Letter/#:  ","Letter/# Color: "]
        targetlabelpane = Tkinter.PanedWindow(orient=Tkinter.VERTICAL)
        for i in target_label_text:
            targetlabelpane.add(Tkinter.Label(master, text=i,pady=3))
        targetlabelpane.grid(row=2, column=3,sticky=Tkinter.W)
        
        # Use combobox if user entry is also required
        # Here we use optionmenu
        self.available_target_type = Tkinter.StringVar() #["standard", "qrc", "off_axis", "emergent"]
        self.available_target_type.set("standard")
        self.available_orientation = Tkinter.StringVar() #["N","E","S","W","NE","SE","SW","NW"]
        self.available_orientation.set("N")
        self.available_shapes = Tkinter.StringVar()      #["circle","semicircle","quarter_circle","triangle","square","rectangle","trapezoid","pentagon","hexagon","heptagon","octagon","star","cross"]
        self.available_shapes.set("circle")
        self.available_bg_colors = Tkinter.StringVar()   #["white","black","gray","red","blue","green","yellow","purple","brown","orange"]
        self.available_bg_colors.set("white")
        self.available_alpha_colors = Tkinter.StringVar()
        self.available_alpha_colors.set("white")

        '''
        drop_target_type = Tkinter.OptionMenu(master,available_target_type,"standard", "qrc", "off_axis", "emergent")
        entry_lat = Tkinter.Entry(master, bd=5, width=70)
        entry_lon = Tkinter.Entry(master, bd=5, width=70)
        drop_orientation = Tkinter.OptionMenu(master,available_orientation,"N","E","S","W","NE","SE","SW","NW")
        drop_shapes = Tkinter.OptionMenu(master,available_shapes,"circle","semicircle","quarter_circle","triangle","square","rectangle","trapezoid","pentagon","hexagon","heptagon","octagon","star","cross")
        drop_bg_colors = Tkinter.OptionMenu(master,available_bg_colors,"white","black","gray","red","blue","green","yellow","purple","brown","orange")
        entry_alphanum = Tkinter.Entry(master, bd=5, width=70)
        drop_alpha_colors= Tkinter.OptionMenu(master,available_alpha_colors,"white","black","gray","red","blue","green","yellow","purple","brown","orange")
        '''
        
        targetpane = Tkinter.PanedWindow(orient=Tkinter.VERTICAL)
        targetpane.add(Tkinter.OptionMenu(master,self.available_target_type,"standard", "qrc", "off_axis", "emergent"))
        self.entry_lat = Tkinter.Entry(master, width=10)
        targetpane.add(self.entry_lat)
        self.entry_lon = Tkinter.Entry(master,width=10)
        targetpane.add(self.entry_lon)
        targetpane.add(Tkinter.OptionMenu(master,self.available_orientation,"N","E","S","W","NE","SE","SW","NW"))
        targetpane.add(Tkinter.OptionMenu(master,self.available_shapes,"circle","semicircle","quarter_circle","triangle","square","rectangle","trapezoid","pentagon","hexagon","heptagon","octagon","star","cross"))
        targetpane.add(Tkinter.OptionMenu(master,self.available_bg_colors,"white","black","gray","red","blue","green","yellow","purple","brown","orange"))
        self.entry_alphanum = Tkinter.Entry(master, width=10)
        targetpane.add(self.entry_alphanum)
        targetpane.add(Tkinter.OptionMenu(master,self.available_alpha_colors,"white","black","gray","red","blue","green","yellow","purple","brown","orange"))
    
        targetpane.grid(row=2, column=3, padx=5, pady=5)

        #General Terminal Text Area
        self.gt_text = Tkinter.Entry(master,width=70)
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
            self.cred_text.append((Tkinter.Entry(master,width=50)))
            credpane[i].add(self.cred_label[i])
            credpane[i].add(self.cred_text[i])
            credpane[i].grid(row=5+i, column=1, columnspan=1, padx=5, pady=5, sticky=Tkinter.E)
        self.cred_but = Tkinter.Button(master, text='Login', command=self.loginWithCred)
        self.cred_but.grid(row=8, column=1, columnspan=1, padx=5, pady=5)
        # Add more GUI stuff here

    def processIncoming(self):
        """
        Handle all the messages currently in the queue (if any).
        """
        #self.counter = 0
        #self.queue.put('asdas')
        while self.queue.qsize():
            try:
                self.msg = str(self.queue.get(0)) + '\n'
                # Check contents of message and do what it says
                # As a test, we simply print it
                self.tele.insert('end', self.msg)
                if(float(self.tele.index('end')) > 11.0):
                    self.tele.delete('1.0','end')
                self.msg2 = self.queue2.get(0) + '\n'
                #print self.msg2
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
        #print interop_api_url
        c = pycurl.Curl()
        c.setopt(pycurl.URL, interop_api_url+'login')
        c.setopt(pycurl.POSTFIELDS, 'username='+self.cred_text[1].get()+'&password='+self.cred_text[2].get())
        c.setopt(pycurl.COOKIEJAR, 'sessionid.txt')
        c.perform()

    def uploadTarget(self):
        global interop_api_url
        target_data_dict = { \
            "type": self.available_target_type.get(), \
            "latitude": float(self.entry_lat.get()), \
            "longitude": float(self.entry_lon.get()), \
            "orientation": self.available_orientation.get(), \
            "shape": self.available_shapes.get(), \
            "background_color": self.available_bg_colors.get(), \
            "alphanumeric": self.entry_alphanum.get(), \
            "alphanumeric_color": self.available_alpha_colors.get(), \
        }
        json_target_data = json.dumps(target_data_dict)
        c = pycurl.Curl()
        c.setopt(pycurl.URL, interop_api_url+'targets')
        c.setopt(pycurl.POSTFIELDS, json_target_data)
        c.setopt(pycurl.COOKIEFILE, 'sessionid.txt')
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
        self.queue3 = Queue.Queue()

        # Set up the GUI part
        self.gui = GuiPart(master, self.queue, self.queue2, self.endApplication)

        # Set up the thread to do asynchronous I/O
        # More can be made if necessary
        self.running = 1
        self.locTime = {}

        #Recieving and sending telemetry data
        self.thread1 = threading.Thread(target=self.workerThread1)
        self.thread1.daemon = True
        self.thread1.start()

        #Receiving target data
        self.thread2 = threading.Thread(target=self.workerThread2)
        self.thread2.daemon = True
        self.thread2.start()

        #Collecting location data
        self.thread3 = threading.Thread(target=self.workerThread3)
        self.thread3.daemon = True
        self.thread3.start()

        #Processing location info for targets
        self.thread4 = threading.Thread(target=self.workerThread4)
        self.thread4.daemon = True
        self.thread4.start()
        
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

            mpurl = 'http://127.0.0.1:56781/mavlink/'

            while self.running:

                self.queue.put('Waiting for connection')
                '''#print datetime.datetime.now().time()
                telemdata = json.load(urllib2.urlopen(mpurl))
                
                #print telemdata['lat'] + " " + telemdata['lon']
                telemdatastr = "latitude="+str(telemdata['GPS_RAW_INT']['msg']['lat']/float(10000000))+ \
                      "&longitude="+str(telemdata['GPS_RAW_INT']['msg']['lon']/float(10000000)) + \
                      "&altitude_msl="+str(telemdata['VFR_HUD']['msg']['alt'])+ \
                      "&uas_heading="+str(telemdata['VFR_HUD']['msg']['heading'])
                self.queue.put(telemdatastr)'''
                '''try:
                    c = pycurl.Curl()
                    buffer = StringIO()
                    c.setopt(pycurl.URL, interop_api_url+'server_info')
                    c.setopt(pycurl.WRITEDATA, buffer)
                    c.setopt(pycurl.COOKIEFILE, 'sessionid.txt')
                    c.setopt(pycurl.CONNECTTIMEOUT, 10)
                    c.perform()
                    self.queue.put('Server Time:\n' + buffer.getvalue())
                except:
                    self.queue.put("S_Time: Connection failure")
                try:
                    c = pycurl.Curl()
                    c.setopt(pycurl.URL, interop_api_url+'telemetry')
                    c.setopt(pycurl.POSTFIELDS, telemdatastr)
                    c.setopt(pycurl.COOKIEFILE, 'sessionid.txt')
                    c.setopt(pycurl.CONNECTTIMEOUT, 10)
                    c.perform()
                    #self.queue.put(telemdatastr)
                except:
                    self.queue.put("S_Telem: Connection failure")'''
                time.sleep(0.1)

    def workerThread2(self):
        
        '''s = socket.socket()         # Create a socket object
        host = 'localhost' # Get local machine name
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
            #Add received info to queue3
            
        s.close()'''
        path = 'C:\Users\JWong\Documents\Photos'
        tempnew = []
        newfiles = []
        oldfiles = []

        while True:
            
            for root, dirs, filenames in os.walk(path):

                for f in filenames:
                    tempnew.append(os.path.relpath(os.path.join(root, f), path))
                    
                newfiles = tempnew
                tempnew = []

                if not oldfiles:
                    oldfiles = newfiles

                if cmp(newfiles, oldfiles) != 0:
                    print 'Found new file!'
                    time.sleep(10)
                    nf = set(newfiles)
                    of = set(oldfiles)
                    delta = nf.difference(of)
                    for x in delta:
                        print x[:len(x) - 4]
                        #self.queue3.put(x)

                oldfiles = newfiles

            time.sleep(5)
            print 'Nothing new!'
            #print cmp(newfiles, oldfiles)
            #print newfiles
        
    def workerThread3(self):

        locInterval = 5     #Seconds
        mpurl = 'http://127.0.0.1:56781/mavlink/'

        #Syncing time stuff goes here
        targetTime = datetime.time(15,33,40,405000)
        while datetime.datetime.now().time() < targetTime and self.running:
            print "Waiting for start"
            
        while self.running:
            if datetime.datetime.now().time().second % locInterval == 0:
                print "test"
                #telemdata = json.load(urllib2.urlopen(mpurl))
                latdata = "100" #str(telemdata['GPS_RAW_INT']['msg']['lat']/float(10000000))
                longdata = "50" #str(telemdata['GPS_RAW_INT']['msg']['lon']/float(10000000))
                altdata = "200" #str(telemdata['VFR_HUD']['msg']['alt'])
                uasdata = "20" #str(telemdata['VFR_HUD']['msg']['heading'])
                currTime = datetime.datetime.now().time()
                key = str(currTime.hour) + ':' + str(currTime.minute) + ':' + str(int(currTime.second / 5) * 5)
                self.locTime[key] = str(latdata) + ',' + str(longdata) + ',' + str(altdata) + ',' + str(uasdata)
                #print key
                self.queue3.put(key + ',asdsad')    #Tests
                time.sleep(1)   #Ensure there is no overlap

    def workerThread4(self):

        locInfo = ''

        while self.running:
            try:
                #Stack order: Timestamp -> X and Y Coords of Target in Pic
                #print 'test2'
                newTargets = self.queue3.get(0)
                newTargetsParsed = newTargets[:len(newTargets) - 4].split(',')

                sorted(self.locTime)

                for key in self.locTime:
                    if key == newTargetsParsed[0]:
                        locInfo = self.locTime[key].split(',')
                        #del self.locTime[key]
                        break
                    #else:
                    #    del self.locTime[key]

                self.locationProcessing(newTargetsParsed, locInfo)  #Send in lists
                time.sleep(10)

            except Queue.Empty:
                pass
    
    def locationProcessing(self, targetData, locData):
        print targetData
        print locData
        #Add location processing code

    def endApplication(self):
        self.running = 0

root = Tkinter.Tk()

client = ThreadedClient(root)
root.mainloop()
