// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define WORD_LENGTH 5

int main() {
    WSADATA wsaData;
    SOCKET client_socket;
    struct sockaddr_in server_addr;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        return EXIT_FAILURE;
    }

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        perror("Error creating socket");
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Set up server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        perror("Connection failed");
        closesocket(client_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    printf("Connected to the server\n");

    // Receive the word from the server
    char word[WORD_LENGTH + 1];
    recv(client_socket, word, sizeof(word), 0);
    printf("Received word to guess: %s\n", word);

    while (1) {
        // Get user input for the guess
        char guess[WORD_LENGTH + 1];
        printf("Enter your guess: ");
        scanf("%s", guess);

        // Send the guess to the server
        send(client_socket, guess, sizeof(guess), 0);

        // Receive feedback from the server
        recv(client_socket, guess, sizeof(guess), 0);
        printf("Server feedback: %s\n", guess);

        // Check if the guess is correct
        if (strcmp(guess, "Correct! You guessed the word.") == 0) {
            printf("Congratulations! You guessed the word!\n");
            break;
        }

        // Check if the maximum number of attempts is reached
        if (strcmp(guess, "Sorry, you've reached the maximum number of attempts. The word was not guessed.") == 0) {
            printf("Sorry, you've reached the maximum number of attempts. The word was not guessed.\n");
            break;
        }
    }

    // Cleanup
    closesocket(client_socket);
    WSACleanup();

    return EXIT_SUCCESS;
}
