/**
 * implementations of functions for working with configs and logs
 **/
#ifndef LOGLIB_H
#define LOGLIB_H

#include "HashTable.h"

#define BUFSIZE 1024

const char *CONFIG_FILE = "/tmp/ftracker.config";
const char *LOG_FILE = "/tmp/ftracker.log";

FILE *flog;

int getfSize(char *str) {
    struct stat file;
    if (stat(str, &file) == -1) {
        perror("stat");
        exit(-1);
    }

    return file.st_size;
}

void readConfig() {
    if (hashTable != NULL) {
        memset(hashTable, 0, sizeof(hashTable));
        freeHash(HashTableSize);
    }

    char buf[BUFSIZE];
    FILE *fconfig = fopen(CONFIG_FILE, "r");

    fgets(buf, BUFSIZE, fconfig);
    if (ferror(fconfig)) {
        perror("fgets");
        exit(-1);
    }

    HashTableSize = atoi(buf);
    hashTable = malloc(sizeof(struct linked_list*) * HashTableSize);
    if (hashTable == NULL) {
        perror("malloc");
        exit(-1);
    }

    //################to avoid segmentation faults################
    for (int index = 0; index < HashTableSize; ++index)
        hashTable[index] = NULL;
    //############################################################

    while (fgets(buf, BUFSIZE, fconfig) != NULL) {

        if (buf[strlen(buf)-1] == '\n')
            buf[strlen(buf)-1] = '\0';

        int index = Hash(buf);

        struct linked_list *new_list = malloc(sizeof(struct linked_list));
        if (new_list == NULL) {
            perror("malloc");
            exit(-1);
        }

        new_list->next = NULL;

        if (hashTable[index] != NULL) 
            new_list->next = hashTable[index];

        hashTable[index] = new_list;

        hashTable[index]->str = malloc(sizeof(strlen(buf) + 1));
        if (hashTable[index]->str == NULL) {
            perror("malloc");
            exit(-1);
        }

        strcpy(hashTable[index]->str, buf);
        hashTable[index]->fSize = getfSize(buf);

    }

    if (ferror(fconfig)) {
        perror("fgets");
        exit(-1);
    }

    fclose(fconfig);
}

void logOpen() {
    flog = fopen(LOG_FILE, "a+");
    if (flog == NULL) {
        perror("fopen");
        exit(-1);
    }
}

void logClose() {
    fclose(flog);
}

void logMessage(char *msg) {
    fputs(msg, flog); 
    if (ferror(flog)) {
        perror("fputs");
        exit(-1);
    }
}

#endif