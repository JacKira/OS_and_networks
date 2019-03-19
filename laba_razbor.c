#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#define N 16
#define M 80

//ПРОЦЕДУРА ВЫЗОВА ПРИЛОЖЕНИЙ ЛИНУКС С ПОМОЩЬЮ МАССИВА АРГУМЕНТОВ
void execution(char args[N][M], int number) {
    char *argv[number+1];  //ОБЬЯВИЛИ МАССИВ УКАЗАТЕЛЕЙ РАЗМЕРА number +1
   //ПРИСВАИВАЕМ КАЖДОМУ УКАЗАТЕЛЮ АДРЕСС СТРОКИ
	 for (int i = 0; i <= number; i++) {
        argv[i] = args[i]; 
    }
    argv[number+1] = NULL; //И ОБЯВЛЯЕМ НУЛЬ-ТЕРМИНАННД В КОНЦЕ
    pid_t child_pid = fork(); //СОЗДАЕМ ДОЧЕРНИЙ ПРОЦЕСС  
    if (!child_pid){ //ЕСЛИ ЭТО ДОЧЕРНИЙ ПРОЦЕСС, ВЫПОЛНЯЕМ
        if (execvp(argv[0],argv) == -1) { //ИСПОЛНЯЕМ И ОДНОВРЕМЕННО ПРОВЕРЯЕМ НА ОШИБКУ
            perror("execvp"); 
            exit(1);
        }
        exit(0); 
    } 
    int pid = wait(NULL); //ПРОВЕРЯЕМ ДОЧЕРНИЙ ПРОЦЕСС, ЗАВЕРАШЕМ ЕГО, ЕСЛИ ОН ВЫПОЛНИЛ КОД
    if (pid == -1) { 
        perror("wait");
        exit(1);
    }
}

//ПРОЦЕДУРА ЧТЕНИЯ СТРОКИ И ПАРСИРОВАНИЯ ЕЕ НА АРГУМЕНТЫ
void read_str(char args[N][M], int *status, int *number) {
    int c;
    int i = 0, j = 0, flag = 0;   
    while ( (c = getchar()) != EOF) { //ПОКА СЧИТАННЫЙ СИМВОЛ НЕ БУДЕТ КОНЦОМ ФАЙЛА, ВЫПОЛНЯЕМ
        if ((i > (N - 2)) || (j > (M - 2))){   //ПРОВЕРЯЕМ НА ПЕРЕПОЛНЯЕМОСТЬ МАССИВА
            printf("Incorrect input. PLease, try again\n");
            stdin = freopen(NULL,"r",stdin); //ОЧИЩАЕМ БУФФЕР ВВОДА
            *number = -1; //ПЕРЕДАЕМ СПЕЦИАЛЬНОЕ ЗНАЧЕНИЕ И ВЫХОДИМ
            return;
       }    
         
        if ((c == 10)&(i == 0)&(flag == 0)) { //ЕСЛИ ВВЕЛИ ПУСТОЙ ENTER, ПЕРЕДАЕМ СПЕЦИАЛЬНОЕ ЗНАЧЕНИЕ И ВЫХОДИМ
            *number = -1;
            return;
        }
        if ( c == 10) { //ЕСЛИ ВВЕЛИ ENTER В КОНЦЕ СТРОКИ, ПЕРЕДАЕМ КОЛИЧЕСТВО ВВЕДЕННЫХ СЛОВ И ВЫХОДИМ
            *number = i;
            return;
        }
        if ((c == 32)&(flag == 1)) { //ЕСЛИ СИМВОЛ ПРОБЕЛЬНЫЙ И ФЛАГ СЛОВА ПОДНЯТ- ЗНАЧИТ ЗАКОНЧИЛИ ВВОД ДАННОГО СЛОВА
            flag--;
            i++;
            j = 0;
            continue;
        }
        if (c != 32) { //ЕСЛИ НЕ ПРОБЕЛЬНЫЙ СИМВОЛ, ТО ЗАПИСЫВАЕМ В СТРОКУ
            args[i][j++] = c;
            flag = 1;
        }
    }  
    *status = 0; //ЕСЛИ КОНЕЦ ФАЙЛА, ТО ИЗМЕНЯЕМ ЗНАЧЕНИЕ СТАТУСА БЕСКОНЕЧНОГО ЦИКЛА
    *number = i;
	if ((i == 0)&(j == 0)){
    	*number = -1;
    }
    
}



int main() {
    int status = 1;
    while (status) {
        printf(">> ");
        char args[N][M] = {0}; //ОБЪЯВЛЯЕМ МАССИВ, ЗАПОЛНЕННЫЙ НУЛЯМИ
        int  number = 0;
        read_str(args,&status,&number); // ПРОЦЕДУРА СЧИТЫВАНИЯ СТРОКИ
        if (number == -1) {
            continue; //ЕСЛИ ПОЛУЧЕНО СПЕЦИАЛЬНОЕ ЗНАЧЕНИЕ, ПРОПУСКАЕМ ИТЕРАЦИЮ
        }       
        execution(args,number); //ПРОЦЕДУРА ДЛЯ ВЫПОЛНЕНИЯ СИСТЕМНЫХ КОМАНД
    }
    printf("\n");
    return 0;
}
