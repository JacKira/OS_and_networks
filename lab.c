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

#define ROW 16
#define SYMB 80
#define STDOUT_FILENO 1
#define STDIN_FILENO 0
#define FILES 2
void execution(char *argv[], int flow_flag, char file[FILES][SYMB]);
void binding(char args[ROW][SYMB], char *argv[], int number);
int parc_args(char args[ROW][SYMB], int *flow_code, char file[FILES][SYMB]);
int flow_read(char file[FILES][SYMB], int c);
void flow_change(char file[FILES][SYMB], int flow_code);

int main() {
    char args[ROW][SYMB], file[FILES][SYMB];
    int  number,  flow_code = 0;;
    char *argv[ROW];

    while ((number = parc_args(args,  &flow_code, file)) != 0) {
        if (number == -1) {
            continue;
        }
          
  /*      puts(args[0]);
        puts(args[1]);
        puts(file[0]); 
        puts(file[1]);
*/        binding(args, argv, number);
        execution(argv, flow_code, file);

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
void execution(char *argv[], int flow_code, char file[FILES][SYMB]) {
    pid_t child_pid = fork(); 
    if (!child_pid) {
        if (flow_code) {
            flow_change(file, flow_code);
        }
        if (execvp(argv[0], argv) == -1) {
           perror("execvp");
            exit(0); 
        }    

        exit(0);
    } 
    pid_t pid = wait(NULL); 
    if (pid == -1) { 
        perror("wait");
        exit(0);
    }
}


//Read and parcing arguments
int parc_args(char args[ROW][SYMB], int *flow_code, char file[FILES][SYMB]) {
    int c;
    int i = 0, j = 0, flag_in_word = 0;   
    printf(">> ");
    while ((c = getchar()) != EOF) {

        if ((c == '<') || (c == '>')) {
            args[i++][j] = '\0';
            j = 0;
            flag_in_word = 0;
            if ((*flow_code = flow_read( file, c)) == -1) {
                return -1;
            }
            if (((file[0][strlen(file[0])]) == '\n') || ((file[1][strlen(file[0])]) == '\n')) {
                return i;
            }
            continue;
        }      

        if ((i > (ROW - 1)) || (j > (SYMB - 2))) {
            printf("Incorrect input. Please, try again\n");
            while((c = getchar()) != '\n');  //Clearing input buffer
            return -1;
        }    
        if ((c == '\n') & (i == 0) & (flag_in_word == 0)) {
            return -1;
        }
        if (c == '\n') { 
            args[i][j] = '\0';

            if ( j == 0) {
                i -= 1;

            }
            return i+1;
        }
        if ((c == ' ') & (flag_in_word == 1)) {
            args[i++][j] = '\0';
            flag_in_word = 0;
            j = 0;
            continue;
        }
        if (c != ' ') { 
            args[i][j++] = c;
            flag_in_word = 1;
        }
    }
    return 0;  
}


int flow_read( char file[FILES][SYMB], int c) {
    int   j = 0, flag_in_word = 0, k = 0;  
    if (c == '<') { 
        while(((c = getchar()) != '>') && (c != EOF)) {
            if (c == '\n') { 
                file[0][j++] = '\0';
                file[0][j] = '\n';
                if ( j == 0) {
                }
                return  k + 1;
            }
            if ((c == ' ') & (flag_in_word == 1)) {
                file[0][j] = '\0';
                flag_in_word = 0;
                j = 0;
                break;
            }
            if (c != ' ') { 
                file[k][j++] = c;
                flag_in_word = 1;
            }
        }
        if (c == EOF) {
            return -1;
        }
        if (c != '>'){
            while((c = getchar()) != '>');
        }
    }
    while((c = getchar()) != EOF) {
        if (c == '\n') { 
            file[1][j++] = '\0';
            file[1][j] = '\n';
            if ( j == 0) {
            }
            return  k + 2;
        }
        if ((c == ' ') & (flag_in_word == 1)) {
            file[1][j] = '\0';
            return k + 2;
        }
        if (c != ' ') { 
            file[1][j++] = c;
            flag_in_word = 1;
        }
    }
    return -1;
}

void flow_change(char file[FILES][SYMB], int flow_code) {
    char *p_filename;
    if (flow_code >= 2) { 
        p_filename = file[1];
        int fl = open(p_filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fl == -1) {
            perror("open");
            exit(0);
        }
        if (-1 == dup2(fl, STDOUT_FILENO)) {
            perror("dup2");
            exit(0);
        }
        if (flow_code == 2) {
            return;
        }
    }
    if (flow_code >= 1) {
        p_filename = file[0];
        int fl = open(p_filename, O_RDONLY, 0666);
		if (fl == -1)
		{
			perror("open");
			exit(0);
		}
		if(-1 == dup2(fl,STDIN_FILENO))
		{
		    perror("dup2");
			exit(0);
		}
    }
}

