#include <pthread.h>
#include "serverutil.h"
#define PORT 9999
SSL_CTX *CTX; // Declaring SSL context as global variable, since we shall remove the privatekey once the server is up and running.

// Function to handle each client connection
void *handle_client(void *arg) {
    int client_fd = *((int *)arg);
    char buffer[BUFFER_SIZE];
    char filename[BUFFER_SIZE];
    char choice;
    long filesize;
    char *client_ip = FDtoIP(client_fd);
    printf("Connected to client with IP address: %s\n", client_ip);
    
    //Getting choice from client
	recv(client_fd, &choice, 1, 0);
	if(choice == 'y' || choice == 'Y')
   		SSL_client(client_fd, CTX);
	else {
    	while (1) {
        	// Receiving from client
        	memset(filename, 0, BUFFER_SIZE);
        	if (recv(client_fd, filename, sizeof(filename), 0) <= 0)
				break;
				
			printf("%s requested %s\n", client_ip, filename);

        	// Open the file to be sent (if exists)
        	char flag = 'y';
    		FILE *file = fopen(filename, "rb");
    		if (file == NULL)
    			flag = 'n';
    		if (send(client_fd, &flag, 1, 0) <=0)
    			break;
    		if (flag=='n')
    			continue;

        	// Calculate the filesize
        	filesize = sizeInKB(filename);
        	if (send(client_fd, &filesize, sizeof(filesize), 0) <= 0)
        		break;

    		// Read from the file and send data to the client
        	size_t bytesRead , bytesSent;
        	memset(buffer, 0, sizeof(buffer));
			while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
				if ((bytesSent=send(client_fd, buffer, bytesRead, 0)) <= 0)
					break;
    		}
    		if (bytesSent<=0)
    			break;
        	printf("File transfer with %s completed \n", client_ip);
        	fclose(file);
    	}
    	printf("IP %s has left \n", client_ip);
	}
   	close(client_fd);
   	pthread_exit(NULL);
}

int main() {
	CTX = Init_Config();
    const char *keypath = "/etc/ssl/servercerts/server.key";
    if (unlink(keypath) != 0) {
        perror("Failed to delete private key");
    }
    
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        error("Socket creation failed");

    // Configuring server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Binding socket to the address
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) 
        error("Binding failed");

    // Listening for incoming connections
    if (listen(server_fd, 5) == -1)
        error("Listening failed");
    printf("Server listening on port %d\n", PORT);

    while (1) {
        // Accepting incoming connection
        if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len)) == -1) {
            perror("Accepting connection failed");
            continue;
        }

        pthread_t tid;
    	// Creating a thread for each client
    	if (pthread_create(&tid, NULL, handle_client, &client_fd) != 0) {
    	    perror("Thread creation failed");
    	    close(client_fd);
    	}
    	pthread_detach(tid);
    }

    close(server_fd);
    return 0;
}
