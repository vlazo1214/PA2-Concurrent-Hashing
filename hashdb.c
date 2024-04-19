#include "hashdb.h"
#include <stdio.h>

#define DEBUG 1

// auxiliary functions
uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length)
{
    size_t i = 0;
    uint32_t hash = 0;

    while (i != length)
	{
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
    hashRecord *newRecord = malloc(sizeof(hashRecord));
	
    if (!newRecord)
	{
        return false;
    }
    
	newRecord->hash = hash;
    strncpy(newRecord->name, name, sizeof(newRecord->name) - 1);
    newRecord->name[sizeof(newRecord->name) - 1] = '\0';
    newRecord->salary = salary;
    newRecord->next = NULL;

	return newRecord;
}

// main functions
bool insert(hashRecord *head, const char *name, uint32_t salary)
{
	if (!head)
	{
		return false;
	}

    // Calculate hash value for the given name
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)name, strlen(name));
    
	if (DEBUG) printf("debug %s, %u\n", name, hash);

    // Create new record
    hashRecord *newRecord = create_node(hash, name, salary);

    // Insert new record into hash table/linked list
    hashRecord *current = head;
    if (!current)
	{
        head = newRecord;
    }
	else
	{
        while (current->next)
		{
            current = current->next;
        }

        current->next = newRecord;
    }

    return true;
}
