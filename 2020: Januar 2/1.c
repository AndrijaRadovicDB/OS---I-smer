// Program ispisuje najblizu prestupnu godinu prosledjenom vremenu
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <time.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

bool is_leap_year(int year){

    if((year % 4 == 0 && year % 100 != 0) || (year % 100 == 0 && year % 400 == 0)){
        return true;
    }

    return false;
}

int last_leap(int year){

    while(!is_leap_year(year)){
        year = year - year % 4;
    }

    return year;
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    time_t time = atoi(argv[1]);
    struct tm* broken_time = localtime(&time);
    check_error(broken_time != NULL, "localtime");

    printf("%d\n", last_leap(broken_time->tm_year + 1900));

    exit(EXIT_SUCCESS);
}
