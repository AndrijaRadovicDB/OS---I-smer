// Program uklanja sadrzaj fajl od pozicije a do pozivije b i ispisuje na stdout
// uklonjeni sadrzaj kao i koliko karaktera je uklonjeno. Fajl nece biti veci od
// 8192 bajta.
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

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

#define MAX_SIZE 8192

int main(int argc, char** argv){

    check_error(argc == 4, "Argumenti");

    int fd = open(argv[1], O_RDWR);
    check_error(fd != -1, "open");
    int a = atoi(argv[2]);
    int b = atoi(argv[3]);
    check_error(a <= b, "a mora biti manje od b");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "stat");
    check_error(S_ISDIR(fInfo.st_mode), "Nije regularan");

    int removed = b - a + 1;
    char buffer[MAX_SIZE];
    check_error(lseek(fd, a, SEEK_SET) != -1, "lseek");
    check_error(read(fd, buffer, removed) != -1, "read");
    printf("%s\n%d\n", buffer, removed);

    int after_b = fInfo.st_size - b;
    check_error(lseek(fd, b + 1, SEEK_SET) != -1, "lseek");
    check_error(read(fd, buffer, after_b - 1) != -1, "read");

    int new_size = fInfo.st_size - removed;
    check_error(ftruncate(fd, a) != -1, "ftruncate");
    check_error(ftruncate(fd, new_size - 1) != -1, "ftruncate");
    check_error(lseek(fd, a, SEEK_SET) != -1, "lseek");
    check_error(write(fd, buffer, after_b - 1) != -1, "write");

    close(fd);

    exit(EXIT_SUCCESS);
}
