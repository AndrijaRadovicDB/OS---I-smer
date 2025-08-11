// Program za sve regularne fajlove za koje ostali korisnici imaju rw pristup
// radi sledece:
// 1) ako su modifikovani u zadnjih 30 dana pomera ih u odgovarajuci direktorijum
// 2) inace se brisu
#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <time.h>
#include <sys/time.h>

#include <ftw.h>

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

void osMkPublicDir(const char* dname){

    static mode_t mode = 0777;
    check_error(mkdir(dname, mode) != -1, "mkdir");
    check_error(chmod(dname, mode) != -1, "chmod");
}

unsigned osNumOfDaysFileModified(const char* fPath){

    struct stat fInfo;
    check_error(stat(fPath, &fInfo) != -1, "Nepostojeci fajl");

    time_t now = time(NULL);
    time_t zadnja_izmena = now - fInfo.st_mtime;

    struct tm* brokenTime = localtime(&zadnja_izmena);
    check_error(brokenTime != NULL, "\n");

    return (unsigned)brokenTime->tm_mday;
}

void osMoveFile(const char *srcPath, const char *destPath) {

    check_error(strcmp(srcPath, destPath) != 0, "Isti fajl");

    struct stat fInfo;
    check_error(stat(srcPath, &fInfo) != -1, "Nepostojeci fajl");

    int fdSrc = open(srcPath, O_RDONLY);
    check_error(fdSrc != -1, "open1");
    int fdDest = open(destPath, O_WRONLY | O_CREAT | O_TRUNC);
    check_error(fdDest != -1, "open2");

    int bytesRead = 0;
    char memBuff[1024];
    while((bytesRead = read(fdSrc, memBuff, 1024)) > 0){
      check_error(write(fdDest, memBuff, bytesRead) != -1, "write");
    }
    check_error(bytesRead != -1, "write");

    close(fdSrc);
    close(fdDest);

    check_error(unlink(srcPath) != -1, "unlink");
    check_error(chmod(destPath, fInfo.st_mode) != -1, "chmod");

}

char newDir[1024];

int proccesFile(const char* fPath, const struct stat* sb, int typeflag, struct FTW* ftwbuf){

    if(S_ISREG(sb->st_mode)){ // Proverava da li je regularan
        if(osIsPublicFile(fPath)){ // Provera da li ima date pristupe
            if(osNumOfDaysFileModified(fPath) < 30){ // Provera kada je menjan
                // Pravimo potrebnu putanju
                char *tmp = malloc(strlen(newDir) + strlen(fPath + ftwbuf->base) + 2);
                strcpy(tmp, newDir);
                strcat(tmp, "/"); // Dodavanje na nisku
                strcat(tmp, fPath + ftwbuf->base); // Isto i ovde
                osMoveFile(fPath, tmp);
                free(tmp);
            }
            else{ // Ako je vise od 30 dana brise se
                unlink(fPath);
            }
        }
    }

    return 0;
}

int main(int argc, char** argv){

    check_error(argc == 3, "Argumenti");

    struct stat dir1;
    check_error(stat(argv[1], &dir1) != -1, "Ne postoji");
    check_error(S_ISDIR(dir1.st_mode), "Nije dir");

    strcpy(newDir, argv[2]);
    osMkPublicDir(argv[2]);

    // Rekurzivni obilazak kroz direktorijum sa nftw
    check_error(nftw(argv[1], proccesFile, 50, 0) != -1, "nftw");

    exit(EXIT_SUCCESS);
}
