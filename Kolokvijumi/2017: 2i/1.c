// Program vraca true ako ostali korisnici imaju rw pristupa nad regularnim
// fajlovima na putanji fpath
#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

bool osIsPublicFile(const char* fPath){

    struct stat fInfo;
    check_error(stat(fPath, &fInfo) != -1, "Nepostojeci fajl");
    check_error((fInfo.st_mode & S_IFMT) == S_IFREG, "Pogresan tip fajla");

    if(!(fInfo.st_mode & S_IROTH) || !(fInfo.st_mode & S_IWOTH)){
        return false;
    }

    return true;
}

int main(int argc, char** argv){

    check_error(argc == 2, "\n");

    if(osIsPublicFile(argv[1])){
        printf("true\n");
    }
    else{
        printf("false\n");
    }

    exit(EXIT_SUCCESS);
}
