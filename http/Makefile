.PHONY: clean run

build:
	mkdir -p build

workbench:
	mkdir -p workbench
	
CXX = g++

CXXFLAGS = -std=c++20 \
           -Iinclude \
           -Wall \
           -g \
		   -O0 \
		   -MMD \
		   -MP

WRKFLAGS = -std=c++20 \
           -Iinclude \
           -Wall \
           -O2 \
           -DNDEBUG \
           -MMD \
           -MP

LDFLAGS  = -pthread

LDLIBS = -lmysqlclient \
          -lssl \
          -lcrypto \
		  -lhiredis


TARGET = build/main
TARGET2 = workbench/main


SRC = $(wildcard src/*.cpp)


OBJ = $(SRC:src/%.cpp=build/%.o)
OBJ2 = $(SRC:src/%.cpp=workbench/%.o)


$(TARGET): $(OBJ)

	$(CXX) $(OBJ) \
	$(LDFLAGS) \
	$(LDLIBS) \
	-o $@

$(TARGET2): $(OBJ2)
	$(CXX) $(OBJ2) \
	$(LDFLAGS) \
	$(LDLIBS) \
	-o $@

release: $(TARGET2)

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

workbench/%.o: src/%.cpp | workbench
	$(CXX) $(WRKFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

test: $(TARGET2)
	./$(TARGET2)

clean:
	rm -f build/*.o build/*.d $(TARGET)
	rm -f workbench/*.o workbench/*.d $(TARGET2)

-include $(OBJ:.o=.d)
-include $(OBJ2:.o=.d)