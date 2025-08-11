// Program prima broj sekundi od epohe, treba da se ispise kom dobu dana odgovara
// prosledjeno vreme 0-7 nocm 7-9 jutro, 9-12 prepodne, 12-19 popodne, 19-0 vece
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

    if((broken_time->tm_hour >= 0 && broken_time->tm_hour < 7) || (broken_time->tm_hour == 7 && broken_time->tm_min == 0)){
        printf("noc\n");
    }
    else if((broken_time->tm_hour >= 7 && broken_time->tm_hour < 9) || (broken_time->tm_hour == 9 && broken_time->tm_min == 0)){
        printf("jutro\n");
    }
    else if((broken_time->tm_hour >= 9 && broken_time->tm_hour < 12) || (broken_time->tm_hour == 12 && broken_time->tm_min == 0)){
        printf("prepodne\n");
    }
    else if((broken_time->tm_hour >= 12 && broken_time->tm_hour < 19) || (broken_time->tm_hour == 19 && broken_time->tm_min == 0)){
        printf("popodne\n");
    }
    else if((broken_time->tm_hour >= 19 && broken_time->tm_hour <= 23) || (broken_time->tm_hour == 0 && broken_time->tm_min == 0)){
        printf("vece\n");
    }

    exit(EXIT_SUCCESS);
}
