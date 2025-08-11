// Program pomera fajl sa jedne putanje na drugu
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define check_error(cond,msg)\
  do {\
    if (!(cond)) {\
      perror(msg);\
      exit(EXIT_FAILURE);\
    }\
  } while (0)

void osMoveFile(const char *srcPath, const char *destPath) {

    // Provera da li su isti
    check_error(strcmp(srcPath, destPath) != 0, "Isti fajl");

    // Dobijanje informacija o fajlu na putanji
    struct stat fInfo;
    check_error(stat(srcPath, &fInfo) != -1, "Nepostojeci fajl");

    // Otvaranje fajlova
    int fdSrc = open(srcPath, O_RDONLY);
    check_error(fdSrc != -1, "open1");
    int fdDest = open(destPath, O_WRONLY | O_CREAT | O_TRUNC);
    check_error(fdDest != -1, "open2");

    // Citanje sadrzaja fajla sa jedne putanje i upisivanje na drugu
    int bytesRead = 0;
    char memBuff[1024];
    while((bytesRead = read(fdSrc, memBuff, 1024)) > 0){
      check_error(write(fdDest, memBuff, bytesRead) != -1, "write");
    }
    check_error(bytesRead != -1, "write");

    // Zatvaranje fajl deskriptora
    close(fdSrc);
    close(fdDest);

    // Uklanjanje prvog fajla i menjanje dozvola drugog sa dozvolama prvog
    check_error(unlink(srcPath) != -1, "unlink");
    check_error(chmod(destPath, fInfo.st_mode) != -1, "chmod");

}

int main(int argc, char** argv) {

    check_error(3 == argc, "\n");

    osMoveFile(argv[1], argv[2]);

    exit(EXIT_SUCCESS);
}
