CC=g++
CPPFLAGS=-std=c++11 -Wall -Wextra -pedantic
LDFLAGS= -lpthread -lrt
EXECUTABLE=slave
SOURCES=slave.cpp

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CPPFLAGS) -o $(EXECUTABLE) $(LDFLAGS) $(SOURCES) 

run: $(EXECUTABLE)
	sudo ./$<

clean:
	rm -rf slave
