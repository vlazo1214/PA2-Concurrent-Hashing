#include "hashdb.h"
#include <stdio.h>

#define DEBUG 1
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

bool insert(hashRecord **head, const char *name, uint32_t salary) {
    // Calculate hash based on the name
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t *)name, strlen(name));

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

    // If the list is empty, make the new record the head
    if (*head == NULL) {
        *head = newRecord;
        return true; 
    }

    // Traverse the list to find the last node
    hashRecord *current = *head;
    while (current->next != NULL) {
        current = current->next;
    }

    // Append the new record to the end of the list
    current->next = newRecord;
    return true; 
}

bool print(hashRecord *head)
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
    FILE *output = fopen("output.txt", "w");

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
}