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

typedef struct Cred
{
    char username[50];
    char password[50];
} Cred;

typedef struct Client
{
    char *username;
    char *password;
    int sock_fd;

    Cred *creds;
    int no_of_users;

    struct Client *head;
    struct Client *next;
} Client;

Client *new_client(int sock_fd, Client *head, Cred *creds, int no_of_users)
{
    Client *client = (Client *)malloc(sizeof(Client));
    client->username = NULL;
    client->password = NULL;
    client->sock_fd = sock_fd;

    client->creds = creds;
    client->no_of_users = no_of_users;

    client->head = head;
    client->next = NULL;
    return client;
}

Client *initialize_list()
{
    Client *head = new_client(0, NULL, NULL, 0);
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

    printf("- New client connected!\n");

    char *username, *password;
    recv_packet(client->sock_fd, &username);
    recv_packet(client->sock_fd, &password);

    char *invalid_password = "Invalid Password!";
    char *user_not_found = "User not found!";
    char *authenticated = "AUTHENTICATED";

    int flag = 0;
    for (int i = 0; i < client->no_of_users; i++)
    {
        if (strcmp(client->creds[i].username, username) == 0)
        {
            if (strcmp(client->creds[i].password, password) != 0)
            {
                send_packet(client->sock_fd, invalid_password, strlen(invalid_password));

                close(client->sock_fd);
                remove_client(client->head, client);
                return NULL;
            }
            else
            {
                flag = 1;
                break;
            }
        }
    }

    if (flag == 0)
    {
        send_packet(client->sock_fd, user_not_found, strlen(user_not_found));
        close(client->sock_fd);
        remove_client(client->head, client);
        return NULL;
    }

    send_packet(client->sock_fd, authenticated, strlen(authenticated));

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

    int no_of_users = 0;
    while (fscanf(cred_file, "%[^,]%*c %[^\n]%*c", fusername, fpassword) != EOF)
        no_of_users++;

    Cred *creds = calloc(no_of_users, sizeof(Cred));

    fseek(cred_file, 0, SEEK_SET);
    int i = 0;
    while (fscanf(cred_file, "%[^,]%*c %[^\n]%*c", fusername, fpassword) != EOF)
    {
        strcpy(creds[i].username, fusername);
        strcpy(creds[i].password, fpassword);
        i++;
    }

    Client *head = initialize_list();

    while (1)
    {
        if ((conn_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            printf("Error accepting connection!");
            continue;
        }

        fflush(stdout);

        Client *client = new_client(conn_socket, head, creds, no_of_users);
        add_client(head, client);
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, client);
    }

    return 0;
}