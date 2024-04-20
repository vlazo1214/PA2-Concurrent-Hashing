#include "hashdb.h"
#include <stdio.h>
#include <pthread.h>

#define DEBUG 1

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
threadArgs *curr_args;

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

threadArgs *fillArgs(hashRecord **head, const char *name, uint32_t salary, FILE *output)
{
    if (DEBUG) printf("in fill arguments\n");
    
    // Allocate memory for the new record
    threadArgs *newArgs = (threadArgs*)malloc(sizeof(threadArgs));

    if (DEBUG) printf("malloc'd\n");

    // Populate the fields of the new record
    newArgs->head = &head;

    strncpy(newArgs->name, name, sizeof(newArgs->name)-1);
    newArgs->name[sizeof(newArgs->name)-1] = '\0';
    newArgs->salary = salary;

    newArgs->output = output;

    if (DEBUG) printf("abt to exit fill arguments\n");

    return newArgs;
}

hashRecord *create_node(uint32_t hash, const char *name, uint32_t salary)
{
    // Allocate memory for the new record
    hashRecord *newRecord = (hashRecord*)malloc(sizeof(hashRecord));
    if (newRecord == NULL) {
        printf("Memory allocation failed\n");
        return NULL; 
    }

    // Populate the fields of the new record
    newRecord->hash = hash;
    strncpy(newRecord->name, name, sizeof(newRecord->name)-1);
    newRecord->name[sizeof(newRecord->name)-1] = '\0';
    newRecord->salary = salary;
    newRecord->next = NULL;

    return newRecord;
}

void *insert_routine(void *arg)
{
    if (DEBUG) printf("in insert routine\n");

    // acquire lock
    pthread_mutex_lock(&mutex);

    insert(curr_args->head, curr_args->name, curr_args->salary, curr_args->output);

    // release lock
    pthread_mutex_unlock(&mutex);
}

bool insert(hashRecord **head, const char *name, uint32_t salary, FILE *output) {
    if (DEBUG) printf("in actual insert\n");

    // Calculate hash based on the name
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t *)name, strlen(name));

    fprintf(output, "INSERT,%u,%s,%u\n", hash, name, salary);

    if (DEBUG) printf("printed to file\n");

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

hashRecord* copyHashRecord(const hashRecord* original) {
    if (!original)
        return NULL;

    hashRecord* copy = malloc(sizeof(hashRecord));
    if (!copy)
        return NULL;

    // Copy the data
    copy->hash = original->hash;
    strncpy(copy->name, original->name, sizeof(original->name));
    copy->salary = original->salary;
    copy->next = NULL;

    return copy;
}

hashRecord* copyLinkedList(const hashRecord* original) {
    if (!original)
        return NULL;

    hashRecord* new_head = NULL;
    hashRecord* tail = NULL;

    while (original) {
        hashRecord* copied_node = copyHashRecord(original);
        if (!copied_node) {
            return NULL;
        }

        if (!new_head) {
            new_head = copied_node;
            tail = copied_node;
        } else {
            tail->next = copied_node;
            tail = copied_node;
        }

        original = original->next;
    }

    return new_head;
}

bool print(hashRecord *head, FILE *output)
{
    if (!head)
    {
        return false;
    }

    // Copy the linked list so original stays in tact
    hashRecord* copied_list = copyLinkedList(head);
    if (!copied_list) {
        return false;
    }

    // Sort the copied linked list based on hash values (Insertion Sort)
    hashRecord *sorted = NULL;
    hashRecord *current = copied_list;
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

    // Print the sorted linked list
    hashRecord *temp = sorted;
    while (temp != NULL)
    {
        fprintf(output, "%u,%s,%u\n", temp->hash,temp->name, temp->salary);
        //printf("%u,%s,%u\n", temp->hash, temp->name, temp->salary);
        temp = temp->next;
    }

    // Free the copied linked list
    while (copied_list != NULL)
    {
        hashRecord *temp = copied_list;
        copied_list = copied_list->next;
        free(temp);
    }

    return true;
}
// char* search(hashRecord *hashTable[], const char *name)
hashRecord* search(hashRecord *hashTable, const char *name, FILE * fp)
{
    hashRecord *record = hashTable;
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
        printf("No Record Found");
        fprintf(fp, "No Record Found");
        // release read lock and return -> probably in main
        return NULL;
    }

    // if key is found:
    if (record != NULL)
    {
        // print record
        printf("Search- Key found: %u,%s,%u\n", record->hash, record->name, record->salary); // FORMAT LATER WHEN FIGURE OUT FORMATTING
        //fprintf(fp, "%u,%s,%u\n", record->hash, record->name, record->salary);
        //fprintf(fp, "Search- Key found: %u,%s,%u\n", record->hash, record->name, record->salary);

        return record;
    }
    return record;
    // IN CHASH, IF SEARCH() == NULL, 
    // ELSE PRINT ("%") RECORD.HASH,RECORD.
}

bool delete(hashRecord *hashTable, const char *name, FILE * fp)
{ 
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)name, strlen(name));
    hashRecord *prev = NULL;
    hashRecord *curr = hashTable;

    // Traverse the linked list
    while (curr != NULL) {
        // Check if the current node's hash matches
        if (curr->hash == hash) {
            // Found the node to delete
            if (prev == NULL) {
                // If it's the first node
                hashTable = curr->next;
            } else {
                // If it's not the first node
                prev->next = curr->next;
            }

            // Free the memory of the node to be deleted
            printf("Delete- Deleting: %s\n", curr->name);
            fprintf(fp, "%u,%s,%u\n", curr->hash, curr->name, curr->salary);
            //fprintf(fp, "Delete- Deleting: %s\n", curr->name);
            free(curr);
            return true; 
        }

        prev = curr;
        curr = curr->next;
    }

    return false;
}