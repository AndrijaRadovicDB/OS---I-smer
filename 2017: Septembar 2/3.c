// Program ucitava liniju po liniju oblika x y op do kraja ulaza, za svaku liniju
// pokrece novi dete proces i u njemu izvrsava expr x op y, rezultat ispisuje
// roditelj na stdout
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/wait.h>

#include <stdio.h>
#include <stdlib.h>

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

    check_error(argc == 1, "Argumenti");

    int x, y;
    char op;
    while(scanf("%d%d %c", &x, &y, &op) == 3){

        // printf vraca broj ispisanih karaktera
        int xLen = snprintf(NULL, 0, "%d", x);
        char *xStr = malloc(xLen*sizeof(char) + 1);
        check_error(xStr != NULL, "malloc");
        // U xStr duzine xLen + 1 upisuje broj x
        snprintf(xStr, xLen + 1, "%d", x);

        int yLen = snprintf(NULL, 0, "%d", y);
        char *yStr = malloc(yLen*sizeof(char) + 1);
        check_error(yStr != NULL, "malloc");
        snprintf(yStr, yLen + 1, "%d", y);

        char opStr[2];
        opStr[0] = op;
        opStr[1] = '\0';

        int pipeFds[2];
        check_error(pipe(pipeFds) != -1, "pipe");

        pid_t cldPid = fork();
        check_error(cldPid != -1, "fork");

        if(cldPid == 0){ // Dete
            close(pipeFds[RD_END]);

            check_error(dup2(pipeFds[WR_END], STDOUT_FILENO) != -1, "dup2");
            check_error(execlp("expr", "expr", xStr, opStr, yStr, (char*)NULL) != -1, "execlp");

            exit(EXIT_SUCCESS);
        }
        else{ // Roditelj
            close(pipeFds[WR_END]);

            FILE* f = fdopen(pipeFds[RD_END], "r");
            check_error(f != NULL, "fdopen");

            char* line = NULL;
            size_t read_bytes = 0;
            check_error(getline(&line, &read_bytes, f) != -1, "getline");

            int status = 0;
            check_error(wait(&status) != -1, "wait");

            printf("%s\n", line);

            free(line);
            fclose(f);
        }

        free(xStr);
        free(yStr);
    }

    exit(EXIT_SUCCESS);
}
