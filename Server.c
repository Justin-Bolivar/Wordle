// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h> // Include the time header

#define PORT 12345
#define MAX_CLIENTS 5
#define WORD_LENGTH 5
#define MAX_ATTEMPTS 10

void handle_client(SOCKET client_socket);

int main() {
    WSADATA wsaData;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        return EXIT_FAILURE;
    }

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        perror("Error creating socket");
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Set up server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        perror("Bind failed");
        closesocket(server_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) == SOCKET_ERROR) {
        perror("Listen failed");
        closesocket(server_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept incoming connections and handle them in separate threads
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket == INVALID_SOCKET) {
            perror("Accept failed");
            closesocket(server_socket);
            WSACleanup();
            return EXIT_FAILURE;
        }

        printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));

        // Handle each client in a separate thread (you need to implement this function)
        handle_client(client_socket);
    }

    // Cleanup
    closesocket(server_socket);
    WSACleanup();

    return EXIT_SUCCESS;
}

void handle_client(SOCKET client_socket) {
    char word[WORD_LENGTH + 1];
    char guess[WORD_LENGTH + 1];
    int attempts = 0;
    int i;

    // Generate a random word for the client to guess
    srand((unsigned int)time(NULL));
    for (i = 0; i < WORD_LENGTH; i++) {
        word[i] = 'a' + rand() % 26;
    }
    word[WORD_LENGTH] = '\0';

    send(client_socket, word, sizeof(word), 0);

    printf("Word to guess: %s\n", word);

    while (1) {
        // Receive the client's guess
        recv(client_socket, guess, sizeof(guess), 0);
        printf("Client guess: %s\n", guess);

        // Check if the guess is correct
        if (strcmp(guess, word) == 0) {
            send(client_socket, "Correct! You guessed the word.", sizeof("Correct! You guessed the word."), 0);
            break;
        }

        // Provide feedback on the guess
        char feedback[WORD_LENGTH + 1];
        for (i = 0; i < WORD_LENGTH; i++) {
            if (guess[i] == word[i]) {
                feedback[i] = '+';
            } else {
                feedback[i] = '-';
            }
        }
        feedback[WORD_LENGTH] = '\0';

        send(client_socket, feedback, sizeof(feedback), 0);

        // Increment the attempts
        attempts++;

        // Check if the maximum number of attempts is reached
        if (attempts == MAX_ATTEMPTS) {
            send(client_socket, "Sorry, you've reached the maximum number of attempts. The word was not guessed.", sizeof("Sorry, you've reached the maximum number of attempts. The word was not guessed."), 0);
            break;
        }
    }

    // Close the client socket
    closesocket(client_socket);
}
