// Concurrent Hash Table struct definitions

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#define NUM_BUCKETS 13

typedef struct hash_struct
{
  uint32_t hash;
  char name[50];
  uint32_t salary;
  struct hash_struct *next;
} hashRecord;

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

