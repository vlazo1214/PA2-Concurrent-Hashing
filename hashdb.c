#include "hashdb.h"
#include <stdio.h>
#include <pthread.h>

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
    
    // Allocate memory for the new record
    threadArgs *newArgs = (threadArgs*)malloc(sizeof(threadArgs));

    // Populate the fields of the new record
    newArgs->head = head;

    strncpy(newArgs->name, name, sizeof(newArgs->name)-1);
    newArgs->name[sizeof(newArgs->name)-1] = '\0';
    newArgs->salary = salary;

    newArgs->output = output;

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

void insert_routine(void *arg)
{


    pthread_mutex_lock(&mutex);
    threadArgs *curr_args = (threadArgs *)arg;

    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t *)curr_args->name, strlen(curr_args->name));
    fprintf(curr_args->output, "INSERT,%u,%s,%u\n", hash, curr_args->name, curr_args->salary);

    fprintf(curr_args->output, "WRITE LOCK ACQUIRED\n");

    // acquire lock

    insert(curr_args->head, curr_args->name, curr_args->salary, curr_args->output);

    // release lock
    pthread_mutex_unlock(&mutex);
    fprintf(curr_args->output, "WRITE LOCK RELEASED\n");

    return;
}

void insert(hashRecord **head, const char *name, uint32_t salary, FILE *output) {
    output = output;
    // Calculate hash based on the name
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t *)name, strlen(name));

    //printf("INSERT,%u,%s,%u\n", hash, name, salary);
    //fprintf(output, "INSERT,%u,%s,%u\n", hash, name, salary);


    // If the list is empty, make the new record the head
    if (*head == NULL) {
        *head = create_node(hash, name, salary);
        return; 
    }

    // Traverse the list to find the last node
    hashRecord *current = *head;
    while (current->next != NULL) {
        current = current->next;
    }

    // Append the new record to the end of the list
    current->next = create_node(hash, name, salary);
    return; 
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
    fprintf(output, "READ LOCK ACQUIRED\n");

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

    fprintf(output, "READ LOCK RELEASED\n");

    return true;
}
// char* search(hashRecord *hashTable[], const char *name)
hashRecord* search(hashRecord *hashTable, const char *name, FILE *fp)
{
    pthread_mutex_lock(&mutex); 
    fprintf(fp, "READ LOCK ACQUIRED\n");

    hashRecord *record = hashTable;
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)name, strlen(name));

    while (record != NULL)
    {
        if (record->hash == hash)
            break;
        record = record->next;
    }

    if (record == NULL)
    {
        printf("No Record Found\n");
        fprintf(fp, "No Record Found\n");
        pthread_mutex_unlock(&mutex);
        return NULL;
    }


    pthread_mutex_unlock(&mutex); // Release mutex lock
    fprintf(fp, "READ LOCK RELEASED\n");
    
    fprintf(fp, "%u,%s,%u\n", record->hash, record->name, record->salary);

    return record;
}

void delete_real(hashRecord *hashTable, const char *name, FILE * fp)
{ 
    pthread_mutex_lock(&mutex); 
    fprintf(fp, "WRITE LOCK ACQUIRED\n");

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
            //fprintf(fp, "Delete- Deleting: %s\n", curr->name);
            free(curr);


            pthread_mutex_unlock(&mutex); 
            fprintf(fp, "WRITE LOCK RELEASED\n");
            return; 
        }

        prev = curr;
        curr = curr->next;
    }

    pthread_mutex_unlock(&mutex); 
    fprintf(fp, "WRITE LOCK RELEASED\n");

    return;
}

void *search_routine(void *arg) {
        searchArgs *args = (searchArgs *)arg;

    // Perform search operation within the critical section protected by mutex
    search(args->hashTable, args->name, args->output);

    return NULL;
}

void *delete_routine(void *arg)
{
    searchArgs *args = (searchArgs *)arg;

    if (!delete_prelude(args))
    {
        return NULL;
    }

    delete_real(args->hashTable, args->name, args->output);

    return NULL;
}

bool delete_prelude(searchArgs *delete_args)
{
    pthread_mutex_lock(&mutex); 
    fprintf(delete_args->output, "READ LOCK ACQUIRED\n");

    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)delete_args->name, strlen(delete_args->name));
    
    fprintf(delete_args->output, "SEARCH,%s\n", delete_args->name);

    hashRecord *curr = delete_args->hashTable;

    while (curr != NULL)
    {
        if (curr->hash == hash)
            break;
        curr = curr->next;
    }

    if (curr == NULL)
    {
        printf("No Record Found\n");
        fprintf(delete_args->output, "No Record Found\n");
        pthread_mutex_unlock(&mutex);
        return false;
    }

    // printf("Search- Key found: %u,%s,%u\n", curr->hash, curr->name, curr->salary);

    pthread_mutex_unlock(&mutex); 
    fprintf(delete_args->output, "READ LOCK RELEASED\n");
    
    fprintf(delete_args->output, "%u,%s,%u\n", curr->hash, curr->name, curr->salary);
    
    return true;
}