/*
   Shell interpreter
   by Kirill Feyzullin AM 2-1
   25.03.2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ROW 16
#define SYMB 80
void execution(char *argv[]);
int pars_args(char args[ROW][SYMB]);

int main() {
    char args[ROW][SYMB];
    int  number;
    char *argv[ROW];
    while ((number = pars_args(args)) != 0){
        if (number == -1) {
            continue;
        }
        for (int i = 0; i < number; i++) {
            argv[i] = args[i]; 
        }
        argv[number] = NULL;    
        execution(argv);
    }
    return 0;
}

//Execute our programs by argv
void execution(char *argv[]) {
    pid_t child_pid = fork(); 
    if (!child_pid) { 
        if (execvp(argv[0], argv) == -1) {
            perror("execvp"); 
        }
        exit(0);
    } 
    pid_t pid = wait(NULL); 
    if (pid == -1) { 
        perror("wait");
        exit(0);
    }
}

//Read and parsing arguments
int pars_args(char args[ROW][SYMB]) {
    int c;
    int i = 0, j = 0, flag_in_word = 0;   
    printf(">> ");
    while ((c = getchar()) != EOF) {
        if ((i > ROW) || (j > (SYMB - 2))) {
            printf("Incorrect input. Goodbye\n");
            stdin = freopen(NULL, "r", stdin);
            return -1;
        }    
        if ((c == '\n') & (i == 0) & (flag_in_word == 0)) {
            return -1;
        }
        if (c == '\n') { 
            return i + 1;
        }
        if ((c == ' ') & (flag_in_word == 1)) {
            
            flag_in_word = 0;
            i++;
            j = 0;
            continue;
        }
        if (c != ' ') { 
            args[i][j++] = c;
            args[i][j] = '\0';
            flag_in_word = 1;
        }
    }
    return 0;  
}

