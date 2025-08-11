// Program vraca pre koliko je dana zadnji put sadrzaj fajla promenjen u odnosu
// na dan kad se program pokrece
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <sys/time.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

unsigned osNumOfDaysFileModified(const char* fPath){

    struct stat fInfo;
    check_error(stat(fPath, &fInfo) != -1, "Nepostojeci fajl");

    time_t now = time(NULL);
    time_t zadnja_izmena = now - fInfo.st_mtime;

    struct tm* brokenTime = localtime(&zadnja_izmena);
    check_error(brokenTime != NULL, "\n");

    return (unsigned)brokenTime->tm_mday;
}

int main(int argc, char** argv){

    check_error(argc == 2, "\n");

    printf("%u\n", osNumOfDaysFileModified(argv[1]));

    exit(EXIT_SUCCESS);
}
