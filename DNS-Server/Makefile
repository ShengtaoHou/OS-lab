all: server client client2

server: server.c status.h
	cc -o server server.c -pthread
	
client: client.c status.h
	cc -o client client.c

client2: client2.c status.h
	cc -o client2 client2.c

clean:
	rm server client client2
