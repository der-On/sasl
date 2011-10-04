include ../common.mk
    
TARGET=libavionics.a
HEADERS=$(wildcard *.h)
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

CXXFLAGS+=$(LUAJIT_CXXFLAGS)

all: $(TARGET)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<
	
$(TARGET): $(OBJECTS)
	$(AR) rs $(TARGET) $(OBJECTS)

clean:
	rm -f $(OBJECTS) $(TARGET)

