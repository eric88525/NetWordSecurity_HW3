all: Host view insert 

CC = gcc
GG = g++
Host: host.cpp
	$(GG) -Wall host.cpp -o Host -L/usr/lib -lssl -lcrypto	
	
view: view.c
	$(CC) -o view.cgi view.c

insert: insert.c
	$(CC) -o insert.cgi insert.c

.PHONY: clean,run
clean:
	rm Host program.cgi

test:
	./Host
