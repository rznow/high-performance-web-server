CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Wall -g -pthread

SRC = src/main.cpp \
      src/Reactor.cpp \
      src/Connection.cpp \
      src/Acceptor.cpp \
	  src/HttpRequest.cpp \
	  src/HttpResponse.cpp

TARGET = main

all:
	$(CXX) $(SRC) $(CXXFLAGS) -o $(TARGET)

run:
	./$(TARGET)

clean:
	rm -f $(TARGET)