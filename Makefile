PROGS = client server

all: $(PROGS)

client: client.c
	gcc -g -o client client.c
  
server: server.c
	gcc -g -o server server.c
  
clean: 
	rm -rf $(PROG)
