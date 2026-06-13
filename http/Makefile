CXX = g++
CXXFLAGS = -std=c++20 -Iinclude -Wall -g -pthread -lmysqlclient

SRC = src/main.cpp \
      src/Reactor.cpp \
      src/Connection.cpp \
      src/Acceptor.cpp \
	  src/HttpRequest.cpp \
	  src/HttpResponse.cpp \
	  src/ThreadPool.cpp \
	  src/HttpServer.cpp \
	  src/MySQL.cpp

TARGET = main

all:
	$(CXX) $(SRC) $(CXXFLAGS) -o $(TARGET)

run:
	./$(TARGET)

clean:
	rm -f $(TARGET)