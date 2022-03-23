// Server side C program to demonstrate HTTP Server programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080
int main(int argc, char const *argv[])
{
    int server_fd, new_socket;
    long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    //get the post.html file
    FILE *fp;
    fp = fopen("post.html", "r");
    char buffer_temp[3000];
    int i = 0;
    char ch;
    char bye[] = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 3000\n\n";
    while (i < 3000 - 1 && (ch = fgetc(fp)) != EOF)
    {
        buffer_temp[i++] = ch;
    }
    buffer_temp[i] = '\0';
    fclose(fp);
    strcat(bye, buffer_temp);

    // Only this line has been changed. Everything is same.
    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 50\n\nHello world. Welcome to index.html!";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    memset(address.sin_zero, '\0', sizeof address.sin_zero);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        // GET /index HTTP/1.1
        // GET / HTTP/1.1

        char buffer[30000] = {0};
        valread = read(new_socket, buffer, 30000);
        printf("%s\n", buffer);
        char *ret1 = strstr(buffer, "GET /index.html HTTP/1.1");
        char *ret2 = strstr(buffer, "GET / HTTP/1.1");
        char *ret3 = strstr(buffer, "GET /post.html HTTP/1.1");
        char *ret4 = strstr(buffer, "POST / HTTP/1.1");
        // if ret1 or ret2 is not null, then it means that the request is for index.html
        if (ret1)
            write(new_socket, hello, strlen(hello));
        else if (ret2)
            write(new_socket, hello, strlen(hello));
        else if (ret3)
            write(new_socket, bye, strlen(bye));
        else if (ret4)
        {
            // get the fname and lname from the buffer and store it in a file
            char *client = strstr(buffer, "fname=");
            // store it in form.txt
            FILE *f = fopen("form.txt", "a");
            // fprintf(f, "%s", client);
            fputs(client, f);
            fclose(f);
            // read the file and send it to the client
            f = fopen("form.txt", "r");
            char buffer_temp[3000];
            int i = 0;
            char ch;
            while (i < 3000 - 1 && (ch = fgetc(f)) != EOF)
            {
                buffer_temp[i++] = ch;
            }
            buffer_temp[i] = '\0';
            fclose(f);
            strcat(bye, buffer_temp);
            write(new_socket, buffer_temp, strlen(buffer_temp));
        }
        else
            write(new_socket, "HTTP/1.1 404 Not Found\nContent-Type: text/plain\nContent-Length: 50\n\n404 Not Found", strlen("HTTP/1.1 404 Not Found\nContent-Type: text/plain\nContent-Length: 50\n\n404 Not Found"));
        printf("------------------Hello message sent-------------------");

        close(new_socket);
    }
    return 0;
}