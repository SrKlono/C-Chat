#ifndef UI_H
#define UI_H

void displayMenu();
int getUserChoice();
void clearInputBuffer();
void clearScreen();
void displayChatHistory(const char *messages[], int numMessages);

#endif