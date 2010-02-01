CC=g++
FLAGS=`sdl-config --cflags` -Wall -g -ggdb
LINKFLAGS=-O0
LIBS=`sdl-config --libs` -lSDL_ttf
CPPFILES=Main.cpp SharedData.cpp Timer.cpp Board.cpp Row.cpp Player.cpp

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
