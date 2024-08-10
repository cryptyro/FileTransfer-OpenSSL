#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "clientutil.h"

#define SERVER_IP "127.0.0.1"
#define port 9999

//For error handling
void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    int client_fd;
    struct sockaddr_in server_addr;

    // Create a socket
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error("Unable to create socket");
    
    // Configuring server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to the server
    if(connect(client_fd,(struct sockaddr*)&server_addr,sizeof(server_addr))<0)
        error("Connection failed");
    
    // Get user preference
    printf("====== Welcome ======\n");
    printf("Do you want to employ SSL(Y/n)? ");
	char c = getchar();
    while(getchar() != '\n');// Consume the remaining input to clear the buffer
    if(send(client_fd, &c, 1, 0) <= 0)
    	error("send failed");
    
   	if(c == 'y' || c == 'Y')
		SSL_client(client_fd);//Employ SSL
	else {
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
        	
			if(send(client_fd, line1, strlen(line1),0)<=0)
				error("send failed");
    		
    		// If the requested file doesnot exist on server
    		char flag;
    		if(recv(client_fd, &flag, 1, 0) <= 0 )
    			error("recv failed");
    		if (flag  == 'n'){
    			printf("The requested file does not exist on server !\n");
    			continue;
    		}
    		 		
    		// Get size of the file in KB
    		long filesize;
    		if(recv(client_fd, &filesize, sizeof(filesize), 0) <= 0 )
    			error("recv failed");
    		
    		// Open file to write received data
    		FILE *file = fopen(line2, "wb");
    		if (file == NULL){
        		perror("Failed to open file");
        		continue;
        	}
    		
    		// Receive data from server and write to file
    		long KBcount=0;
    		int bytesReceived;
    		memset(buffer, 0, sizeof(buffer));
    		while ((bytesReceived = recv(client_fd, buffer, BUFFER_SIZE, 0)) > 0){
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
        		error("File transfer was not successful");
    	}
	}
    close(client_fd);
    printf("Disconnected from server\n");
    return 0;
}
