CXX = g++
CXXFLAGS = -std=c++20 -Iinclude -Wall -g -pthread -lmysqlclient -lssl -lcrypto

SRC := $(wildcard src/*.cpp) \
       $(wildcard src/network/*.cpp) \
       $(wildcard src/http/*.cpp) \
       $(wildcard src/mysql/*.cpp) \
       $(wildcard src/auth/*.cpp) \
       $(wildcard src/common/*.cpp)

TARGET = build/main

all:
	$(CXX) $(SRC) $(CXXFLAGS) -o $(TARGET)

run:
	./$(TARGET)

clean:
	rm -f $(TARGET)