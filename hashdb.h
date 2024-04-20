// Concurrent Hash Table struct definitions

#ifndef __hashdb_h__
#define __hashdb_h__

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct hash_struct
{
  uint32_t hash;
  char name[50];
  uint32_t salary;
  struct hash_struct *next;
} hashRecord;

typedef struct args_struct
{
  hashRecord **head;
  char name[50];
  uint32_t salary;
  FILE *output;
}threadArgs;

uint32_t jenkins_one_at_a_time_hash(const uint8_t *key, size_t length);

// hashRecord **head, const char *name, uint32_t salary, FILE *output
bool insert(hashRecord **head, const char *name, uint32_t salary, FILE *output);
void *insert_routine(void *arg);
// bool search(hashRecord *head, uint32_t hash);
hashRecord* search(hashRecord *hashTable, const char *name, FILE * fp);
bool delete(hashRecord *hashTable, const char *name, FILE * fp);
bool print(hashRecord *head, FILE *output);
hashRecord* copyLinkedList(const hashRecord* original);

threadArgs *fillArgs(hashRecord **head, const char *name, uint32_t salary, FILE *output);

#endif