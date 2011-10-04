include ../common.mk
    
TARGET=slava
HEADERS=$(wildcard *.h)
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

CXXFLAGS+=`sdl-config --cflags` -I../libavionics  -I../libaccgl $(LUAJIT_CXXFLAGS)
LNFLAGS+=-L../libavionics -L../libaccgl $(LUAJIT_LNFLAGS)
LIBS+=-lm `sdl-config --libs` -lavionics -laccgl -lSOIL32 -lGL $(LUAJIT_LIBS)

all: $(TARGET)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<
	
$(TARGET): $(OBJECTS) ../libavionics/libavionics.a ../libaccgl/libaccgl.a
	$(CXX) -o $(TARGET) $(LNFLAGS) $(OBJECTS) $(LIBS)

clean:
	rm -f $(OBJECTS) $(TARGET)

install: $(TARGET)
	cp -f $(TARGET) $(PLUGINS)

run: $(TARGET)
	./$(TARGET) --secret supersecret --data ../data --panel ../examples/Tu-104BN/avionics.lua

        
