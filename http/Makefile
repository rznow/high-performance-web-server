CXX = g++

CXXFLAGS = -std=c++20 \
           -Iinclude \
           -Wall \
           -g \
           -pthread


LDFLAGS = -lmysqlclient \
          -lssl \
          -lcrypto


TARGET = build/main


SRC = $(wildcard src/*.cpp)


OBJ = $(SRC:src/%.cpp=build/%.o)



$(TARGET): $(OBJ)

	$(CXX) $(OBJ) \
	$(CXXFLAGS) \
	$(LDFLAGS) \
	-o $(TARGET)



build/%.o: src/%.cpp

	mkdir -p build

	$(CXX) $(CXXFLAGS) \
	-c $< \
	-o $@



clean:

	rm -rf build/*.o $(TARGET)



run:

	./$(TARGET)