/**
 * implementing a hash table to get the status of a file by name
 **/
#ifndef HASHTABLE_H
#define HASHTABLE_H

int HashTableSize = 1024;

struct linked_list
{
    char *str;
    int fSize;
    struct linked_list *next;
};

struct linked_list **hashTable;

int Hash(const char *str) 
{
    int sum = 0;

    while (*str) 
    {
        sum += *str;
        ++str;
    }

    return sum % HashTableSize;    
}

void freeHash(int size) 
{
    for (int index = 0; index < size; ++index) 
    {
        while (hashTable[index]) 
        {
            free(hashTable[index]->str);
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