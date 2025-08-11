// Program ispisuje dan u nedelji za prosledjeno vreme i vreme sa dodatim god
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

void day_of_week(int num){

    if(num == 0){
        printf("nedelja ");
    }
    else if(num == 1){
        printf("pondeljak ");
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

    time_t time = atoi(argv[1]);
    int add_years = atoi(argv[2]);

    struct tm* broken_time = localtime(&time);
    check_error(broken_time != NULL, "localtime");
    day_of_week(broken_time->tm_wday);

    broken_time->tm_year += add_years;
    time_t new_time = mktime(broken_time);
    check_error(new_time != -1, "mktime");
    day_of_week(broken_time->tm_wday);
    printf("\n");

    /* mktime azurira prosledjeno vreme
        broken_time = localtime(&new_time);
        check_error(broken_time != NULL, "localtime");
        day_of_week(broken_time->tm_wday);
    */

    exit(EXIT_SUCCESS);
}
