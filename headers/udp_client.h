#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 255
#define PORT    "16680"

char send_message[BUFSIZE];
typedef struct
{
    int sockfd;
    struct addrinfo *ptr;
} sockfd_and_addrindo_ptr;

void *get_addr(struct sockaddr *sa) 
{
    return (sa->sa_family==AF_INET) ? 
    &(((struct sockaddr_in*)sa)->sin_addr) : 
    &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void SetParametrs(int argc, char* argv[])
{
    if (argc < 2 && argc > 3) 
    {
        printf("Too many arguments\n");
        exit(EXIT_FAILURE);
    }
    else if (argc == 2)
    {
        strcat(send_message, argv[1]);
    }
    else if (argc == 3) 
    {
        strcat(send_message, argv[1]);
        strcat(send_message, " ");
        strcat(send_message, argv[2]);
    }
}

sockfd_and_addrindo_ptr Initialize()
{
    int sockfd;
    struct addrinfo hints, *res, *ptr;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int rv;
    if (rv = getaddrinfo(INADDR_ANY, PORT, &hints, &res))
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(EXIT_FAILURE);
    }

    for (ptr = res; ptr!= NULL; ptr = ptr->ai_next) 
    {
        if ((sockfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol)) == -1) 
        {
            perror("client: sockfd");
            continue;
        }

        break;
    }

    if (!ptr) 
    {
        fprintf(stderr, "ptr: null pointer\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(res);

    sockfd_and_addrindo_ptr sock_and_ptr = {.sockfd = sockfd, .ptr = ptr};
    return sock_and_ptr;
}

void Send(int sockfd, struct addrinfo *addr)
{
    printf("Client send message: %s\n", send_message); 
    
    if (sendto(sockfd, send_message, strlen(send_message), 0, addr->ai_addr, addr->ai_addrlen) == -1) 
    {
        perror("sendto");
        exit(EXIT_FAILURE);
    }

    char ip_dst[INET_ADDRSTRLEN];
    if (!inet_ntop(AF_INET, get_addr((struct sockaddr*)&addr), ip_dst, INET_ADDRSTRLEN)) 
    {
        perror("inet_ntop");
        exit(EXIT_FAILURE);
    }

    printf("Sending to %s terminated\n", ip_dst);
}

void Recv(int sockfd)
{
    struct sockaddr addr_from;
    socklen_t addr_size = sizeof addr_from;
    char buf[BUFSIZE];
    int numbytes;

    if ((numbytes = recvfrom(sockfd, buf, BUFSIZE - 1, 0, &addr_from, &addr_size)) == -1) 
    {
        perror("recvfrom");
        exit(EXIT_FAILURE);
    }

    printf("recv: %d\n", numbytes);

    char ip_src[INET_ADDRSTRLEN];
    if (!inet_ntop(AF_INET, get_addr(&addr_from), ip_src, INET_ADDRSTRLEN)) 
    {
        perror("inet_ntop");
        exit(EXIT_FAILURE);
    }

    buf[numbytes] = '\0';
    printf("Client received from %s: %s\n", ip_src, buf);
}

void Shutdown(int sockfd)
{
    close(sockfd);
}