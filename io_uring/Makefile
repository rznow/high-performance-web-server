.PHONY: clean run
	
CXX = g++

CXXFLAGS = -std=c++20 \
           -Wall \
           -g \
		   -O0 \
		   -MMD \
		   -MP

LDFLAGS  = -pthread

LDLIBS = -luring

TARGET = build/main

SRC = $(wildcard src/*.cpp)

OBJ = $(SRC:src/%.cpp=build/%.o)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) \
	$(LDFLAGS) \
	$(LDLIBS) \
	-o $@

build: $(TARGET)

build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

gdb: $(TARGET)
	gdb ./$<

clean:
	rm -f build/*.o build/*.d $(TARGET)
