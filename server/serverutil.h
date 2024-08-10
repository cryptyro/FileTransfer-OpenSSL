#include <string.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "baseutil.h"

#define BUFFER_SIZE 1024

// Function to Initialize SSL and create SSL context
SSL_CTX* Init_Config (){
    // Initialize SSL
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();

    SSL_CTX *ctx = SSL_CTX_new(SSLv23_server_method()); // Context
    if (!ctx)
        ERR_print_errors_fp(stderr);
    SSL_CTX_set_options(ctx, SSL_OP_SINGLE_DH_USE); // Set SSL options
    
    //Configuration of the context
    SSL_CTX_use_certificate_file(ctx, "/etc/ssl/servercerts/server.crt", SSL_FILETYPE_PEM);
    // Provide the password placed during generating private key
   	SSL_CTX_set_default_passwd_cb_userdata(ctx, "123456");
    SSL_CTX_use_PrivateKey_file(ctx, "/etc/ssl/servercerts/server.key", SSL_FILETYPE_PEM);
    
    
    return ctx;
}

void SSL_client(int client_socket,SSL_CTX *ctx ) {
    // Create SSL structure
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client_socket);
    char *client_ip = FDtoIP(client_socket);
    
    // Perform SSL handshake
    if (SSL_accept(ssl) > 0) {
        char buffer[BUFFER_SIZE];
    	char filename[BUFFER_SIZE];

    	while (1) {
        	// Receiving from client
        	memset(filename, 0, BUFFER_SIZE);
        	if (SSL_read(ssl, filename, sizeof(filename)) <= 0)
				break;
        		
			printf("%s requested %s\n", client_ip, filename);
				
			// Open the file to be sent (if exists)
        	char flag = 'y';
    		FILE *file = fopen(filename, "rb");
    		if (file == NULL)
    			flag = 'n';
    		if (SSL_write(ssl, &flag, 1) <=0)
    			break;
    		if (flag=='n')
    			continue;
			
        	// Calculate the filesize
        	long filesize = sizeInKB(filename);
        	if (SSL_write(ssl, &filesize, sizeof(filesize)) <= 0)
				break;
	
    		// Read from the file and send data to the client
    		size_t bytesRead, bytesSent;
    		memset(buffer, 0, sizeof(buffer));
    		while ((bytesRead=fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        		if ((bytesSent=SSL_write(ssl, buffer, bytesRead)) <= 0)
        			break;
        	}
        	if (bytesSent<=0)
    			break;
        	printf("File transfer with %s completed \n", client_ip);
        	fclose(file);
    	}
    }
	printf("IP %s has left \n", client_ip);

    // Shutdown SSL
    SSL_shutdown(ssl);
    SSL_free(ssl);
}
