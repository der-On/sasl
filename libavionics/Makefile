TARGET=libxavionics.a
HEADERS=$(wildcard *.h)
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

LUAJIT=/opt/luajit/
CXXFLAGS+=-Wall -I$(LUAJIT)/include/luajit-2.0 -fPIC -m32 -ggdb -fno-stack-protector 
LNFLAGS+=-shared -rdynamic -nodefaultlibs -fPIC -m32 -ggdb -L/opt/luajit/lib
LIBS+=-lluajit-5.1 -lm -lIL -lILU -lILUT

all: $(TARGET)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<
	
$(TARGET): $(OBJECTS)
	$(AR) rs $(TARGET) $(OBJECTS)
#	$(CXX) -o $(TARGET) $(LNFLAGS) $(OBJECTS) $(LIBS)

clean:
	rm -f $(OBJECTS) $(TARGET)

