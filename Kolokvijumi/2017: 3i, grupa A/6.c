// Prograam pomera sve regularne fajlove i direktorijume, ako su putanje iste
// izaci iz programa sa exit code-om 1
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <dirent.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0);

void osMoveFile(const char* srcPath, const char* destPath){

    // Sa realpath proveravamo da li putanja uopste postoji
    char* sPath = realpath(srcPath, NULL);
    check_error(sPath != NULL, "Putanja ne postoji");
    char* dPath = realpath(destPath, NULL);

    // Provera da li su iste
    if(sPath != NULL && dPath != NULL){
        check_error(strcmp(sPath, dPath), "Putanje su iste");
    }

    free(sPath);
    free(dPath);

    // Rename pomera sa jedne putanje na drugu ako je potrebno
    check_error(rename(srcPath, destPath) != -1, "rename");

}

int main(int argc, char** argv){

    check_error(argc == 3, "Argumenti");

    osMoveFile(argv[1], argv[2]);

    exit(EXIT_SUCCESS);
}
