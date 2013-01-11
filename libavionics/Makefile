include ../common.mk
    
TARGET=libavionics.a
HEADERS=$(wildcard *.h)
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

DEFS=-DLIN=1 -DXPM200
CXXFLAGS+=$(LUAJIT_CXXFLAGS) -I$(XPSDK)/CHeaders/XPLM $(DEFS)

all: $(TARGET)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<
	
$(TARGET): $(OBJECTS)
	$(AR) rs $(TARGET) $(OBJECTS)

clean:
	rm -f $(OBJECTS) $(TARGET)

