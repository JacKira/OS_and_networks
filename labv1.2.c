/*
   Shell interpreter 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ROW 16
#define SYMB 80
#define FILES 2
void execution(char *argv[], char *p_stream[]);
void binding(char args[ROW][SYMB], char *argv[], int number, char *p_stream[]);
int parc_args(char args[ROW][SYMB]);

int main()
{
    char args[ROW][SYMB];
    int number;
    char *argv[ROW + 1];
    char *p_stream[FILES];
    while ((number = parc_args(args)) != -1)
    {
        if (number == 0)
        {
            continue;
        }
        p_stream[0] = NULL;
        p_stream[1] = NULL;
        binding(args, argv, number, p_stream);
        execution(argv, p_stream);
    }
    return 0;
}

//Binding lines with pointers
void binding(char args[ROW][SYMB], char *argv[], int number, char *p_stream[])
{
    int k = 0;
    for (int i = 0; i < number; i++)
    {
        if (args[i][0] == '>')
        {
            p_stream[1] = &args[i][1];
            continue;
        }
        if (args[i][0] == '<')
        {
            p_stream[0] = &args[i][1];
            continue;
        }
        argv[k++] = args[i];
    }
    argv[k] = NULL;
}

//Execute our programs by argv
void execution(char *argv[], char *p_stream[])
{
    pid_t child_pid = fork();
    if (!child_pid)
    {
        if (p_stream[1])
        {
            int fd1 = open(p_stream[1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (fd1 == -1)
            {
                perror("open");
                exit(1);
            }
            if (-1 == dup2(fd1, STDOUT_FILENO))
            {
                perror("dup2");
                exit(0);
            }
        }
        if (p_stream[0])
        {
            int fd0 = open(p_stream[0], O_RDONLY, 0666);
            if (fd0 == -1)
            {
                perror("open");
                exit(1);
            }
            if (-1 == dup2(fd0, STDIN_FILENO))
            {
                perror("dup2");
                exit(0);
            }
        }
        if (execvp(argv[0], argv) == -1)
        {
            perror("execvp");
            exit(1);
        }
        exit(0);
    }
    pid_t pid = wait(NULL);
    if (pid == -1)
    {
        perror("wait");
        exit(1);
    }
}

//Read and parcing arguments
int parc_args(char args[ROW][SYMB])
{
    int c;
    int i = 0, j = 0, fl_flag = 0;
    printf("$$ ");
    while ((c = getchar()) != EOF)
    {
        if ((c == '<') || (c == '>'))
        {
            if (j)
            {
                args[i++][j] = '\0';
                j = 0;
            }
            args[i][j++] = c;
            fl_flag = 1;
            continue;
        }
        if ((i > (ROW - 1)) || (j > (SYMB - 2)))
        {
            printf("Incorrect input. Please, try again\n");
            while ((c = getchar()) != '\n')
                ; //Clearing input buffer
            return -1;
        }
        if ((c == '\n') && !i && !j)
        {
            return 0;
        }
        if (c == '\n')
        {
            args[i][j] = '\0';
            if (j == 0)
            {
                i--;
            }
            return i + 1;
        }
        if ((c == ' ') && j && !fl_flag)
        {
            args[i++][j] = '\0';
            j = 0;
            continue;
        }
        if (c != ' ')
        {
            args[i][j++] = c;
            fl_flag = 0;
        }
    }
    return -1;
}
