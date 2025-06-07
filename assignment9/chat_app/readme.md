# Encrypted TLS Chat Server & Client in C

This project implements a **multi-client chat system** in C using **OpenSSL/TLS encryption**. The server authenticates clients via login/signup, stores chat logs, and broadcasts messages securely over a TLS connection.

## Features

### Server
- TLS-encrypted communication using OpenSSL.
- User authentication via login or signup (username/password stored in `users.txt`).
- Handles multiple clients concurrently using `pthread`.
- Broadcasts messages to all connected clients.
- Saves chat history to `chatlog.txt`.
- Sends chat history to new clients on successful login.
- Announces when a user joins the chat.

### Client
- Connects to the server securely using TLS.
- Handles server prompts (login/signup, username, password, message input/output).
- Sends and receives encrypted messages.
- Displays server responses and other clients' messages.

## File Structure
├── server_tls_final.c # TLS-encrypted chat server  
├── client_tls_final.c # TLS chat client  
├── cert.pem # Server certificate (self-signed)  
├── key.pem # Server private key  
├── users.txt # User credentials  
├── chatlog.txt # Chat message log  
├── README.md # This documentation  


## Code flow diagram
```
[Start]
   |
   v
[Initialize OpenSSL context]
   |
   v
[Load cert.pem and key.pem]
   |
   v
[Create TCP socket and bind to port]
   |
   v
[Listen for connections]
   |
   v
{ Loop: accept client }
   |
   v
[Create SSL object, attach client socket]
   |
   v
[Perform TLS handshake]
   |
   v
[Send login/signin prompts to client]
   |
   v
[Receive user credentials via SSL_read]
   |
   v
[Check authentication (username/password)]
   |
   v
(If fail) --> [Send fail msg and disconnect]
   |
  (Else)
   |
   v
[Send chat history to client]
   |
   v
[Broadcast "<user> joined the chat"]
   |
   v
{ Loop: Handle incoming message }  <---------
   |                                        |
   v                                        |
[Receive message from client]               |
   |                                        |
   v                                        |
[Append message to chat log file]           |
   |                                        |
   v                                        |
[Broadcast message to all clients]          |
   |                                        |
   |                                        |
   |----->  { Repeat until disconnect }-----|
   |
   v
[Broadcast "<user> left the chat"]
   |
   v
[Free SSL, close socket]
   |
   v
[End client thread]
```

## Prerequisites

- GCC Compiler
- OpenSSL Development Libraries

### On Ubuntu/Debian:
```bash
sudo apt update
sudo apt install build-essential libssl-dev
```

## Build
### client-server
You only need run:
```
make
```
to genarate final output to run, or you can run step by step below.

### server (no encrypt)

- I also make server.c with same features but without TLS, and don't need a script to become client.
You just need to:
```
nc [ip address] [port]
```
to connect to ip address of device is running server.c

- To build, run:
```
make no_tls_server.mk
```
then run this to host server chat app:
```
./no_tls_server
```

## Generate TLS Certificate
```bash
openssl req -x509 -newkey rsa:2048 -keyout key.pem -out cert.pem -days 365 -nodes
```

## Build Instructions
```bash
gcc server_tls_final.c -o server_tls_final -lpthread -lssl -lcrypto
gcc client_tls_final.c -o client_tls_final -lssl -lcrypto
```

## Run Instructions
```bash
./server_tls_final
./client_tls_final
```
When prompted, enter the server's IP address (e.g., 127.0.0.1 for localhost).

## Chat Flow
### Client Interaction:
- Input server IP.
- Choose login (1) or register (2).
- Enter username & password (server will prompt).
- Receive chat log on successful login.
- Start chatting!

### Server Behavior:
- Accepts multiple clients via threads.
- Validates or registers users.
- Broadcasts messages to all connected clients.
- Sends chat log and join notification on new connection.
- Appends every message to chatlog.txt.

## Example Session
### On Server:
```
[SERVER]: alice joined the chat
[SERVER]: bob joined the chat
alice: Hello everyone!
```

### On Client:
```
Enter IP address of server: 127.0.0.1
1. to login
2. to register
1
Username: alice
Password: secret123
Login successful. Welcome
bob joined the chat
[bob]: Hello!
```

## Security Notes
- TLS encryption ensures confidentiality and integrity.
- Passwords stored as plaintext in users.txt (for demonstration only).
- Use hashing (e.g., SHA256 with salt) for real deployments.
- Self-signed certificates used — for testing only. Use CA-signed certificates for production.

## Limitations
- Terminal-only UI.
- No password hashing.
- Not suitable for production without security improvements.
