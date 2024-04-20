// main file that reads the commands.txt and produces output to the console

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashdb.h"
#include "rwlocks.h"
#include "common.h"
#include "common_threads.h"

#define MAX_COMMAND_SIZE 100
#define MAX_NAME_SIZE 100

int main(void)
{
    FILE *input = fopen("commands.txt", "r");
    FILE *output = fopen("test.txt", "w");
    char command[MAX_COMMAND_SIZE];
    char name[MAX_NAME_SIZE];
    uint32_t salary;

    if (input == NULL || output == NULL)
    {
        printf("Error opening file\n");
        return 1;
    }

    hashRecord *head = NULL;

    while (1)
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

        if (strcmp(token, "threads") == 0)
        {
            token = strtok(NULL, ",");
            if (!token || sscanf(token, "%d", &salary) != 1)
            {
                fprintf(output, "Error reading number of threads\n");
                return 1;
            }
            // Testing
            fprintf(output, "Setting number of threads to %d\n", salary); // Comment this after you implement threads

        }
        else if (strcmp(token, "insert") == 0)
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

            // Call insert
            if (!insert(&head, name, salary, output))
            {
                fprintf(output, "Error inserting record\n");
                return 1;
            }
        }
        else if (strcmp(token, "delete") == 0)
        {
            token = strtok(NULL, ",");
            if (!token || sscanf(token, "%[^,],", name) != 1)
            {
                fprintf(output, "Error reading delete name\n");
                return 1;
            }
            fprintf(output, "DELETE,%s", name);
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
            hashRecord *r = search(head, name);
            // FPRINT READ LOCK RELEASE
            fprintf(output, "%u,%s,%u\n", r->hash, r->name, r->salary);

            // Implement search here
        }
        else if (strcmp(token, "print") == 0)
        {
            // Testing
            fprintf(output, "Printing hash table\n"); // Comment this after you implement print
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