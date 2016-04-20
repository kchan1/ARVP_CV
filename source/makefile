CC=g++
CMAIN=imgtestmain
CFLAGS=-I/usr/local/include
LFLAGS=-I/usr/local/lib
LLIBS=-lgsl -lgslcblas -lm -ljpeg
DEPS= ARVP_JPEG.hpp imgformat.hpp imgops.hpp SyncQueue.hpp LinkedList.hpp
$(CMAIN): $(CMAIN).o
	LD_LIBRARY_PATH=/usr/local/lib
	export LD_LIBRARY_PATH
	$(CC) $(LFLAGS) $(CMAIN).o $(LLIBS) -o $(CMAIN)
%: %.o	
	LD_LIBRARY_PATH=/usr/local/lib
	export LD_LIBRARY_PATH
	$(CC) $(LFLAGS) %.o $(LLIBS) -o %
%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) -Wall
