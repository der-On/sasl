include ../common.mk

PLUGINS="../plugin/xap"

TARGET=lin.xpl
HEADERS=$(wildcard *.h)
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

DEFS=-DLIN=1 -DXPLM200
CXXFLAGS+=-I$(XPSDK)/CHeaders/XPLM -I$(XPSDK)/CHeaders/Widgets $(LUAJIT_CXXFLAGS) -I../libavionics -I../libaccgl -I../alsasound $(DEFS)
LNFLAGS+=-shared -rdynamic -nodefaultlibs -L../libavionics -L../libaccgl -L../alsasound -L$(LUAJIT)/lib $(LUAJIT_LNFLAGS)
LIBS+=-lm -lavionics -laccgl -lalsasound -lSOIL32 -lopenal $(LUAJIT_LIBS)

all: $(TARGET)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<
	
$(TARGET): $(OBJECTS) ../libavionics/libavionics.a ../libaccgl/libaccgl.a ../alsasound/libalsasound.a
	$(CXX) -o $(TARGET) $(LNFLAGS) $(OBJECTS) $(LIBS)

clean:
	rm -f $(OBJECTS) $(TARGET)

install: $(TARGET)
	cp -f $(TARGET) $(PLUGINS)


