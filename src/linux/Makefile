CC=g++
CFLAGS=`sdl-config --cflags` -I. -IGL -DUSE_GLEE
LDFLAGS=-L/usr/lib `sdl-config --libs` -lGL
SOURCES=ScreenBuffer.cpp CameraPlane.cpp Cloud.cpp PerlinNoise.cpp Primitive.cpp Ray.cpp Raytracer.cpp Scene.cpp  Sky.cpp Utils.cpp main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=pgr-sky

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

run:
	./prg-sky	

.PHONY: clean clean-exe clean-all
clean:
	rm -f *~ *.bak *.o

clean-exe:
	rm -f $(EXECUTABLE)

clean-all: clean-exe clean
