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
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define ROW 16
#define SYMB 80
#define FILES 2
void execution(char *argv[], char file[FILES][SYMB]);
void binding(char args[ROW][SYMB], char *argv[], int number);
int parc_args(char args[ROW][SYMB], char file[FILES][SYMB]);
int flow_read(char file[FILES][SYMB], int c);
void flow_change(int fd0, int fd1);
void invitation(void);

int main() {
    char args[ROW][SYMB], file[FILES][SYMB];
    int  number;
    char *argv[ROW + 1];
    while ((number = parc_args(args, file)) != -1) {
        if (number == 0) {
            continue;
        }
        binding(args, argv, number);
        execution(argv, file);
    }
    return 0;
}

//Binding lines with pointers
void binding(char args[ROW][SYMB], char *argv[], int number) {
    for (int i = 0; i < number; i++) {
            argv[i] = args[i]; 
    }
    argv[number] = NULL;  
}

//Execute our programs by argv
void execution(char *argv[],  char file[FILES][SYMB]) {
    int fd0, fd1;
    fd0 = fd1 = 0;
    int fl_len1 = strlen(file[0]), fl_len2 = strlen(file[1]);
    pid_t child_pid = fork(); 
    if (!child_pid) {
        if (fl_len1 || fl_len2) {
            char *p_filename;
            if (fl_len2) { 
                p_filename = file[1];
                fd1 = open(p_filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            }
            if (fl_len1) {
                p_filename = file[0];
                fd0 = open(p_filename, O_RDONLY, 0666);
            }
            if ((fd0 == -1) || (fd1 == -1)) {
                perror("open");
                exit(0);
            }
            flow_change(fd0,fd1);
        }
        if (execvp(argv[0], argv) == -1) {
           perror("execvp");
            exit(0); 
        }    
        exit(0);
    } 
    if (fd0) {
        close(fd0);
    }
    if (fd1) {
        close(fd1);
    }
    pid_t pid = wait(NULL); 
    if (pid == -1) { 
        perror("wait");
        exit(0);
    }
}

//Read and parcing arguments
int parc_args(char args[ROW][SYMB], char file[FILES][SYMB]) {
    int c;
    int i = 0, j = 0, k;
    file[0][0] = '\0';
    file[1][0] = '\0';
    invitation();   
    printf("$$ ");
    while ((c = getchar()) != EOF) {
        if ((c == '<') || (c == '>')) {
            if (j) {
                args[i][j] = '\0';
            } 
            if ((k = flow_read(file, c)) == 1) {
                if (!j) {
                    i--;
                }
                return i + 1;
            }
            if (k == -1) {
                return -1;
            }
            continue;
        }      
        if ((i > (ROW - 1)) || (j > (SYMB - 2))) {
            printf("Incorrect input. Please, try again\n");
            while((c = getchar()) != '\n');  //Clearing input buffer
            return -1;
        }    
        if ((c == '\n') && (!i) && (!j)) {
            return 0;
        }
        if (c == '\n') { 
            args[i][j] = '\0';
            if ( j == 0) {
                i--;
            } 
            return i + 1;
        }
        if ((c == ' ') && (j)) {
            args[i++][j] = '\0';
            j = 0;
            continue;
        }
        if (c != ' ') { 
            args[i][j++] = c;
        }
    }
    return -1;  
}

//Reading file(s) for flow(s) change
int flow_read( char file[FILES][SYMB], int c) {
    int   j = 0;  
    if (c == '<') { 
        while(((c = getchar()) != '>') && (c != EOF)) {
            if (c == '\n') { 
                if (!j) {
                    file[0][j] = '\0';
                }
                return 1;
            }
            if ((c == ' ') && (j)) {
                break;
            }
            if (c != ' ') { 
                file[0][j++] = c;
            }
        }
        file[0][j] = '\0';
        j = 0;
        if (c == EOF) {
            return -1;
        }
        if (c != '>'){
            while((c = getchar()) != '>') {
                if (c == '\n') {
                    return 1;
                }
            }
        }
    }
    while((c = getchar()) != EOF) {
        if (c == '\n') { 
            if (j != 0) {
                file[1][j] = '\0';
            }
            return  1;
        }
        if ((c == ' ') && (j)) {
            file[1][j] = '\0';
            return 0;
        }
        if (c != ' ') { 
            file[1][j++] = c;
        }
    }
    return -1;
}

//IO change
void flow_change(int fd0, int fd1) {
    if(fd1) {
        if (-1 == dup2(fd1, STDOUT_FILENO)) {
            perror("dup2");
            exit(0);
        }
    }
    if (fd0) {
        if(-1 == dup2(fd0, STDIN_FILENO)) {
		    perror("dup2");
		    exit(0);
	    }
    }
}

void invitation(void) {
    pid_t pid = fork();
    if (!pid) {
        execlp("pwd","pwd",NULL);
        exit(0);
    }
    pid = wait(NULL);
    printf("\b");
}