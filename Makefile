PROGS = project05 server
all: $(PROGS)

project05: project05.c
	gcc -g -o project05 project05.c
  
server: server.c
	gcc -g -o server server.c
  
clean: 
	rm -rf $(PROG)
