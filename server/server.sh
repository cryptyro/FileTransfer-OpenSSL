#!/bin/bash

# Update package list and install OpenSSL along with the development package
sudo apt update
sudo apt install openssl
sudo apt install libssl-dev

# Create directory to store the certificates
CERT_DIR="/etc/ssl/servercerts"
sudo mkdir -p ${CERT_DIR}
sudo chmod 700 ${CERT_DIR}

# Generate the private key
sudo openssl genpkey -algorithm RSA -out ${CERT_DIR}/server.key -aes256

# Generate the CSR (Certificate Signing Request)
sudo openssl req -new -key ${CERT_DIR}/server.key -out ${CERT_DIR}/server.csr

# Generate the self-signed certificate
sudo openssl x509 -req -days 365 -in ${CERT_DIR}/server.csr -signkey ${CERT_DIR}/server.key -out ${CERT_DIR}/server.crt

# Change permissions of the generated files
sudo chmod 600 ${CERT_DIR}/server.key
sudo chmod 644 ${CERT_DIR}/server.crt

# Output the paths of the generated files
echo "SSL certificate and key have been generated:"
echo "Certificate: ${CERT_DIR}/server.crt"
echo "Private Key: ${CERT_DIR}/server.key"
