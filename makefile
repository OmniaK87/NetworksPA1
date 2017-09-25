clientserver: 
	gcc udp_client.c -o client
	gcc udp_server.c -o server 

client:
	gcc udp_client.c -o client

server:
	gcc udp_server.c -o server

clean:
	rm -f server client
