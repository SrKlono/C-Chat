#include <stdio.h>
#include <string.h>
#include "nickname.h"

#define MAX_NICKNAME_LENGTH 64

static char currentNickname[MAX_NICKNAME_LENGTH];

void setNickname(char *nickname) {
    strncpy(currentNickname, nickname, sizeof(currentNickname));
    currentNickname[sizeof(currentNickname) - 1] = '\0'; // null termination
}

void changeNickname() {
    char newNickname[100];
    printf("Enter your new nickname: ");
    fgets(newNickname, sizeof(newNickname), stdin);
    setNickname(newNickname);
}

char *getNickname() {
    return currentNickname;
}