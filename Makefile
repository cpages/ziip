CC=g++
FLAGS=`sdl-config --cflags` -Wall -g -ggdb
LINKFLAGS=-O0
LIBS=`sdl-config --libs` -lSDL_ttf -lSDL_gfx -lSDL_image
CPPFILES=Main.cpp Resources.cpp Timer.cpp Board.cpp Row.cpp Player.cpp InputMgr.cpp

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
