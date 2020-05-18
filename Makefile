all: Host view  getfile

CC = gcc
GG = g++
Host: host.cpp
	$(GG) -Wall host.cpp -o Host -L/usr/lib -lssl -lcrypto	
	
view: view.c
	$(CC) -o view.cgi view.c

getfile: getfile.c
	$(CC) -o getfile.cgi getfile.c

.PHONY: clean,run
clean:
	rm Host program.cgi

test:
	./Host
