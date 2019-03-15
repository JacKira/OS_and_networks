#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#define N 16
#define M 80


void execution(char args[N][M], int number){

    char *argv[number+1];
    for (int i = 0; i <= number; i++){
        argv[i] = args[i];
    }
    argv[number+1] = NULL;
       
    pid_t child_pid = fork();
    if (!child_pid){
        if (execvp(argv[0],argv) == -1){
            perror("execvp");
            exit(1);
        }
        exit(0);
    } 
    
    int pid = wait(NULL);     
    if (pid == -1){
        perror("wait");
        exit(1);
    }
}


void read_str(char args[N][M], int *status, int *number){
    char c;
    int i = 0, j = 0, flag = 0;   
    while ( (c = getchar()) != EOF){
    
        if ((c == '\n')&(i == 0)&(flag == 0)){
            *number = -1;
            break;
        }
        
        if ( c == '\n'){
            *number = i;
            break;
        }
        
        if ((c == ' ')&(flag == 1)){
            flag--;
            i++;
            j = 0;
            continue;
        }
        
        if (c != ' '){
            args[i][j++] = c;
            flag = 1;
        }
        
    }
    
    if (c == EOF){
        *status=0;
        if ((i == 0)&(j == 0)){
            *number = -1;
        }
    }
}



int main(){
    int status = 1;
    
    while (status){
        
        printf(">> ");
        char args[N][M] = {0};
        int  number;
        read_str(args,&status,&number);
      
        if (number == -1){
            continue;
        }       
        execution(args,number);
              
        
       
    }
    printf("\n");
    return 0;
}





