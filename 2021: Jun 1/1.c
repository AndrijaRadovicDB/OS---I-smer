// Program za dati broj sekundi ispisuje koliko je proslo sati od tekuceg meseca
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

    time_t time = atoi(argv[1]);
    struct tm* broken_time = localtime(&time);
    check_error(broken_time != NULL, "localtime");

    // Za testiranje
    // char string[256];
    // strftime(string, 256, "%d/%m/%y %H:%M", broken_time);
    //printf("%s\n", string);

    // printf("%d\n", broken_time->tm_mday*24 + broken_time->tm_hour);
    // Oni su ili dali los test primer ili pogresno napisali tekst vrv treba
    // Jer je proslo n - 1 dan od pocetka meseca + vreme u toku danasnjeg dana
    printf("%d\n", (broken_time->tm_mday - 1)*24 + broken_time->tm_hour);


    exit(EXIT_SUCCESS);
}
