#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "packet.c"

#define PORT 5000

int main()
{
    int sock_fd = 0, recv_len;
    struct sockaddr_in server_address;
    char message[256];
    char buffer[1024] = {0};
    char username[50], password[50];

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error creating socket!!\n");
        return -1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0)
    {
        printf("Invalid address\\Address not supported\n");
        return -1;
    }

    printf("Connecting...\n");

    if (connect(sock_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        printf("Connection failed!\n");
        return -1;
    }

    printf("Please enter username and password to continue...\n");
    printf("Username -\t");
    scanf("%s%*c", username);

    printf("Password -\t");
    scanf("%s%*c", password);

    int length;
    char *data;
    length = recv_packet(sock_fd, &data);

    printf("Received data - %s\n", data);

    return 0;
}
