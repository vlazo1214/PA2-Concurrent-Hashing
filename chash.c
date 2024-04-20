// main file that reads the commands.txt and produces output to the console

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "hashdb.h"
#include "common.h"
#include "common_threads.h"

#define MAX_COMMAND_SIZE 100
#define MAX_NAME_SIZE 100

#define DEBUG 0

int main(void)
{
    FILE *input = fopen("commands.txt", "r");
    FILE *output = fopen("output.txt", "w");
    char command[MAX_COMMAND_SIZE];
    char name[MAX_NAME_SIZE];
    uint32_t salary;
    int i, num_threads;
    threadArgs *curr_args;

    if (input == NULL || output == NULL)
    {
        printf("Error opening file\n");
        return 1;
    }

    hashRecord *head = NULL;

    char first[MAX_COMMAND_SIZE];
    fscanf(input, "%[^\n]\n", first);

    char *first_token = strtok(first, ",");


    if (strcmp(first_token, "threads") == 0)
    {
        first_token = strtok(NULL, ",");
        if (!first_token || sscanf(first_token, "%d", &num_threads) != 1)
        {
            fprintf(output, "Error reading number of threads\n");
            return 1;
        }
        // Testing
        fprintf(output, "Running %d threads\n", num_threads); // Comment this after you implement threads
    }

    pthread_t *threads = malloc(sizeof(pthread_t) * num_threads);

    for (i = 0; i < num_threads; i++)
    {
        if (DEBUG) printf("in for loop\n");

        if (fscanf(input, "%[^\n]\n", command) != 1)
        {
            if (feof(input))
            {
                print(head, output);
                break;
            }
            else
            {
                fprintf(output, "Error reading command\n");
                return 1;
            }
        }

        char *token = strtok(command, ",");
        if (!token)
        {
            fprintf(output, "Command not recognized: %s\n", command);
            continue;
        }

        if (strcmp(token, "insert") == 0)
        {
            token = strtok(NULL, ",");
            if (!token || sscanf(token, "%[^,],", name) != 1)
            {
                fprintf(output, "Error reading insert name\n");
                return 1;
            }
            token = strtok(NULL, ",");
            if (!token || sscanf(token, "%d", &salary) != 1)
            {
                fprintf(output, "Error reading insert salary\n");
                return 1;
            }
            // Testing
            //fprintf(output, "Inserting %s with salary %d\n", name, salary); // Comment this after you implement insert

            // fill arguments here
            if (DEBUG) printf("abt to fill arguments\n");

            curr_args = fillArgs(&head, name, salary, output);

            // Call insert
            pthread_create(&threads[i], NULL, insert_routine, (void *)curr_args);
            pthread_join(threads[i], NULL);
        }
        else if (strcmp(token, "delete") == 0)
        {
            token = strtok(NULL, ",");
            if (!token || sscanf(token, "%[^,],", name) != 1)
            {
                fprintf(output, "Error reading delete name\n");
                return 1;
            }
            fprintf(output, "DELETE,%s\n", name);
            // fprintf(output, "Deleting %s\n", name); // Comment this after you implement delete
            // FPRINT READ LOCK ACQUIRED
            fprintf(output, "SEARCH,%s\n", name);
            // FPRINT READ LOCK RELEASE
             if (delete(head, name, output)) // if true
             {

             }

        }
        else if (strcmp(token, "search") == 0)
        {
            token = strtok(NULL, ",");
            if (!token || sscanf(token, "%[^,],", name) != 1)
            {
                fprintf(output, "Error reading search name\n");
                return 1;
            }
            // Testing
            // fprintf(output, "Searching for %s\n", name); // Comment this after you implement search
            fprintf(output, "SEARCH,%s\n", name);
            // FPRINT READ LOCK ACQUIRED
            hashRecord *r = search(head, name, output);
            // FPRINT READ LOCK RELEASE
            fprintf(output, "%u,%s,%u\n", r->hash, r->name, r->salary);

            // Implement search here
        }
        else if (strcmp(token, "print") == 0)
        {
            // Testing
            //fprintf(output, "Printing hash table\n"); // Comment this after you implement print
            print(head, output);
            // Implement print here
        }
        else
        {
            fprintf(output, "Command not recognized: %s\n", command); //
        }
        fflush(stdout);
    }
    
    fclose(input);
    fclose(output);
    return 0;
}