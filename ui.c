#include <stdio.h>
#include "ui.h"
#include "nickname.h"

#define MAX_CHAT_HISTORY 20 // maximum number of messages to display

void displayChatHistory(const char *messages[], int numMessages) {
    int startIndex = (numMessages > MAX_CHAT_HISTORY) ? numMessages - MAX_CHAT_HISTORY : 0;

    for (int i = startIndex; i < numMessages; i++) {
        printf("%s", messages[i]);
    }
}

void displayMenu() {
    clearScreen();
    printf("Connected as: %s\n", getNickname());

    printf("\n=== Main Menu ===\n");
    printf("1. Connect to Server\n");
    printf("2. Host a Server\n");
    printf("3. Change Nickname\n");
    printf("4. Quit\n");
}

int getUserChoice() {
    int choice;
    printf("Enter your choice: ");
    scanf("%d", &choice);
    clearInputBuffer();
    return choice;
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void clearScreen() {
#ifdef _WIN32
    system("cls"); // for Windows
#else
    printf("\033[2J\033[H"); // for Unix/Linux/MacOS
#endif
}