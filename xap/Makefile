include ../common.mk

PLUGINS="../../plugin/sasl"

TARGET=lin.xpl

HEADERS=$(wildcard *.h)
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

DEFS=-DLIN=1 -DXPLM200
CXXFLAGS+=-I$(XPSDK)/CHeaders/XPLM -I$(XPSDK)/CHeaders/Widgets $(LUAJIT_CXXFLAGS) -I../libavionics -I../libaccgl -I../alsound $(DEFS)
LNFLAGS+=-shared -rdynamic -nodefaultlibs -L../libavionics -L../libaccgl -L../alsound -L$(LUAJIT)/lib $(LUAJIT_LNFLAGS)
LIBS+=-lm -lavionics -laccgl -lalsound -lopenal $(LUAJIT_LIBS)

ifneq ($(BUILD_64),yes)
LIBS+=-lSOIL32
else
LIBS+=-lSOIL
endif

all: $(TARGET)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<
	
$(TARGET): $(OBJECTS) ../libavionics/libavionics.a ../libaccgl/libaccgl.a ../alsound/libalsound.a
	$(CXX) -o $(TARGET) $(LNFLAGS) $(OBJECTS) $(LIBS)

clean:
	rm -f $(OBJECTS) $(TARGET)

install: $(TARGET)
	cp -f $(TARGET) $(PLUGINS)


