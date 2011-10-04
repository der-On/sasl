include ../common.mk

TARGET=libalsasound.a
HEADERS=$(wildcard *.h)
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

CXXFLAGS+=-I../libavionics

all: $(TARGET)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<
	
$(TARGET): $(OBJECTS)
	$(AR) rs $(TARGET) $(OBJECTS)

clean:
	rm -f $(OBJECTS) $(TARGET)

