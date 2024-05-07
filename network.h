#ifndef NETWORK_H
#define NETWORK_H

int hostServer(const char *port);
int connectToServer(const char *host, const char *port);
void sendMessage(int sockfd, const char *message);
void receiveMessage(int sockfd, char *buffer, int bufferSize);
void messageExchangeLoop(int sockfd);

#endif