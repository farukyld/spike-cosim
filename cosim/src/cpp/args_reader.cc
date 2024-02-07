// write a c header and source to read arguments from a file.
// it should return a pointer to a struct containing int argc and char** argv fields.
// chatGPT:

#include "args_reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug_header.h"

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

// Function to read arguments from a file
argv_argc_t *read_args_from_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, __FILE__ ":%d: couldn't open file %s\n",__LINE__, filename);
        exit(1);
    }
    DEBUG_PRINT_WARN("filename = %s, file pointer = %p\n", filename, file);
    // Read file content
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *content = (char *)malloc(length + 1);
    if (!content)
    {
        fprintf(stderr, __FILE__ ":%d: couldn't allocate space for file content\n",__LINE__);
        fclose(file);
        exit(1);
    }
    fread(content, 1, length, file);
    // print the content that is read from the file
    printf("\n--running cosim with arguments:--\n %s\n"
    "------------------------------------------\n",
    content);
    fclose(file);
    content[length] = '\0';

    // Count arguments
    int argc = count_args(content);
    DEBUG_PRINT_WARN("argc = %d\n", argc);

    char **argv = (char **)malloc(sizeof(char *) * argc);
    if (!argv)
    {
        fprintf(stderr, __FILE__ ":%d: couldn't allocate space for argv\n",__LINE__);
        free(content);
        exit(1);
    }

    // Parse arguments
    char *token = strtok(content, " ");
    int i = 0;
    while (token)
    {
        DEBUG_PRINT_WARN("token[%d] = %s\n",i, token);
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
        free(content);
        fprintf(stderr, __FILE__ ":%d: couldn't allocate space for args\n",__LINE__);
        return NULL;
    }
    args->argc = argc;
    args->argv = argv;

    // Free the original content as strdup made copies
    free(content);

    return args;
}


#include <string>
#include <cstring>

std::string get_directory_path(const std::string& file_path) {
    size_t ind_last_dir_sep = file_path.find_last_of("/\\");
    if (ind_last_dir_sep != std::string::npos) {
        return file_path.substr(0, ind_last_dir_sep);
    }
    fprintf(stderr,__FILE__ ":%d: cannot extract the directory containing %s",__LINE__, file_path);
    exit(1);
    return "";
}
