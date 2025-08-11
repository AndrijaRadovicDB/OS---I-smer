// Program cita fajl rec po rec. Ako se rec sastoji samo od velikih slova
// engleske abecede i nije zakljuca, program treba da je zakljuca i da zameni
// u tekstu sa onoliko # koliko je rec dugacka. Ispisati koliko je izmenjeno
// karaktera.
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

#define MAX_SIZE 255

int valid_word(char* word){

    for(int i = 0; word[i]; i++){
        //if(word[i] < 'A' || word[i] > 'Z'){
        if(isdigit(word[i]) || islower(word[i]) || !(isalpha(word[i]))){
            return 0;
        }
    }

    return 1;
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    int fd = open(argv[1], O_RDWR);
    check_error(fd != -1, "open");
    FILE* f = fdopen(fd, "r+");
    check_error(f != NULL, "fdopen");

    int changed_chars = 0;
    char word[MAX_SIZE];
    while(fscanf(f, "%s", word) == 1){
        if(valid_word(word)){
            struct flock fLock;
            fLock.l_type = F_WRLCK;
            fLock.l_whence = SEEK_SET;
            fLock.l_start = ftell(f);
            fLock.l_len = -strlen(word);

            for(int i = 0; word[i]; i++){
                word[i] = '#';
            }

            check_error(fcntl(fd, F_GETLK, &fLock) != -1, "fcntl");
            if(fLock.l_type == F_UNLCK){
                check_error(fseek(f, -strlen(word), SEEK_CUR) != -1, "fseek");
                fprintf(f, "%s", word);
                changed_chars += strlen(word);
                fLock.l_type = F_WRLCK;
                check_error(fcntl(fd, F_SETLK, &fLock) != -1, "fcntl");
            }
        }
    }
    check_error(feof(f), "feof");

    printf("%d\n", changed_chars);

    fclose(f);

    exit(EXIT_SUCCESS);
}
