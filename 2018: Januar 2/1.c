// Program broj sekundi od epohe ispisuje u formatu hh:mm
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

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    time_t time = strtol(argv[1], NULL, 10);
    struct tm* broken_time = localtime(&time);
    check_error(broken_time != NULL, "localtime");

    int hour = broken_time->tm_hour;
    int min = broken_time->tm_min;

    if(hour < 10 && min >= 10){
        printf("0%d:%d\n", hour, min);
    }
    else if(hour >= 10 && min < 10){
        printf("%d:0%d\n", hour, min);
    }
    else if(hour < 10 && min < 10){
        printf("0%d:0%d\n", hour, min);
    }
    else{
        printf("%d:%d\n", hour, min);
    }

    exit(EXIT_SUCCESS);
}
