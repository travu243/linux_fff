CC = gcc
CFLAGS = -Wall -O2
SSLFLAGS = -lssl -lcrypto -lpthread

SERVER = server_tls_final
CLIENT = client_tls_final

CERT = cert.pem
KEY = key.pem

.PHONY: all clean cert

all: cert $(SERVER) $(CLIENT)

# compile server_tls_final
$(SERVER): $(SERVER).c
	$(CC) $(CFLAGS) -o $@ $< $(SSLFLAGS)

# compile client_tls_final
$(CLIENT): $(CLIENT).c
	$(CC) $(CFLAGS) -o $@ $< $(SSLFLAGS)

# generate SSL certificate and key
cert:
	@echo "generating self-signed SSL certificate and key..."
	openssl req -x509 -newkey rsa:4096 -keyout $(KEY) -out $(CERT) -days 365 -nodes -subj "/CN=localhost"
	@echo "certificate and key generated."

clean:
	rm -f $(SERVER) $(CLIENT) $(CERT) $(KEY)
