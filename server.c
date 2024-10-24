#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

#include "customer.h"
#include "entities.h"
#include "employee.h"
#include "admin.h"

#define PORT 8081
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

void *handleClient(void *arg);
void choose(int clientSocket, int choice);

int main()
{
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientAddrLen = sizeof(clientAddress);

    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        perror("Socket creation failed");
        exit(1);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        perror("Socket binding failed");
        exit(1);
    }

    // Listen for incoming client connections
    if (listen(serverSocket, MAX_CLIENTS) == -1)
    {
        perror("Listening failed");
        exit(1);
    }

    printf("Server is listening on port %d...\n", PORT);

    // Accept and handle client connections
    while (1)
    {
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddrLen);
        if (clientSocket == -1)
        {
            perror("Accepting connection failed");
            continue;
        }

        pthread_t clientThread;
        pthread_create(&clientThread, NULL, handleClient, (void *)&clientSocket);
    }

    // Close the server socket (never reached in this example)
    close(serverSocket);

    return 0;
}

void *handleClient(void *arg)
{
    int clientSocket = *((int *)arg);
    char buffer[BUFFER_SIZE];
    char roleMenu[] = "Select your role:\n1. Admin\n2. Employee\n3. Customer\n\nEnter your choice:";
    send(clientSocket, roleMenu, strlen(roleMenu), 0);
    // Receive and process data from the client
    while (1)
    {
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0)
        {
            close(clientSocket);
            pthread_exit(NULL);
        }
        buffer[bytesRead] = '\0';
        printf("received msg from client is: %s", buffer);
        // Process the received data based on client role
        int choice = atoi(buffer);
        switch (choice)
        {
        case 1:
            adminOperationHandler(clientSocket);
            break;
        case 2:
            employeeOperationHandler(clientSocket);
            break;
        case 3:
            customerOperationHandler(clientSocket);
            break;
        default:
            send(clientSocket, "wrong Input....Choose again\n\nEnter your choice:", strlen("wrong Input....Choose again\n\nEnter your choice:"), 0);
            continue;
            break;
        }

        // Process the received data (e.g., echo it back)
        send(clientSocket, buffer, bytesRead, 0);
    }
    pthread_exit(NULL);
}

void choose(int clientSocket, int choice){
        switch (choice)
        {
        case 1:
            adminOperationHandler(clientSocket);
            break;
        case 2:
            employeeOperationHandler(clientSocket);
            break;
        case 3:
            customerOperationHandler(clientSocket);
            break;
        default:
            send(clientSocket, "wrong Input....Choose again\n", strlen("wrong Input....Choose again\n"), 0);
            choose(clientSocket, choice);
            break;
        }

}