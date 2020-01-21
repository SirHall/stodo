#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0

char  todoFileName[]   = ".todo";
char *fullTodoFilePath = NULL;

int   ArgExists(int argc, char **argv, char *flag);
void  PrintHelp();
char *GetTodoFilePath();
void  PrintTodo();
void  EnsureExists();
void  AppendTodo(int argc, char **argv);

int main(int argc, char *argv[])
{
    fullTodoFilePath = GetTodoFilePath();

    if (ArgExists(argc, argv, "-h") || ArgExists(argc, argv, "--help"))
        PrintHelp();
    else if (ArgExists(argc, argv, "-p"))
        PrintTodo();
    else
        AppendTodo(argc, argv);

    return 0;
}

int ArgExists(int argc, char **argv, char *flag)
{
    for (int i = 0; i < argc; i++)
        if (strcmp(argv[i], flag) == 0)
            return 1;
    return 0;
}

void PrintHelp()
{
    printf("-h --help    - Print this dialogue\n"
           "-p           - Print out todo list\n"
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

    char *        line = NULL;
    unsigned long len  = 0;
    signed long   read;
    int           index = 0;

    while ((read = getline(&line, &len, file)) != -1)
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

void DeleteLineFromTodo(int argc, char **argv) { EnsureExists(); }