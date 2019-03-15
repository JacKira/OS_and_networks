#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#define N 16
#define M 80

//ПРОЦЕДУРА ВЫЗОВА ПРИЛОЖЕНИЙ ЛИНУКС С ПОМОЩЬЮ МАССИВА АРГУМЕНТОВ
void execution(char args[N][M], int number) {
    char *argv[number+1]; 
    for (int i = 0; i <= number; i++) {
        argv[i] = args[i]; 
    }
    argv[number+1] = NULL; 
    pid_t child_pid = fork(); //СОЗДАЕМ ДОЧЕРНИЙ ПРОЦЕСС
    if (!child_pid){ //ЕСЛИ ЭТО ДОЧЕРНИЙ ПРОЦЕСС, ВЫПОЛНЯЕМ
        if (execvp(argv[0],argv) == -1) {
            perror("execvp"); 
            exit(1);
        }
        exit(0); 
    } 
    int pid = wait(NULL); 
    if (pid == -1) { 
        perror("wait");
        exit(1);
    }
}

//ПРОЦЕДУРА ЧТЕНИЯ СТРОКИ И ПАРСИРОВАНИЯ ЕЕ НА АРГУМЕНТЫ
void read_str(char args[N][M], int *status, int *number) {
    int c;
    int i = 0, j = 0, flag = 0;   
    while ( (c = getchar()) != EOF) {
        if ((i > (N - 2)) || (j > (M - 2))){
            printf("Incorrect input. PLease, try again\n");
            stdin = freopen(NULL,"r",stdin);
            *number = -1;
            break;
       }    
         
        if ((c == 10)&(i == 0)&(flag == 0)) {
            *number = -1;
            return;
        }
        if ( c == 10) { 
            *number = i;
            return;
        }
        if ((c == 32)&(flag == 1)) {
            flag--;
            i++;
            j = 0;
            continue;
        }
        if (c != 32) { 
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



int main() {
    int status = 1;
    while (status) {
        printf(">> ");
        char args[N][M] = {0};
        int  number;
        read_str(args,&status,&number);
        if (number == -1) {
            continue;
        }       
        execution(args,number);
    }
    printf("\n");
    return 0;
}





