// Program ispisuje apsolutnu razliku u danima izmedju vremena modifikacije fajlova
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <time.h>
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

int main(int argc, char** argv){

    check_error(argc == 3, "Argumenti");

    struct stat fInfo1;
    check_error(stat(argv[1], &fInfo1) != -1, "Nepostojeci fajl");
    struct stat fInfo2;
    check_error(stat(argv[2], &fInfo2) != -1, "Nepostojeci fajl");

    time_t vreme1 = fInfo1.st_mtime;
    time_t vreme2 = fInfo2.st_mtime;
    time_t razlika = (abs(vreme1 - vreme2)+24*60*60-1)/(24*60*60);
    printf("%d\n", (int)razlika);

    exit(EXIT_SUCCESS);
}
