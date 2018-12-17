CC=g++
CPPFLAGS=-g -c -std=c++14 -Wall -Wextra -pedantic
LDFLAGS=-lpthread -lboost_system -lpigpio
EXECUTABLE=server

SOURCES=$(wildcard *.cpp)
OBJECTS=$(notdir $(SOURCES:.cpp=.o))

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cc.o:
	$(CC) $(CPPFLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)

run: $(EXECUTABLE)
	sudo ./$(EXECUTABLE)

client:
	$(MAKE) -C client

.PHONY: all clean client
