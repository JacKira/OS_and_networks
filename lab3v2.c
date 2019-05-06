/*
   Shell interpreter 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ROW 16
#define SYMB 80
#define FILES 2
void execution(char *argv[ROW + 1][ROW + 1], char *p_stream[], int flag_pipe);
int binding(char args[ROW][SYMB], char *argv[ROW + 1][ROW + 1], int number, char *p_stream[]);
int parc_args(char args[ROW][SYMB]);
void pipe_0(int arr_fd[]);
void pipe_1(int arr_fd[]);
void open_in_file(char *p);
void open_out_file(char *p);

int main()
{
    char args[ROW][SYMB];
    int number, pipes_count;
    char *argv[ROW + 1][ROW + 1];
    char *p_stream[FILES];
    while ((number = parc_args(args)) != -1)
    {
        if (number == 0)
        {
            continue;
        }
        p_stream[0] = NULL;
        p_stream[1] = NULL;
        pipes_count = binding(args, argv, number, p_stream);
        execution(argv, p_stream, pipes_count);
    }
    return 0;
}

//Binding lines with pointers
int binding(char args[ROW][SYMB], char *argv[ROW + 1][ROW + 1], int number, char *p_stream[])
{
    int k = 0, j = 0;
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
        if (args[i][0] == '|')
        {
            argv[j++][k] = NULL;
            k = 0;
            argv[j][k++] = &args[i][1];
            continue;
        }
        argv[j][k++] = args[i];
    }
    argv[j][k] = NULL;
    argv[j + 1][0] = NULL;
    return j;
}

//Program call with input and output stream redirection
void execution(char *argv[ROW + 1][ROW + 1], char *p_stream[], int pipes_count)
{
    int k = pipes_count + 1;
    int arr_fd[k][2];
    for (int i = 0; i < k; i++)
    {
        if (-1 == pipe(arr_fd[i]))
        {
            perror("pipe");
            return;
        }
    }
    for (int j = 0; j < k; j++)
    {
        pid_t child_pid = fork();
        if (!child_pid)
        {
            if (j && pipes_count)
            {
                pipe_0(arr_fd[j]); //pipe_in
            }
            if (p_stream[0] && !j)
            {
                open_in_file(p_stream[0]);
            }
            if (argv[j + 1][0])
            {
                pipe_1(arr_fd[j + 1]); //pipe_out
            }
            if (p_stream[1] && !argv[j + 1][0])
            {
                open_out_file(p_stream[1]);
            }
            if (execvp(argv[j][0], argv[j]) == -1)
            {
                perror("execvp");
                exit(1);
            }
            exit(0);
        }
        close(arr_fd[j][0]);
        close(arr_fd[j][1]);
    }
    for(int i = 0; i < k; i++)
    {
       pid_t pid = wait(NULL);
        if (pid == -1)
        {
            perror("wait");
            exit(1);
        }
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
        if ((c == '<') || (c == '>') || (c == '|'))
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

//Opening pipe to enter
void pipe_0(int arr_fd[])
{
    close(arr_fd[1]);
    if (-1 == dup2(arr_fd[0], STDIN_FILENO))
    {
        perror("dup2_in");
        exit(1);
    }
}

//Opening the pipe to the output
void pipe_1(int arr_fd[])
{
    close(arr_fd[0]);
    if (-1 == dup2(arr_fd[1], STDOUT_FILENO))
    {
        perror("dup2_out");
        exit(1);
    }
}

//Redirecting the input stream from a file
void open_in_file(char *p)
{
    int fd0 = open(p, O_RDONLY, 0666);
    if (fd0 == -1)
    {
        perror("open");
        exit(1);
    }
    if (-1 == dup2(fd0, STDIN_FILENO))
    {
        perror("dup2");
        exit(1);
    }
}

//Redirecting output stream to file
void open_out_file(char *p)
{
    int fd1 = open(p, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd1 == -1)
    {
        perror("open");
        exit(1);
    }
    if (-1 == dup2(fd1, STDOUT_FILENO))
    {
        perror("dup2");
        exit(1);
    }
}
