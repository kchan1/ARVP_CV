CC=g++
CMAIN=TargetFindTest
CFLAGS=-I/usr/local/include
LFLAGS=-I/usr/local/lib
LLIBS=-lgsl -lgslcblas -lm -ljpeg
DEPS= ARVP_JPEG.hpp imgformat.hpp imgops.hpp SyncQueue.hpp LinkedList.hpp
$(CMAIN): $(CMAIN).o
	LD_LIBRARY_PATH=/usr/local/lib
	export LD_LIBRARY_PATH
	$(CC) $(LFLAGS) $(CMAIN).o $(LLIBS) -o $(CMAIN)
%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) -Wall
