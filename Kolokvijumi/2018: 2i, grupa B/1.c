// Program ispisuje razliku vremena zadnje pristupa i menjanja sadrzaja fajla,
// vrednost zaokruziti na veci broj u satima
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

    check_error(argc == 2, "Argumenti");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "Nepostojeci fajl");

    time_t razlika = abs(fInfo.st_atime - fInfo.st_mtime);
    printf("%u\n", (unsigned)ceil(razlika/3600.0));

    exit(EXIT_SUCCESS);
}
