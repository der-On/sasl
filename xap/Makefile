PLUGINS="../plugin/xap"
SDK=/opt/x-plane/SDK

TARGET=lin.xpl
HEADERS=$(wildcard *.h)
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
LUAJIT=/opt/luajit/

DEFS= -DLIN=1 -fPIC -m32 -fno-stack-protector -DXPLM200 -ggdb
CXXFLAGS+=-Wall -I$(SDK)/CHeaders/XPLM -I$(SDK)/CHeaders/Widgets -I$(LUAJIT)/include/luajit-2.0 -I../libavionics -I../libogl $(DEFS) -O2
LNFLAGS+=-shared -rdynamic -nodefaultlibs -m32 -ggdb -L/usr/lib32 -L../libavionics -L../libogl -L$(LUAJIT)/lib
LIBS+=-lm -lxavionics -lluajit-5.1 -logl -lSOIL32 -lopenal #$(PLUGINS)/XPLM.so $(PLUGINS)/XPWidgets.so

all: $(TARGET)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<
	
$(TARGET): $(OBJECTS) ../libavionics/libxavionics.a ../libogl/libogl.a
	$(CXX) -o $(TARGET) $(LNFLAGS) $(OBJECTS) $(LIBS)

clean:
	rm -f $(OBJECTS) $(TARGET)

install: $(TARGET)
	cp -f $(TARGET) $(PLUGINS)


