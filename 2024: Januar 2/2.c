// Program kroz promenjivu okruzenja REG_FAJL dobija putanju do reg fajla. Zatim
// se poziva dete koje izvrsava cat za prosledjeni fajl cije se sadrzaj salje
// roditelju kroz pajp preusmeravanjem. Roditelj ispisuje duzinu najduze linije.
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

extern char** environ;

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    char* value = getenv("REG_FAJL");
    check_error(value != NULL, "getenv");

    int pipeFds[2];
    check_error(pipe(pipeFds) != -1, "pipe");
    pid_t cldPid = fork();
    check_error(cldPid != -1, "fork");

    if(cldPid == 0){
        close(pipeFds[RD_END]);

        check_error(dup2(pipeFds[WR_END], STDOUT_FILENO) != -1, "dup2");
        check_error(execlp("cat", "cat", value, NULL) != -1, "execlp");

        exit(EXIT_SUCCESS);
    }

    close(pipeFds[WR_END]);
    FILE* f = fdopen(pipeFds[RD_END], "r");
    check_error(f != NULL, "fdopen");

    char* line = NULL;
    size_t len = 0;
    int max_len = 0, tmp_len;
    while((tmp_len = getline(&line, &len, f)) > 0){
        if(tmp_len > max_len){
            max_len = tmp_len;
        }
    }

    int status = 0;
    check_error(wait(&status) != -1, "wait");

    printf("%d\n", max_len);

    free(line);
    fclose(f);

    exit(EXIT_FAILURE);
}
