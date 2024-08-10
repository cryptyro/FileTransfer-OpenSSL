# A File Transfer Protocol Using OpenSSL

## Overview
This guide outlines the steps to generate a self-signed SSL certificate and key for a server, compile and execute the server code, and set up the client to communicate securely with the server. It also includes some basic security measures.

## Server Setup

### Generate Private Key and Certificate
Run the following script to generate a self-signed SSL certificate and private key for the server:

```bash
bash server.sh
```
During the generation of the private key, you will be prompted to provide a PEM passphrase. Set it to 123456 unless you want to provide the password each time a new connection is set up.

### Compile and Execute the Server Code
On successful gneration of server private key and certificate, compile and execute server.c by linking the associated libraries 
```bash
gcc -o server server.c -lssl -lcrypto -lpthread
sudo ./server
```
#### Security Modifications:

    Mod 1: To prevent accidental sharing of the server.key file with the client, delete the file after the SSL context is set up. This is a simple but effective way to ensure the key isn't shared accidentally.

    Mod 2: For additional security, run the server in a chroot environment. This restricts the server's visibility to the rest of the filesystem.
To set up the chroot environment:
```bash
bash chroot.sh
```
