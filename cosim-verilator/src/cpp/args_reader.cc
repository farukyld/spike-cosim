// write a c header and source to read arguments from a file.
// it should return a pointer to a struct containing int argc and char** argv fields.
// chatGPT:

#include "args_reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug_header.h"
#include <string>
#include <cstring>


// Helper function to count the number of arguments in a file
static int count_args(const char *str)
{
    int count = 0;
    const char *p = str;

    while (*p)
    {
        while (*p && *p == ' ')
            p++; // Skip spaces
        if (*p)
            count++;
        while (*p && *p != ' ')
            p++; // Skip non-spaces
    }

    return count;
}

// Function to read arguments from "arg1 arg2 arg3"
argv_argc_t *split_args(char *combined_args)
{
    // Count arguments
    int argc = count_args(combined_args);
    DEBUG_PRINT_WARN("argc = %d\n", argc);

    char **argv = (char **)malloc(sizeof(char *) * argc);
    if (!argv)
    {
        fprintf(stderr, __FILE__ ":%d: couldn't allocate space for argv\n",__LINE__);
        exit(1);
    }

    // Parse arguments
    char *token = strtok(combined_args, " ");
    int i = 0;
    while (token)
    {
        argv[i++] = strdup(token);
        token = strtok(NULL, " ");
    }

    // Create and populate argv_argc_t
    argv_argc_t *args = (argv_argc_t *)malloc(sizeof(argv_argc_t));
    if (!args)
    {
        for (int j = 0; j < i; j++)
            free(argv[j]);
        free(argv);
        fprintf(stderr, __FILE__ ":%d: couldn't allocate space for args\n",__LINE__);
        return NULL;
    }
    args->argc = argc;
    args->argv = argv;

    return args;
}
