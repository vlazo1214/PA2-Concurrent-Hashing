// Concurrent Hash Table struct definitions

#ifndef __hashdb_h__
#define __hashdb_h__

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


uint32_t calculate_hash(const char *str);

bool insert(hashRecord *hashTable[], const char *name, uint32_t salary);

#endif