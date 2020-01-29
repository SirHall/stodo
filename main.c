#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0

#define MAX_TODO_LENGTH 256

char  todoFileName[]   = ".todo";
char *fullTodoFilePath = NULL;

int   ArgIndex(int argc, char **argv, char *flag);
void  PrintHelp();
char *GetTodoFilePath();
void  PrintTodo();
void  EnsureExists();
void  AppendTodo(int argc, char **argv);
void  DeleteLineFromTodo(int argc, char **argv, int deleteFlagIndex);

int main(int argc, char *argv[])
{
    fullTodoFilePath = GetTodoFilePath();

    int deleteIndex = -1;

    if (ArgIndex(argc, argv, "-h") != -1 ||
        ArgIndex(argc, argv, "--help") != -1)
        PrintHelp();
    else if (argc <= 1 || ArgIndex(argc, argv, "-p") != -1)
        PrintTodo();
    else if ((deleteIndex = ArgIndex(argc, argv, "-d")) != -1)
        DeleteLineFromTodo(argc, argv, deleteIndex);
    else
        AppendTodo(argc, argv);

    return 0;
}

int ArgIndex(int argc, char **argv, char *flag)
{
    for (int i = 0; i < argc; i++)
        if (strcmp(argv[i], flag) == 0)
            return i;
    return -1;
}

void PrintHelp()
{
    printf("-h --help    - Print this dialogue\n"
           "noflags -p   - Print out todo list\n"
           "-d index     - Delete entry by index\n"
           "msg          - Add entry\n"
           "-i index msg - Insert entry before index\n");
}

char *GetTodoFilePath()
{
    char *loc =
        (char *)malloc(strlen(getenv("HOME")) + strlen(todoFileName) + 2);
    strcat(loc, getenv("HOME"));
    strcat(loc, "/");
    strcat(loc, todoFileName);
    return loc;
}

void EnsureExists()
{
    FILE *file = fopen(fullTodoFilePath, "ab+");
    fclose(file);
}

void PrintTodo()
{
    EnsureExists();

    FILE *file = fopen(fullTodoFilePath, "r");
    if (file == NULL)
        exit(EXIT_FAILURE);

    int  index = 0;
    char line[MAX_TODO_LENGTH];

    while (fgets(line, MAX_TODO_LENGTH, file) != NULL)
        printf("%d %s", index++, line);

    fclose(file);
}

void AppendTodo(int argc, char **argv)
{
    EnsureExists();
    FILE *file       = fopen(fullTodoFilePath, "a");
    char *strToWrite = (char *)malloc(1);
    strToWrite[0]    = '\0';

    for (int i = 1; i < argc; i++)
    {
        strcat(strToWrite, argv[i]);
        if (i < argc - 1)
            strcat(strToWrite, " ");
    }

    strcat(strToWrite, "\n");
    fputs(strToWrite, file);
    free(strToWrite);
    fclose(file);
}

void DeleteLineFromTodo(int argc, char **argv, int deleteFlagIndex)
{
    EnsureExists();

    // Find out which index to delete, if no index is given, delete 0 by
    // default
    int deleteIndex = -1;

    if ((deleteFlagIndex + 1) < argc)
    {
        int len = strlen(argv[deleteFlagIndex + 1]);
        for (int i = 0; i < len; i++)
        {
            if (!isdigit(argv[deleteFlagIndex + 1][i]))
            {
                printf("'%s' is not recognized as a todo entry index\n",
                       argv[deleteFlagIndex + 1]);
                return;
            }
        }

        // Find out which index the user wishes to delete.
        // atoi() 'should' be safe here as we already checked to make sure
        // that the argument is completely made up of digits.
        deleteIndex = atoi(argv[deleteFlagIndex + 1]);
    }
    else
    {
        // The user has not specified which index to delete
        deleteIndex = 0; // Default to 0
    }

    // Read lines from todofile

    FILE * todoFileRead = fopen(fullTodoFilePath, "r");
    char **lines        = (char **)malloc(1);

    char *newLine = calloc(MAX_TODO_LENGTH, sizeof(char));

    int linesCount = 0;

    // int readingCurrentLine = 0;
    while (fgets(newLine, MAX_TODO_LENGTH, todoFileRead) != NULL)
    {
        // if (readingCurrentLine == deleteIndex || !isgraph(newLine[0]))
        //     continue; // Don't save this line

        lines = realloc(lines, (linesCount + 1) * sizeof(char **));
        linesCount++;
        lines[linesCount - 1] = newLine;
        newLine               = (char *)calloc(MAX_TODO_LENGTH, sizeof(char));

        // readingCurrentLine++;
    }

    fclose(todoFileRead);

    if (deleteIndex >= linesCount)
    {
        printf("%d is not a valid line index for deletion\n", deleteIndex);
        exit(EXIT_FAILURE);
    }

    // Write all lines to todo file, except the deleted line

    FILE *todoFileWrite = fopen(fullTodoFilePath, "w");

    for (int i = 0; i < linesCount; i++)
    {
        if (i != deleteIndex && lines[i][0] != 0)
        {
            fputs(lines[i], todoFileWrite);
            free(lines[i]);
        }
    }
    free(lines);

    fclose(todoFileWrite);
}