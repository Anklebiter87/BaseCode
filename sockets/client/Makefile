CC=/usr/bin/gcc
CFLAGS=-I include/ -Wall
SOURCES=client_socket.c main.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=client


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	    $(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	    rm $(OBJECTS) $(EXECUTABLE)


