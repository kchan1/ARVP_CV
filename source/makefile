CC=g++
CMAIN=main
CFLAGS=-I/usr/local/include
LFLAGS=-I/usr/local/lib
LLIBS=-lgsl -lgslcblas -lm -ljpeg
DEPS= *.hpp TargetFind/*.hpp KMeans/*.hpp
$(CMAIN): $(CMAIN).o
	LD_LIBRARY_PATH=/usr/local/lib
	export LD_LIBRARY_PATH
	$(CC) $(LFLAGS) $(CMAIN).o $(LLIBS) -o $(CMAIN)
%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) -Wall
%.out: %.o $(DEPS)
	LD_LIBRARY_PATH=/usr/local/lib
	export LD_LIBRARY_PATH
	$(CC) $(LFLAGS) $< $(LLIBS) -o $*
