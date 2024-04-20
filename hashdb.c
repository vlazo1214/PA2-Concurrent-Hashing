#include "hashdb.h"
#include <stdio.h>

#define DEBUG 1

// auxiliary functions
uint32_t jenkins_one_at_a_time_hash(const uint8_t *key, size_t length) {
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

hashRecord *create_node(uint32_t hash, const char *name, uint32_t salary)
{
    // Allocate memory for the new record
    hashRecord *newRecord = (hashRecord*)malloc(sizeof(hashRecord));
    if (newRecord == NULL) {
        printf("Memory allocation failed\n");
        return false; 
    }

    // Populate the fields of the new record
    newRecord->hash = hash;
    strncpy(newRecord->name, name, sizeof(newRecord->name)-1);
    newRecord->name[sizeof(newRecord->name)-1] = '\0';
    newRecord->salary = salary;
    newRecord->next = NULL;

    return newRecord;
}

bool insert(hashRecord **head, const char *name, uint32_t salary, FILE *output) {
    // Calculate hash based on the name
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t *)name, strlen(name));

    // If the list is empty, make the new record the head
    if (*head == NULL) {
        *head = create_node(hash, name, salary);
        return true; 
    }

    // Traverse the list to find the last node
    hashRecord *current = *head;
    while (current->next != NULL) {
        current = current->next;
    }

    // Append the new record to the end of the list
    current->next = create_node(hash, name, salary);
    return true; 
}

bool print(hashRecord *head, FILE *output)
{
    if (!head)
    {
        return false;
    }
    // Sort the linked list based on hash values (Insertion Sort)
    hashRecord *sorted = NULL;
    hashRecord *current = head;
    while (current != NULL)
    {
        hashRecord *temp = current;
        current = current->next;
        if (sorted == NULL || sorted->hash >= temp->hash)
        {
            temp->next = sorted;
            sorted = temp;
        }
        else
        {
            hashRecord *current_sorted = sorted;
            while (current_sorted->next != NULL && current_sorted->next->hash < temp->hash)
            {
                current_sorted = current_sorted->next;
            }
            temp->next = current_sorted->next;
            current_sorted->next = temp;
        }
    }
    head = sorted;

    // Print the sorted linked list
    hashRecord *temp = head;
    while (temp != NULL)
    {
        fprintf(output, "%u,%s,%u\n", temp->hash,temp->name, temp->salary);
        printf("%u,%s,%u\n", temp->hash, temp->name, temp->salary);
        temp = temp->next;
    }

    // Free allocated memory
    while (head != NULL)
    {
        hashRecord *temp = head;
        head = head->next;
        free(temp);
    }

    return true;
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