/**
 * implementations of functions for working with configs and logs
 **/
#ifndef LOGLIB_H
#define LOGLIB_H

#include "HashTable.h"

#define BUFSIZE 1024

const char *CONFIG_FILE = "/tmp/ftracker.config";
const char *LOG_FILE = "/tmp/ftracker.log";

FILE *flog = NULL;

int getSize(char *filename) 
{
    struct stat file;
    if (stat(filename, &file) == -1) 
    {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    return file.st_size;
}

void readConfig() 
{
    /**
     * @brief 
     * get count of files from config and initialize hash table
     */
    char buf[BUFSIZE];
    
    FILE *fconfig = fopen(CONFIG_FILE, "r");

    fgets(buf, BUFSIZE, fconfig);
    if (ferror(fconfig)) 
    {
        perror("fgets");
        exit(EXIT_FAILURE);
    }

    HashTableSize = atoi(buf);
    hashTable = malloc(sizeof(struct linked_list*) * HashTableSize);
    if (!hashTable) 
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    //################to avoid segmentation faults################
    for (int index = 0; index < HashTableSize; ++index)
    {
        hashTable[index] = NULL;
    } 
    //############################################################

    /**
     * @brief 
     * add information aboit file to hash table
     */

    while (fgets(buf, BUFSIZE, fconfig)) 
    {
        if (buf[strlen(buf)-1] == '\n')
        {
            buf[strlen(buf)-1] = '\0';
        } 

        int index = Hash(buf);

        struct linked_list *new_list = malloc(sizeof(struct linked_list));
        if (!new_list) 
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        new_list->next = NULL;

        if (hashTable[index])
        {
            new_list->next = hashTable[index];
        } 

        hashTable[index] = new_list;

        hashTable[index]->filename = malloc(sizeof(strlen(buf) + 1));
        if (!hashTable[index]->filename) 
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        strcpy(hashTable[index]->filename, buf);
        hashTable[index]->size = getSize(buf);
    }

    if (ferror(fconfig)) 
    {
        perror("fgets");
        exit(EXIT_FAILURE);
    }

    fclose(fconfig);
}

void logOpen() 
{
    flog = fopen(LOG_FILE, "a+");
    if (!flog) 
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
}

void logClose() 
{
    fclose(flog);
}

void logMessage(char *msg) 
{
    fprintf(flog, msg);
}

#endif