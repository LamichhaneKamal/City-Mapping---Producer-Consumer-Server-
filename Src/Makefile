client: client.c
	gcc $(CFLAGS) -g -w -o client client.c -lrt -lpthread
server: server.c
	gcc $(CFLAGS) -g -w -c -o server.c -lrt -lpthread
clean: 
	rm -f *.o *.gch client server serverp map_api

