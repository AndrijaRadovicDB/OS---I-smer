// Program prima putanju do .c fajla i poziva gcc -std=c99 -S 1.c -o 1.s. Ako se
// gcc izvrsi neuspeno, program treba da se zavrsi sa tim exit-codeom. Na kraju
// treba da se obrise dati .s fajl. Program ispisuje broj mov komandi.
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/wait.h>

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

#define RD_END (0)
#define WR_END (1)

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    char* tmp = strrchr(argv[1], '.');
    check_error(tmp != NULL, "Los fajl");
    check_error(!strcmp(tmp, ".c"), "Losa ekstenzija");

    int pipeFds[2];
    check_error(pipe(pipeFds) != -1, "pipe");
    pid_t cldPid = fork();
    check_error(cldPid != -1, "fork");

    int len = strlen(argv[1]);
    char* as_name = malloc((len + 1)*sizeof(char));
    check_error(as_name != NULL, "malloc");
    strncpy(as_name, argv[1], len + 1);
    as_name[len - 1] = 's';

    if(cldPid == 0){
        close(pipeFds[RD_END]);

        check_error(dup2(pipeFds[WR_END], STDOUT_FILENO) != -1, "dup2");
        check_error(execlp("gcc", "gcc", "-std=c99", "-S", argv[1], "-o", as_name, NULL) != -1, "execlp");

        exit(EXIT_SUCCESS);
    }

    close(pipeFds[WR_END]);

    int status = 0;
    check_error(wait(&status) != -1, "wait");
    if(!(WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS)){
        free(as_name);
        exit(WEXITSTATUS(status));
    }

    FILE* f = fopen(as_name, "r");
    check_error(f != NULL, "fopen");

    char* line = NULL;
    size_t read_bytes = 0;
    int mov_num = 0;
    while(getline(&line, &read_bytes, f) > 0){
        if(strstr(line, "mov") != NULL){
            mov_num++;
        }
    }

    printf("%d\n", mov_num);

    fclose(f);
    check_error(unlink(as_name) != -1, "unlink");
    free(as_name);
    free(line);

    exit(EXIT_SUCCESS);
}
