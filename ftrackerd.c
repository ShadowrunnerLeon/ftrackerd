/**
 * ftrackerd - daemon for tracking files and transmitting information about their changes over the network
 * Author: Leonid Sorokin
 **/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include "createdaemon.h"
#include "logLib.h"

static int SLEEP_TIME = 5;
static char *PORT = "16680";

static sig_atomic_t flagSIGHUP;
static int sockfd = -1;

//sighandler SIGHUP and SIGTERM
static void sighandler(int sig) {
    if (sig == SIGHUP) 
        flagSIGHUP = 1;
    else {
        if (flog != NULL)
            logClose();
        if (sockfd != -1)
            close(sockfd);

        exit(0);
    }
}

static void responseHandler() {
    //#########pending request from client#############
    fd_set fds;
    int nfds = sockfd + 1;
    int serr;
    struct timeval tv;

    FD_ZERO(&fds);
    FD_SET(sockfd, &fds);

    tv.tv_sec = 0;
    tv.tv_usec = 5000;

    if ((serr = select(nfds, &fds, NULL, NULL, &tv)) == 0) {
        logMessage("timeout\n");
        return;
    }
    else if (serr == -1) {
        perror("select");
        exit(-1);
    }
    //#################################################

    int numbytes;
    char recv_buf[BUFSIZE];
    char send_buf[2*BUFSIZE];
    struct sockaddr addr_from;
    socklen_t addr_size = sizeof addr_from;

    memset(send_buf, 0, sizeof send_buf);
    memset(recv_buf, 0, sizeof recv_buf);

    if ((numbytes = recvfrom(sockfd, recv_buf, BUFSIZE-1, 0, &addr_from, &addr_size))==-1) {
        perror("recvfrom");
        exit(-1);
    }

    recv_buf[strlen(recv_buf)] = '\0';

    //###########server commands################
    //hello - display greeting
    //track - display file status(changed or not changed) of all files
    //track file1 - display file status of file1(changed or not changed)
    //help  - display this commands
    //##########################################

    if (!strcmp(recv_buf, "hello"))
        strcpy(send_buf, "Hello, client!\n");
    else if (!strcmp(recv_buf, "track")) {
        send_buf[0] = '\n';

        for (int index = 0; index < HashTableSize; ++index) {
            if (hashTable[index] == NULL) 
                continue;
            
            struct linked_list *tmp = hashTable[index];

            while (tmp != NULL) {

                int newSize = getfSize(tmp->str);

                if (tmp->fSize != newSize) {
                    tmp->fSize = newSize;
                    sprintf(&send_buf[strlen(send_buf)], "%s: changed\n", tmp->str);
                }
                else 
                    sprintf(&send_buf[strlen(send_buf)], "%s: not changed\n", tmp->str);

                tmp = tmp->next;
            }
        }

        logMessage(send_buf);
    }
    else if (strstr(recv_buf, "track") != NULL) {
        char *fname = strtok(recv_buf, " ");
        fname = strtok(NULL, " \n\t");

        int newSize = getfSize(fname);
        int index = Hash(fname);

        struct linked_list *tmp = hashTable[index];

        while (tmp != NULL && strcmp(tmp->str, fname))
            tmp = tmp->next;   

        if (tmp->fSize != newSize) {
            tmp->fSize = newSize;
            sprintf(send_buf, "%s: changed\n", tmp->str);
        }
        else 
            sprintf(send_buf, "%s: not changed\n", tmp->str);
        
        logMessage(send_buf);
    }   
    else if (!strcmp(recv_buf, "help")) {
        strcpy(send_buf, "\n\
        hello       - display greeting\n\
        track       - display file status(changed or not changed)\n\
        track file1 - display file status(changed or not changed) of file1\n\
        help        - display this commands\n");
        logMessage(send_buf);
    }
    else {
        strcpy(send_buf, "Invalid command\n");
        logMessage(send_buf);
    }

    logMessage("Sending message...\n");
    if (sendto(sockfd, send_buf, sizeof(send_buf), 0, &addr_from, addr_size)==-1) {
        perror("sendto");
        exit(-1);
    }
}

int main(int argc, char *argv[]) {

    if (argc == 2) {
        PORT = argv[1];
    }
    else if (argc == 3) {
        PORT = argv[1];
        SLEEP_TIME = atoi(argv[2]);
    }

    //###########configuring signal handlers################
    struct sigaction saHUP, saTERM;

    sigemptyset(&saHUP.sa_mask);
    sigemptyset(&saTERM.sa_mask);

    saHUP.sa_flags = SA_RESTART;
    saHUP.sa_handler = sighandler;
    if (sigaction(SIGHUP, &saHUP, NULL) == -1) {
        perror("sigaction");
        exit(-1);
    }

    saTERM.sa_flags = SA_RESTART;
    saTERM.sa_handler = sighandler;
    if (sigaction(SIGTERM, &saTERM, NULL) == -1) {
        perror("sigaction");
        exit(-1);
    }
    //######################################################

    if (createDaemon(0) == -1) {
        perror("daemon");
        exit(-1);
    }

    //###########server settings################
    struct addrinfo hints, *res, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    int rv;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &res))!=0) {
        printf("getaddrinfo: %s\n", gai_strerror(rv));
        exit(-1);
    }

    for (p = res; p!=NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))==-1) {
            perror("server: sockfd");
            continue;
        }

        int yes = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))==-1) {
            perror("setsockopt");
            exit(-1);
        }
        
        if (bind(sockfd, p->ai_addr, p->ai_addrlen)==-1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p==NULL) {
        printf("p: null pointer\n");
        exit(-1);
    }

    freeaddrinfo(res);
    //##########################################

    readConfig();
    logOpen();

    int unslept = SLEEP_TIME;
    int count = 0;

    while(1) {
        unslept = sleep(unslept);

        //#####Update config#####
        if (flagSIGHUP) {
            logClose();
            readConfig();
            logOpen();
            logMessage("restart\n");
            flagSIGHUP = 0;
        }
        //#######################

        if (!unslept) {
            responseHandler();
            unslept = SLEEP_TIME;
        }
    }
}