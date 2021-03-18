#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define HEADER_SIZE 16

int send_packet(int sock_fd, char *data, int length)
{
    char header[HEADER_SIZE];
    char format[20];

    sprintf(format, "%%0%dd", HEADER_SIZE - 1);
    sprintf(header, format, length);

    header[HEADER_SIZE - 1] = 0;

    send(sock_fd, header, HEADER_SIZE, 0);

    return send(sock_fd, data, length, 0);
}

int recv_packet(int sock_fd, char **data)
{
    char header[HEADER_SIZE];

    recv(sock_fd, header, HEADER_SIZE, 0);

    int length = -1;

    printf("Header: %s\n", header);

    sscanf(header, "%d", &length);

    printf("Length: %d\n", length);

    if (length <= 0)
    {
        printf("Invalid length\n");
        return -1;
    }

    char *buffer = calloc(length, sizeof(char));
    *data = buffer;

    return recv(sock_fd, buffer, length, 0);
}