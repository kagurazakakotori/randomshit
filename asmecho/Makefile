CC = gcc
CFLAGS = -nostdlib -nolibc -nodefaultlibs

all: server client

server: 
	$(CC) $(CFLAGS) -o server server.s

client:
	$(CC) $(CFLAGS) -o client client.s

clean:
	rm -f *~ *.o server client
