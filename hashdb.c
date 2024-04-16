#include "hashdb.h"

uint32_t calculate_hash(const char *str) {
    uint32_t hash = 5381;

    while (*str) {
        hash = ((hash << 5) + hash) + *str++; 
    }

    return hash;
}

bool insert(hashRecord *hashTable[], const char *name, uint32_t salary) {
    // Calculate hash value for the given name
    uint32_t hash = calculate_hash(name);

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

