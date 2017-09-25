CC = gcc

clientserver: 
	$(CC) udp_client.c -o client -lm
	$(CC) udp_server.c -o server -lm

client:
	$(CC) udp_client.c -o client -lm

server:
	$(CC) udp_server.c -o server -lm

clean:
	rm -f server client
