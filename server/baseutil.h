#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>

// Function to get filesize in KB
long sizeInKB( char *filename ) {
    struct stat st;
    long filesize;
    if (stat(filename, &st) == 0) {
        long filesize_in_bytes = st.st_size;
        filesize = (filesize_in_bytes + 1023)/1024;//Round up to the next KB
    } else {
        perror("stat");
    }
    return filesize;
}

// Functio to get IP from socket FD
char *FDtoIP (int client_fd) {
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    getpeername(client_fd, (struct sockaddr *)&addr, &addr_size);
    char *client_ip = (char*)malloc(INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    return client_ip;
}

//For error handling
void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}
