#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/select.h>
#include "nickname.h"
#include "ui.h"

#define MAX_MESSAGE_LENGTH 256
#define MAX_HISTORY_MESSAGES 100

int hostServer(const char *port) {
    int sockfd, newSockfd;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLen;

    // create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // set up server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(atoi(port));

    // bind socket
    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for connection...\n");
    // listen for connections
    if (listen(sockfd, 5) < 0) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    // accept incoming connection
    clientLen = sizeof(clientAddr);
    newSockfd = accept(sockfd, (struct sockaddr *)&clientAddr, &clientLen);
    if (newSockfd < 0) {
        perror("Error accepting connection");
        exit(EXIT_FAILURE);
    }

    printf("Connection accepted from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

    // close the original listening socket
    close(sockfd);

    return newSockfd;
}

int connectToServer(const char *host, const char *port) {
    printf("Connected to %s:%s\n", host, port);

    int sockfd;
    struct sockaddr_in servAddr;

    // create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        exit(EXIT_FAILURE);
    }

    // initialize server address structure
    memset((char *) &servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(atoi(port));

    // convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, host, &servAddr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // connect to the server
    if (connect(sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    return sockfd; // return the socket file descriptor for communication
}

void sendMessage(int sockfd, const char *message) {
    char fullMessage[MAX_MESSAGE_LENGTH + 100]; // including space for nickname
    snprintf(fullMessage, sizeof(fullMessage), "%s: %s\n", getNickname(), message);

    if (send(sockfd, fullMessage, strlen(fullMessage), 0) < 0) {
        perror("Error sending message");
        exit(EXIT_FAILURE);
    }
}

int receiveMessage(int sockfd, char *buffer, int bufferSize) {
    int n = recv(sockfd, buffer, bufferSize - 1, 0);
    if (n < 0) {
        perror("Error receiving message");
        exit(EXIT_FAILURE);
    }
    buffer[n] = '\0'; // null-terminate the received data
    return n;
}

// function to set file descriptor to non-blocking mode
void setNonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void setInputMode() {
  struct termios tattr;
  char *name;

  // make sure stdin is a terminal.
  if (!isatty (STDIN_FILENO))
    {
      fprintf (stderr, "Not a terminal.\n");
      exit (EXIT_FAILURE);
    }

  // set the funny terminal modes.
  tcgetattr (STDIN_FILENO, &tattr);
  tattr.c_lflag &= ~(ICANON|ECHO); // clear ICANON and ECHO.
  tattr.c_cc[VMIN] = 1;
  tattr.c_cc[VTIME] = 0;
  tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
}

// function to handle the message exchange loop asynchronously
void messageExchangeLoop(int sockfd) {
    char message[MAX_MESSAGE_LENGTH];
    char receivedMessage[MAX_MESSAGE_LENGTH];
    char *messageHistory[MAX_HISTORY_MESSAGES] = {NULL}; // buffer for storing chat history
    int numMessages = 0;
    char inputBuffer[MAX_MESSAGE_LENGTH] = ""; // buffer for storing user input
    int inputLength = 0;
    fd_set readFds;
    int maxfd = sockfd > STDIN_FILENO ? sockfd : STDIN_FILENO;

    // set stdin to non-blocking mode
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    setInputMode();

    while (1) {
        FD_ZERO(&readFds);
        FD_SET(sockfd, &readFds);
        FD_SET(STDIN_FILENO, &readFds);

        // wait for activity on either the socket or stdin
        if (select(maxfd + 1, &readFds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        // if there's activity on the socket (incoming message)
        if (FD_ISSET(sockfd, &readFds)) {
            receiveMessage(sockfd, receivedMessage, MAX_MESSAGE_LENGTH);
            printf("%s\n", receivedMessage); // print received message
            
            // store received message in chat history buffer
            messageHistory[numMessages++] = strdup(receivedMessage);
            if (numMessages > MAX_HISTORY_MESSAGES) {
                free(messageHistory[0]); // free the oldest message
                memmove(messageHistory, messageHistory + 1, (numMessages - 1) * sizeof(char *));
                numMessages--;
            }
        }

        // if there's activity on stdin
        if (FD_ISSET(STDIN_FILENO, &readFds)) {
            // read characters from stdin
            char ch;
            while (read(STDIN_FILENO, &ch, 1) == 1) {
                // check for Enter key
                if (ch == '\n') {
                    // process the completed message
                    if (inputLength > 0) {
                        // copy inputBuffer to message and send
                        strncpy(message, inputBuffer, sizeof(inputBuffer));
                        sendMessage(sockfd, message);

                        // store sent message in chat history buffer
                        char fullMessage[MAX_MESSAGE_LENGTH + 100]; // including space for nickname
                        snprintf(fullMessage, sizeof(fullMessage), "%s: %s\n", getNickname(), message);
                        messageHistory[numMessages++] = strdup(fullMessage);
                        
                        if (numMessages > MAX_HISTORY_MESSAGES) {
                            //free(messageHistory[0]); // free the oldest message
                            memmove(messageHistory, messageHistory + 1, (numMessages - 1) * sizeof(char *));
                            numMessages--;
                        }

                        // clear input buffer
                        memset(inputBuffer, 0, sizeof(inputBuffer));
                        inputLength = 0;
                    }
                } else if (ch == 127 || ch == '\b') {
                    // check if there are characters to delete
                    if (inputLength > 0) {
                        // remove character from buffer
                        inputBuffer[--inputLength] = '\0';
                    }
                } else if (inputLength < MAX_MESSAGE_LENGTH - 1) {
                    // add character to input buffer
                    inputBuffer[inputLength++] = ch;
                }
            }
        }

        // display chat history
        clearScreen();
        displayChatHistory((const char **)messageHistory, numMessages);
        printf("%s: %s", getNickname(), inputBuffer);
        fflush(stdout);
    }
}