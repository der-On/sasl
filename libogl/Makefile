TARGET=libogl.a
HEADERS=$(wildcard *.h)
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

CXXFLAGS+=-Wall -fPIC -m32 -ggdb -fno-stack-protector -O2

all: $(TARGET)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<
	
$(TARGET): $(OBJECTS)
	$(AR) rs $(TARGET) $(OBJECTS)

clean:
	rm -f $(OBJECTS) $(TARGET)

