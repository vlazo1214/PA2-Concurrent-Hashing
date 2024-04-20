// Concurrent Hash Table struct definitions

#ifndef __hashdb_h__
#define __hashdb_h__

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#define NUM_BUCKETS 13

typedef struct hash_struct
{
  uint32_t hash;
  char name[50];
  uint32_t salary;
  struct hash_struct *next;
} hashRecord;


uint32_t jenkins_one_at_a_time_hash(const uint8_t *key, size_t length);

bool insert(hashRecord **head, const char *name, uint32_t salary, FILE *output);
// bool search(hashRecord *head, uint32_t hash);
// bool delete(hashRecord *head, uint32_t hash);
bool print(hashRecord *head, FILE *output);


#endif