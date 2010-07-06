TARGET=slava
HEADERS=$(wildcard *.h)
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

CXXFLAGS+=-Wall -I/usr/include/lua5.1 `sdl-config --cflags` -I../libavionics -I../libogl -m32 -ggdb
LNFLAGS+=-m32 -L. -L../libavionics -L../libogl -ggdb  -L/usr/lib32
LIBS+=-llua5.1 -lm `sdl-config --libs` -lxavionics -logl -lSOIL32 -lGL

all: $(TARGET)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<
	
$(TARGET): $(OBJECTS) ../libavionics/libxavionics.a ../libogl/libogl.a
	$(CXX) -o $(TARGET) $(LNFLAGS) $(OBJECTS) $(LIBS)

clean:
	rm -f $(OBJECTS) $(TARGET)

install: $(TARGET)
	cp -f $(TARGET) $(PLUGINS)

run: $(TARGET)
	./$(TARGET) --secret supersecret --data ../data --panel ../examples/Tu-104BN/avionics.lua

        
