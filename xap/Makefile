#PLUGINS="/opt/x-plane/X-Plane/Resources/plugins/xap"
#PLUGINS="../../yk40/plane/plugins/xap"
PLUGINS="../plugin/xap"
SDK=/opt/x-plane/SDK

TARGET=lin.xpl
HEADERS=$(wildcard *.h)
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

DEFS= -DLIN=1 -fPIC -m32 -fno-stack-protector -DXPLM200 -ggdb
CXXFLAGS=-Wall -I$(SDK)/CHeaders/XPLM -I$(SDK)/CHeaders/Widgets -I/usr/include/lua5.1 -I../libavionics $(DEFS)
LNFLAGS=-shared -rdynamic -nodefaultlibs -m32 -ggdb -L/usr/lib32 -L../libavionics
LIBS=-llua5.1 -lm -lxavionics -lSOIL32 #$(PLUGINS)/XPLM.so $(PLUGINS)/XPWidgets.so

all: $(TARGET)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<
	
$(TARGET): $(OBJECTS) ../libavionics/libxavionics.a
	$(CXX) -o $(TARGET) $(LNFLAGS) $(OBJECTS) $(LIBS)

clean:
	rm -f $(OBJECTS) $(TARGET)

install: $(TARGET)
	cp -f $(TARGET) $(PLUGINS)


