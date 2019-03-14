#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#define N 16
#define M 80







void read_str(char args[N][M], int *status, int *k){
    char c;
    int i = 0, j = 0, f = 0;   
    while ( (c = getchar()) != EOF){
    
        if ((c == '\n')&(i == 0)&(f == 0)){
            *k = -1;
            break;
        }
        if ( c == '\n'){
            *k = i;
            break;
        }
        
        if ((c == ' ')&(f == 1)){
            f--;
            i++;
            j = 0;
            continue;
        }
        
        if (c != ' '){
            args[i][j++] = c;
            f = 1;
        }
        
    }
    if (c == EOF)
        *status=0;
}



int main(){
    int status = 1;
    
    while (status){
        
        printf(">> ");
        char args[N][M] = {0};
        int  k;
        read_str(args,&status,&k);
        char *argv[17];
        if (k == -1){
            continue;
        }       
        for (int i = 0; i <= k; i++){
            argv[i] = args[i];
        }
        argv[k+1] = NULL;
        
        pid_t child_pid = fork();
        if (!child_pid){
            execvp(argv[0],argv);
            exit(1);
        } 
        int rv = wait(NULL);  
    }
    printf("\n");
    return 0;
}





