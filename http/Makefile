.PHONY: clean run

CXX = g++

CXXFLAGS = -std=c++20 \
           -Iinclude \
           -Wall \
           -g \
		   -MMD \
		   -MP

LDFLAGS  = -pthread

LDLIBS = -lmysqlclient \
          -lssl \
          -lcrypto \
		  -lhiredis


TARGET = build/main


SRC = $(wildcard src/*.cpp)


OBJ = $(SRC:src/%.cpp=build/%.o)



$(TARGET): $(OBJ)

	$(CXX) $(OBJ) \
	$(LDFLAGS) \
	$(LDLIBS) \
	-o $(TARGET)



build/%.o: src/%.cpp

	$(CXX) $(CXXFLAGS) \
	-c $< \
	-o $@



clean:

	rm -rf build/*.o $(TARGET)



run:

	./$(TARGET)