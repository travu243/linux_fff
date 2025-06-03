CC = gcc
CFLAGS = -Wall -O2
SSLFLAGS = -lcrypto -lpthread

SERVER = server

.PHONY: all clean

all: $(SERVER)

# Compile server_tls
$(SERVER): $(SERVER).c
	$(CC) $(CFLAGS) -o $@ $< $(SSLFLAGS)

clean:
	rm -f $(SERVER)
