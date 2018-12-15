CC=g++
CPPFLAGS=-c -std=c++11 -Wall -Wextra -pedantic
LDFLAGS=-lpthread -lboost_system
EXECUTABLE=main

SOURCES=$(wildcard *.cpp)
OBJECTS=$(notdir $(SOURCES:.cpp=.o))

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cc.o:
	$(CC) $(CPPFLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

.PHONY: all clean
