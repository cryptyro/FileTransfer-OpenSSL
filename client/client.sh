#!/bin/bash

# Update package list and install OpenSSL along with the development package
sudo apt update
sudo apt install openssl
sudo apt install libssl-dev

# Create directory to store the certificates
CERT_DIR="/etc/ssl/clientcerts"
sudo mkdir -p ${CERT_DIR}
sudo chmod 777 ${CERT_DIR}

# Generate the private key
openssl genpkey -algorithm RSA -out ${CERT_DIR}/client.key -aes256

# Generate the CSR (Certificate Signing Request)
openssl req -new -key ${CERT_DIR}/client.key -out ${CERT_DIR}/client.csr

# Generate the self-signed certificate
openssl x509 -req -days 365 -in ${CERT_DIR}/client.csr -signkey ${CERT_DIR}/client.key -out ${CERT_DIR}/client.crt

# Output the paths of the generated files
echo "SSL certificate and key have been generated:"
echo "Certificate: ${CERT_DIR}/client.crt"
echo "Private Key: ${CERT_DIR}/client.key"
