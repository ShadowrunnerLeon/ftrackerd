/**
 * implementing a hash table to get the status of a file by name
 **/
#ifndef HASHTABLE_H
#define HASHTABLE_H

int HashTableSize = 1024;

struct linked_list
{
    char *filename;
    size_t size;
    struct linked_list *next;
};

struct linked_list **hashTable = NULL;

int Hash(const char *filename) 
{
    int sum = 0;

    while (*filename) 
    {
        sum += *filename;
        ++filename;
    }

    return sum % HashTableSize;    
}

void freeHash(int size) 
{
    for (int index = 0; index < size; ++index) 
    {
        while (hashTable[index]) 
        {
            free(hashTable[index]->filename);
            struct linked_list *hashValue = hashTable[index];
            hashTable[index] = hashTable[index]->next;
            free(hashValue);
        }
    }

    free(hashTable);
    for (int index = 0; index < size; ++index)
    {
        hashTable[index] = NULL;
    }
}

#endif