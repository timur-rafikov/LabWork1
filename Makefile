TARGET = app

CXX = g++
CXXFLAGS = -Wall -g

DEPS = $(wildcard *.hpp)
SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, %.o, $(SRC))


%.o: %.cpp $(DEPS)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(CXXFLAGS)

all: app

.PHONY: clean

clean:
	rm $(TARGET) *.o 
