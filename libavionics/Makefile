TARGET=libxavionics.a
HEADERS=$(wildcard *.h)
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

CXXFLAGS+=-Wall -I/usr/include/lua5.1 -fPIC -m32 -ggdb -fno-stack-protector
LNFLAGS+=-shared -rdynamic -nodefaultlibs -fPIC -m32 -ggdb
LIBS+=-llua5.1 -lm -lIL -lILU -lILUT

all: $(TARGET)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<
	
$(TARGET): $(OBJECTS)
	$(AR) rs $(TARGET) $(OBJECTS)
#	$(CXX) -o $(TARGET) $(LNFLAGS) $(OBJECTS) $(LIBS)

clean:
	rm -f $(OBJECTS) $(TARGET)

