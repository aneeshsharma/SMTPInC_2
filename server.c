/*
 * An SMTP server application
 * Author   -   Anish Sharma
 * Date     -   18-03-2021
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>

#include "packet.c"

#define PORT 5000

typedef struct Client
{
    char *username;
    char *password;
    int sock_fd;

    struct Client *head;
    struct Client *next;
} Client;

Client *new_client(int sock_fd)
{
    Client *client = (Client *)malloc(sizeof(Client));
    client->username = NULL;
    client->password = NULL;
    client->sock_fd = sock_fd;
    client->next = NULL;
    return client;
}

Client *initialize_list()
{
    Client *head = new_client(0);
    return head;
}

void add_client(Client *head, Client *client)
{
    Client *curr = head;
    while (curr->next)
        curr = curr->next;
    curr->next = client;
}

int remove_client(Client *head, Client *client)
{
    Client *curr = head;

    while (curr)
    {
        if (curr->next == client)
        {
            curr->next = client->next;
            free(client);
            return 1;
        }
        curr = curr->next;
    }
    return 0;
}

void *handle_client(void *arg)
{
    Client *client = (Client *)arg;

    printf("New client connected!\n");
    char *message = "Hello world!\n";
    send_packet(client->sock_fd, message, strlen(message));
    close(client->sock_fd);

    remove_client(client->head, client);
}

int main()
{
    int server_fd, conn_socket, recv_len;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char message[256];
    char buffer[1024] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\nError creating socket\n");
        return -1;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        printf("Error setting socket options!\n");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        printf("Error binding to port!\n");
        return -1;
    }

    if (listen(server_fd, 3) < 0)
    {
        printf("Error while listening for connections!\n");
        return -1;
    }

    char username[50], password[50];

    FILE *cred_file = fopen("logincred.txt", "r");

    char fusername[50], fpassword[50];

    while (fscanf(cred_file, "%[^,]%*c %[^\n]%*c", fusername, fpassword) != EOF)
    {
        if (strcmp(fusername, username) == 0)
        {
            if (strcmp(fpassword, password) == 0)
            {
                printf("Authenticated!\n");
                break;
            }
        }
    }

    Client *head = initialize_list();

    while (1)
    {
        if ((conn_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            printf("Error accepting connection!");
            continue;
        }

        Client *client = new_client(conn_socket);
        add_client(head, client);
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, client);
    }

    return 0;
}