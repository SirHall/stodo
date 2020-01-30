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
void  InsertTodo(int argc, char **argv, int insertFlagIndex);

int main(int argc, char *argv[])
{
    fullTodoFilePath = GetTodoFilePath();

    int flagIndex = -1;

    if (ArgIndex(argc, argv, "-h") != -1 ||
        ArgIndex(argc, argv, "--help") != -1)
        PrintHelp();
    else if (argc <= 1 || ArgIndex(argc, argv, "-p") != -1)
        PrintTodo();
    else if ((flagIndex = ArgIndex(argc, argv, "-d")) != -1)
        DeleteLineFromTodo(argc, argv, flagIndex);
    else if ((flagIndex = ArgIndex(argc, argv, "-i")) != -1)
        InsertTodo(argc, argv, flagIndex);
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

char *CombineArgvIntoMsg(int argc, char **argv, int startIndex, int count)
{
    char *str = (char *)malloc(1);
    str[0]    = '\0';
    for (int i = startIndex; i < startIndex + count; i++)
    {
        strcat(str, argv[i]);
        if (i < argc - 1)
            strcat(str, " ");
    }

    strcat(str, "\n");
    return str;
}

void AppendTodo(int argc, char **argv)
{
    EnsureExists();
    FILE *file       = fopen(fullTodoFilePath, "a");
    char *strToWrite = CombineArgvIntoMsg(argc, argv, 1, argc - 1);

    fputs(strToWrite, file);
    free(strToWrite);
    fclose(file);
}

char **ReadTodo(int *linesCount)
{
    EnsureExists();

    FILE * todoFileRead = fopen(fullTodoFilePath, "r");
    char **lines        = (char **)malloc(1);

    char *newLine = calloc(MAX_TODO_LENGTH, sizeof(char));

    *linesCount = 0;

    while (fgets(newLine, MAX_TODO_LENGTH, todoFileRead) != NULL)
    {

        lines = realloc(lines, (*linesCount + 1) * sizeof(char **));
        (*linesCount)++;
        lines[*linesCount - 1] = newLine;
        newLine                = (char *)calloc(MAX_TODO_LENGTH, sizeof(char));
    }

    free(newLine);
    fclose(todoFileRead);

    return lines;
}

void FreeTodoStructure(char **todoData, int linesCount)
{
    for (int i = 0; i < linesCount; i++)
        free(todoData[i]);
    free(todoData);
}

int IsStringANumber(char const *str)
{
    int len = strlen(str);
    for (int i = 0; i < len; i++)
        if (!isdigit(str[i]))
            return false;
    return true;
}

// Returns whether or not the flag was found
int GetFlagValWithDefault(int argc, char **argv, int flagIndex,
                          int defaultValue, int *flagValue)
{
    if ((flagIndex + 1) < argc)
    {
        if (!IsStringANumber(argv[flagIndex + 1]))
            return false;
        *flagValue = atoi(argv[flagIndex + 1]);
    }
    else
    {
        *flagValue = defaultValue;
    }

    return true;
}

void DeleteLineFromTodo(int argc, char **argv, int deleteFlagIndex)
{
    // Find out which index to delete, if no index is given, delete 0 by
    // default
    int deleteIndex = -1;
    GetFlagValWithDefault(argc, argv, deleteFlagIndex, 0, &deleteIndex);

    // Read lines from todofile
    int    linesCount = 0;
    char **lines      = ReadTodo(&linesCount);

    if (deleteIndex >= linesCount)
    {
        printf("%d is not a valid line index for deletion\n", deleteIndex);
        exit(EXIT_FAILURE);
    }

    // Write all lines to todo file, except the deleted line

    FILE *todoFileWrite = fopen(fullTodoFilePath, "w");

    for (int i = 0; i < linesCount; i++)
        if (i != deleteIndex && lines[i][0] != 0)
            fputs(lines[i], todoFileWrite);

    FreeTodoStructure(lines, linesCount);

    fclose(todoFileWrite);
}

void InsertTodo(int argc, char **argv, int insertFlagIndex)
{
    int insertIndex  = -1;
    int todoMsgIndex = insertFlagIndex + 1;

    if (GetFlagValWithDefault(argc, argv, insertFlagIndex, 0, &insertIndex))
        todoMsgIndex++; // The msg starts on the second index after the flag

    int    linesCount = 0;
    char **lines      = ReadTodo(&linesCount);

    // If insertIndex == linesCount, then append onto the end
    if (insertIndex < 0 || insertIndex > linesCount)
    {
        printf("%d is not a valid insertion index\n", insertIndex);
        return;
    }

    FILE *todoFile = fopen(fullTodoFilePath, "w");

    char *insertMsg =
        CombineArgvIntoMsg(argc, argv, todoMsgIndex, argc - todoMsgIndex);

    for (int i = 0; i < linesCount; i++)
    {
        if (i == insertIndex)
            fputs(insertMsg, todoFile);
        fputs(lines[i], todoFile);
    }

    if (insertIndex == linesCount)
        fputs(insertMsg, todoFile);

    fclose(todoFile);
    FreeTodoStructure(lines, linesCount);
    free(insertMsg);
}