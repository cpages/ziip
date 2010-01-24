CC=g++
FLAGS=`sdl-config --cflags` -Wall -g -ggdb
LINKFLAGS=-O0
LIBS=`sdl-config --libs`
CPPFILES=Main.cpp SharedData.cpp Timer.cpp

SOURCES=$(CPPFILES)
OBJECTS=$(CPPFILES:.cpp=.o)
PROGRAM=ziip

all: $(OBJECTS) $(PROGRAM)


$(PROGRAM): $(OBJECTS)
	$(CC) $(OBJECTS) $(LINKFLAGS) -o $(PROGRAM) $(LIBS)


%.o: %.cpp
	$(CC) -o $@ $(FLAGS) -c $<


clean:
	rm -r *.o $(PROGRAM)
