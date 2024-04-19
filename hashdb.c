#include "hashdb.h"
#include <stdio.h>

uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length) {
    size_t i = 0;
    uint32_t hash = 0;
    while (i != length) {
    hash += key[i++];
    hash += hash << 10;
    hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

bool insert(hashRecord *hashTable[], const char *name, uint32_t salary) {
    // Calculate hash value for the given name
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)name, strlen(name));
    printf("debug %s, %u\n", name, hash);

    // Create new record
    hashRecord *newRecord = malloc(sizeof(hashRecord));
    if (!newRecord) {
        return false;
    }
    newRecord->hash = hash;
    strncpy(newRecord->name, name, sizeof(newRecord->name) - 1);
    newRecord->name[sizeof(newRecord->name) - 1] = '\0';
    newRecord->salary = salary;
    newRecord->next = NULL;

    //acquire_write_lock(hash);

    // Insert new record into hash table/linked list
    hashRecord *current = hashTable[hash % NUM_BUCKETS];
    if (!current) {
        hashTable[hash % NUM_BUCKETS] = newRecord;
    } else {
        while (current->next) {
            current = current->next;
        }
        current->next = newRecord;
    }

    //release_write_lock(hash);

    return true;
}

char* search(hashRecord *hashTable[], const char *name)
{

    bool key = false; // probably better way to do this but just to set up the code
    // call hash function to compute the hashvalue of the key
    // Calculate hash value for the given name
    uint32_t hash = calculate_hash(name);

    // locate the corresponding node

    // acquire read lock
    // protect list
    // search linked list for the key

    // if key is not found: 
    if (!key)
    {
        // probably need fprintf? but also I'd need the file name as a parameter
        printf("No Record Found");
        return null;

    }

    // if key is found:
    if (key)
    {
        // print record
        print(name) // FORMAT LATER WHEN FIGURE OUT FORMATTING
        // return value
    }
    // release read lock and return

}

void delete(hashRecord *hashTable[], const char *name)
{
    // call hash function to compute the hashvalue of the key

    // locate the corresponding node

    // acquire write lock
    // protect list
    // search linked list for the key

    // if not found: 
    // do nothing (and return?)

    // if found:
    // remove node from list
    // free memory

    // release write lock and return    
}