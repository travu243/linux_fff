run this commands to build docker image and run it to run client_tls_final

docker build -t tls-client . (build image)

docker run -it --network host tls-client (run docker image, -it for use terminal in container to chat)

to terminate container:

docker stop (container id)
