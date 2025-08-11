// Program uklanja sadrzaj fajla od pozicije a do pozicije b i taj sadrza, kao i
// broj uklonjenih karaktera se ispisuju na standardni izlaz.
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
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

#define MAX_SIZE 256

int main(int argc, char** argv){

    check_error(argc == 4, "Argumenti");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "stat");
    check_error(S_ISREG(fInfo.st_mode), "Nije reg");

    int fd = open(argv[1], O_RDWR);
    check_error(fd != -1, "open");

    int a = atoi(argv[2]);
    int b = atoi(argv[3]);
    check_error(a < b, "a mora biti manje od b");

    int removed = b - a + 1;
    char* buffer = malloc((removed + 1)*sizeof(char));
    check_error(lseek(fd, a, SEEK_SET) != -1, "lseek");
    check_error(read(fd, buffer, removed) != -1, "read");
    printf("%s\n%d\n", buffer, removed);

    int after_b = fInfo.st_size - b;
    buffer = realloc(buffer, (after_b)*sizeof(char));
    check_error(buffer != NULL, "realloc");

    // Moglo je i sa memmove, koji bi pomerio ceo sadrzaj
    // memmove(buffer + a, buffer + b + 1, fInfo.st_size - b);

    check_error(read(fd, buffer, after_b) != -1, "read");
    check_error(ftruncate(fd, a) != -1, "ftruncate");
    check_error(ftruncate(fd, a + after_b - 1) != -1, "ftruncate");
    check_error(lseek(fd, a, SEEK_SET) != -1, "lseek");
    check_error(write(fd, buffer, after_b - 1) != -1, "write");

    free(buffer);
    close(fd);

    exit(EXIT_SUCCESS);
}
