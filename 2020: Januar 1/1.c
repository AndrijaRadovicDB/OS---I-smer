// Program ispisuje koliko je proslo sati od pocetka tekuce nedelje
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

int hours(int num){

    if(num == 0){
        return 6*24;
    }
    else{
        return (num - 1)*24;
    }
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    time_t time = atoi(argv[1]);
    struct tm* broken_time = localtime(&time);
    check_error(broken_time != NULL, "localtime");

    printf("%d\n", hours(broken_time->tm_wday) + broken_time->tm_hour);

    exit(EXIT_SUCCESS);
}
