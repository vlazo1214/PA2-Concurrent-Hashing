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

// char* search(hashRecord *hashTable[], const char *name)
hashRecord* search(hashRecord *hashTable, const char *name)
{
    hashRecord *record = hashTable;
    // bool key = false; // probably better way to do this but just to set up the code
    // call hash function to compute the hashvalue of the key
    // Calculate hash value for the given name
    // uint32_t hash = calculate_hash(name);
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)name, strlen(name));

    // locate the corresponding node
    // record = hashTable[hash];
    while (record!= NULL)
    {
        // check the current record's hashval
        if (record->hash == hash)
            break;
        record = record->next;
    }
    // now we have the right record

    // acquire read lock
    // printf("READ LOCK ACQUIRED");
    // protect list

    // search linked list for the key

    // if key is not found: 
    if (record == NULL)
    {
        // probably need fprintf? but also I'd need the file name as a parameter
        printf("No Record Found");
        // release read lock and return -> probably in main
        return NULL;
    }

    // if key is found:
    if (record != NULL)
    {
        // print record
        printf("%u,%s,%u", record->hash, record->name, record->salary); // FORMAT LATER WHEN FIGURE OUT FORMATTING

        return record;
    }
    return record;
    // IN CHASH, IF SEARCH() == NULL, 
    // ELSE PRINT ("%") RECORD.HASH,RECORD.
}

bool delete(hashRecord *hashTable, const char *name, FILE * fp)
{ 
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)name, strlen(name));
    hashRecord *r = hashTable;
    hashRecord *previous = NULL;

    // if the head is what needs to change
    if (r->hash == hash)
    {
        hashTable = hashTable->next;
        fprintf("%s", r->name);
        free(r->name);
        free(r);
        return true;
    }
    // it's not the head, so start looking next
    previous = r;
    r = r->next;

    // at the start of the loop we're still at the head
    while (r != NULL)
    {    
        // if we found the hash value
        if (r->hash == hash)
        {
            // remove record from linked list
            previous->next = r->next;
            fprintf("%s", r->name);
            // free record memory
            free(r->name);
            free(r);
            return true;
        }
        previous = r;
        r = r->next;
    }
    // by here, r is null/at the end and still no match, so return false;
    return false;
}