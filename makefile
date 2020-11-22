Server: Server.c
	gcc Server.c -o Server -lmysqlclient -pthread

clean:
	rm Server