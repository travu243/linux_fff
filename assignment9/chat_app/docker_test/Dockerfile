FROM debian:bullseye

# install gcc, OpenSSL, pthread
RUN apt update && apt install -y \
    gcc \
    libssl-dev \
    libpthread-stubs0-dev \
    && rm -rf /var/lib/apt/lists/*

# Copy file to container
COPY client_tls_final.c /app/client_tls_final.c

# cd to app
WORKDIR /app

# compile client
RUN gcc -o client client_tls_final.c -lssl -lcrypto -lpthread

# first command when run container
CMD ["./client"]
