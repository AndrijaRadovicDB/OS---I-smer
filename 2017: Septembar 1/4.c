// Program dobija broj sekundi od epohe i ispisuje naziv dana u nedelji za
// prosledjeni broj
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <time.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

void day_of_the_week(int num){

    if(num == 0){
        printf("nedelja\n");
    }
    else if(num == 1){
        printf("ponedeljak\n");
    }
    else if(num == 2){
        printf("utorak\n");
    }
    else if(num == 3){
        printf("sreda\n");
    }
    else if(num == 4){
        printf("cetvrtak\n");
    }
    else if(num == 5){
        printf("petak\n");
    }
    else if(num == 6){
        printf("subota\n");
    }

}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    time_t time = strtol(argv[1], NULL, 10);
    struct tm* broken_time = localtime(&time);
    check_error(broken_time != NULL, "localtime");

    day_of_the_week(broken_time->tm_wday);

    exit(EXIT_SUCCESS);
}
