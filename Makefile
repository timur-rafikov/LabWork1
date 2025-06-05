TARGET = app
TESTTARGET = test-app

CXX = g++
CXXFLAGS = -Wall -g -fopenmp -Werror -Wpedantic
TESTFLAGS = -lgtest -lgmock -pthread

DEPS = bmp_reader.hpp
SRC = bmp_reader.cpp main.cpp
OBJ = bmp_reader.o main.o

TESTSRC = test.cpp bmp_reader.cpp


%.o: %.cpp $(DEPS)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(CXXFLAGS)

all: app

test: $(TESTSRC)
	$(CXX) $(TESTSRC) $(CXXFLAGS) -o $(TESTTARGET) $(TESTFLAGS)

clean:
	rm -f $(TARGET) *.o $(TESTTARGET)

.PHONY: all clean test