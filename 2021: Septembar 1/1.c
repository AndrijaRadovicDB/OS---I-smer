// Program ispisuje mesec i mesec pomeren za dat broj dana
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

void print_month(int num){

    if(num == 0){
        printf("januar ");
    }
    else if(num == 1){
        printf("februar ");
    }
    else if(num == 2){
        printf("mart ");
    }
    else if(num == 3){
        printf("april ");
    }
    else if(num == 4){
        printf("maj ");
    }
    else if(num == 5){
        printf("jun ");
    }
    else if(num == 6){
        printf("jul ");
    }
    else if(num == 7){
        printf("avgust ");
    }
    else if(num == 8){
        printf("septembar ");
    }
    else if(num == 9){
        printf("oktobar ");
    }
    else if(num == 10){
        printf("novembar ");
    }
    else if(num == 11){
        printf("decembar ");
    }
}

int main(int argc, char** argv){

    check_error(argc == 3, "Argumenti");

    time_t time = atoi(argv[1]);
    int add_days = atoi(argv[2]);

    struct tm* broken_time = localtime(&time);
    check_error(broken_time != NULL, "localtime");
    print_month(broken_time->tm_mon);

    broken_time->tm_mday += add_days;
    check_error(mktime(broken_time) != -1, "mktime");
    print_month(broken_time->tm_mon);
    printf("\n");

    exit(EXIT_SUCCESS);
}
