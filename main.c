#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "ui.h"
#include "nickname.h"
#include "network.h"

#define MAX_MESSAGE_LENGTH 256

void connectToServerMenu() {
    clearScreen();

    // read server address
    char host[100], port[10];
    printf("Enter server address: ");
    fgets(host, sizeof(host), stdin);
    host[strcspn(host, "\n")] = '\0'; 

    // read port
    printf("Enter port: ");
    fgets(port, sizeof(port), stdin);
    port[strcspn(port, "\n")] = '\0'; 

    // connect to the server
    int sockfd = connectToServer(host, port);
    printf("Connected to server!\n");

    // start message loop
    messageExchangeLoop(sockfd);

    close(sockfd);
}

void hostServerMenu() {
    clearScreen();

    // read port
    char port[10];
    printf("Enter port to host server: ");
    fgets(port, sizeof(port), stdin);
    int sockfd = hostServer(port);

    printf("Server hosted successfully!\n");

    // start message loop
    messageExchangeLoop(sockfd);

    close(sockfd);
}

int main() {
    clearScreen();

    // read nickname
    char nickname[100];
    printf("Welcome to C-Chat!\n");
    printf("Please enter your nickname: ");
    fgets(nickname, sizeof(nickname), stdin);

    // set nickname
    nickname[strcspn(nickname, "\n")] = '\0';
    setNickname(nickname);

    // main menu loop
    int choice;
    do {
        displayMenu();
        choice = getUserChoice();

        switch(choice) {
            case 1: // connect to a server
                connectToServerMenu();
                break;
            case 2: // host a server
                hostServerMenu();
                break;
            case 3: // change nickname
                changeNickname();
                break;
            case 4: // quit
                printf("Goodbye!\n");
                break;
            default: // invalid awnser
                printf("Invalid choice. Please try again.\n");
        }
    } while(choice != 4);

    return 0;
}
