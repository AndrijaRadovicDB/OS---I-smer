// Program dobija broj sekunde od epohe i broj godina, ispisati 2 dana koja
// odgovoraju ovim brojevima u 2 stringa razdvojenim belinom
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
        printf("nedelja ");
    }
    else if(num == 1){
        printf("ponedeljak ");
    }
    else if(num == 2){
        printf("utorak ");
    }
    else if(num == 3){
        printf("sreda ");
    }
    else if(num == 4){
        printf("cetvrtak ");
    }
    else if(num == 5){
        printf("petak ");
    }
    else if(num == 6){
        printf("subota ");
    }
}

int main(int argc, char** argv){

    check_error(argc == 3, "Argumenti");

    time_t time = strtol(argv[1], NULL, 10);
    time_t add_years = strtol(argv[2], NULL, 10);

    struct tm* broken_time = localtime(&time);
    check_error(broken_time != NULL, "localtime");
    print_month(broken_time->tm_wday);

    // Da se ne bi bukvalno pomerili godinu dana
    broken_time->tm_year += add_years;
    time_t new_time = mktime(broken_time);
    check_error(new_time != -1, "mktime");

    struct tm* broken_time1 = localtime(&new_time);
    print_month(broken_time1->tm_wday);

    printf("\n");

    exit(EXIT_SUCCESS);
}
