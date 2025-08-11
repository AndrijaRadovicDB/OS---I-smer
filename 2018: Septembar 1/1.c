// Program dobija broj sekundi i broj minuta za koje treba da pomeri to vreme i
// ispisati ga u formatu DD/MM/YYYY HH:MM
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

int main(int argc, char** argv){

    check_error(argc == 3, "Argumenti");

    time_t time = strtol(argv[1], NULL, 10);
    int add_time = strtol(argv[2], NULL, 10);
    time += 60*add_time;
    struct tm* broken_time = localtime(&time);
    check_error(broken_time != NULL, "localtime");

    char time_string[18];
    strftime(time_string, 17, "%d/%m/%Y %H:%M", broken_time);
    printf("%s\n", time_string);


    exit(EXIT_SUCCESS);
}
