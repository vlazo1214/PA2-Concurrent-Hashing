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

int main(void)
{
    FILE *input = fopen("commands.txt", "r");
    FILE *output = fopen("output.txt", "w");
    char command[MAX_COMMAND_SIZE];
    char name[MAX_NAME_SIZE];
    uint32_t salary;
    int i, num_threads;
    threadArgs *curr_args;
    int count = 0;

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

            curr_args = fillArgs(&head, name, salary, output);

            // Call insert
            pthread_create(&threads[i], NULL, (void *)insert_routine, (void *)curr_args);
            pthread_join(threads[i], NULL);
            ++count;
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

            searchArgs *delete_args = (searchArgs *)malloc(sizeof(searchArgs));
            delete_args->hashTable = head;
            delete_args->name = name;
            delete_args->output = output;

            pthread_create(&threads[i], NULL, delete_routine, delete_args);
            pthread_join(threads[i], NULL);
            count = count + 2;
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
            // Inside the main function

    // Prepare arguments for search routine
        searchArgs *search_args = (searchArgs *)malloc(sizeof(searchArgs));
        search_args->hashTable = head;
        search_args->name = name;
        search_args->output = output;


        // Create thread for search operation
        pthread_create(&threads[i], NULL, search_routine, (void *)search_args);
            pthread_join(threads[i], NULL);
            ++count;
        }
        else if (strcmp(token, "print") == 0)
        {
            // Testing
            //fprintf(output, "Printing hash table\n"); // Comment this after you implement print
            print(head, output);
        }
        else
        {
            fprintf(output, "Command not recognized: %s\n", command); //
        }
        fflush(stdout);
    }

    fprintf(output, "Number of lock acquisitions:  %d\n", count + 1);
    fprintf(output, "Number of lock releases:  %d\n", count + 1);
    print(head, output);
    printf("Output written to output.txt\n");

    fclose(input);
    fclose(output);
    return 0;
}