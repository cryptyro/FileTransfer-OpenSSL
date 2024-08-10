#include <string.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#define BUFFER_SIZE 1024

// Initialize SSL
void initialize_ssl() {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

// For error handling
void Error(const char* msg){
	printf("%s\n",msg);
	ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
}

// Create SSL context
SSL_CTX *create_context() {
    SSL_CTX *ctx = SSL_CTX_new(SSLv23_client_method());
    if (!ctx)
    	Error("Context creation failed");
    SSL_CTX_set_options(ctx, SSL_OP_SINGLE_DH_USE); // Set SSL options

	// Configure SSL context
    if (SSL_CTX_use_certificate_file(ctx,"/etc/ssl/clientcerts/client.crt", SSL_FILETYPE_PEM) <= 0)
    	Error("Configuration failed");
    if (SSL_CTX_use_PrivateKey_file(ctx, "/etc/ssl/clientcerts/client.key", SSL_FILETYPE_PEM) <=0)
    	Error("Configuration failed");
    return ctx;
}

// Function for listening and sending messages over SSL
void SSL_client(int client_fd) {	
	// Initialize SSL
    initialize_ssl();
    SSL_CTX *ctx = create_context();

    // Create SSL structure
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client_fd);
    if (SSL_connect(ssl) <= 0)
    	Error("Handshake failed");
    printf("SSL handshake successful\n");
    printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
    
	char buffer[BUFFER_SIZE], line1[BUFFER_SIZE], line2[BUFFER_SIZE];

    // Continuous communication loop
    while(1) {
    	// Get filename from the user and send to the server
   	   	memset(line1, 0, sizeof(line1));
    	printf("What file are you looking for? (send 'bye' to leave): ");
   		fgets(line1, BUFFER_SIZE, stdin);
		line1[strlen(line1)-1]=0;// Remove the newline character
	
		// On input "bye" disconnect from the server
    	if(strcmp(line1,"bye")==0)
       		break;
       	
       	memset(line2, 0, sizeof(line2));
       	printf("Save file as: ");
    	fgets(line2, BUFFER_SIZE, stdin);
		line2[strlen(line2)-1]=0;// Remove the newline character	
       	
		if(SSL_write(ssl, line1, strlen(line1))<=0)
			Error("send failed");
    	
    	// If the requested file doesnot exist on server
    	char flag;
    	if(SSL_read(ssl, &flag, 1) <= 0 )
    		Error("recv failed");
    	if (flag  == 'n'){
    		printf("The requested file does not exist on server !\n");
    		continue;
    	}
    	
    	// Get size of the file in KB
    	long filesize;
    	if(SSL_read(ssl, &filesize, sizeof(filesize)) <= 0 )
    		Error("recv failed");
			
    	// Open file to write received data
    	FILE *file = fopen(line2, "wb");
    	if (file == NULL){
        	perror("Failed to open file");
        	continue;
        }
    	
    	// Receive data from server and write to file
    	int KBcount=0;
    	int bytesReceived;
    	memset(buffer, 0, sizeof(buffer));
    	while ((bytesReceived = SSL_read(ssl, buffer, BUFFER_SIZE)) > 0){
        	if (fwrite(buffer, 1,bytesReceived, file) <=0 )
        		perror("printing error");
        	KBcount++;
        	if (KBcount == filesize){
        	   	printf("File received successfully\n");
        		break;
			}
		}

   	    fclose(file);
       	if (bytesReceived <= 0)
       		Error("File transfer was not successful");

    }
    SSL_shutdown(ssl);// Shutdown SSL
	SSL_free(ssl);
}
