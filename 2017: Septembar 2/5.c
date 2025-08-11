// Program dobija putanju do direktorijuma, pokrece ls -l u detetu, preusmerava
// stdout u pipe, filtrira sadrzaj tako da ispise samo 8. kolonu
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

// Ne radi zadatak, a ni ovo moje
int main(int argc, char** argv){

    int pipeFds[2];
    check_error(pipe(pipeFds) != -1, "pipe");

    pid_t cldPid = fork();
    check_error(cldPid != -1, "fork");

    if(cldPid == 0){ // Dete
        close(pipeFds[RD_END]);

        check_error(dup2(pipeFds[WR_END], STDOUT_FILENO) != -1, "dup2");
        check_error(execlp("ls", "ls", "-l", argv[1], (char*)NULL) != -1, "execlp");

        exit(EXIT_SUCCESS);
    }

    close(pipeFds[WR_END]);

    FILE* f = fdopen(pipeFds[RD_END], "r");
    check_error(f != NULL, "fdopen");

    char* line = NULL;
    size_t len = 0;
    int line_len;
    int numWs = 7;
    while((line_len = getline(&line, &len, f)) != -1){
        for(int i = 0; i < line_len; i++){
            if(line[i] == ' '){
                numWs--;
            }
            if(numWs == 0){
                printf("%c", line[i]);
            }
        }
        printf("\n");
    }

    free(line);
    fclose(f);

    int signal = 0;
    check_error(wait(&signal) != -1, "wait");

    free(line);

    exit(EXIT_SUCCESS);
}
