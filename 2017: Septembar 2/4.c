// Program dobija broj sekunde od epohe i ispisuje naziv meseca na srpskom
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <time.h>

#include <stdio.h>
#include <stdlib.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

void printf_month(int num){

    if(num == 0){
        printf("januar\n");
    }
    else if(num == 1){
        printf("februar\n");
    }
    else if(num == 2){
        printf("mart\n");
    }
    else if(num == 3){
        printf("april\n");
    }
    else if(num == 4){
        printf("maj\n");
    }
    else if(num == 5){
        printf("jun\n");
    }
    else if(num == 6){
        printf("jul\n");
    }
    else if(num == 7){
        printf("avgust\n");
    }
    else if(num == 8){
        printf("septembar\n");
    }
    else if(num == 9){
        printf("oktobar\n");
    }
    else if(num == 10){
        printf("novembar\n");
    }
    else if(num == 11){
        printf("decembar\n");
    }
    else{
        check_error(0, "Nevazeci mesec\n");
    }
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    time_t now = strtol(argv[1], NULL, 10);
    check_error(now != -1, "time");
    struct tm* broken_time = localtime(&now);
    check_error(broken_time != NULL, "localtime");

    printf_month(broken_time->tm_mon);

    exit(EXIT_SUCCESS);
}
