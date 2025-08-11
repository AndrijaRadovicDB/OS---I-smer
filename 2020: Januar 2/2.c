// Program u detetu pokrece find . -type f i salje roditelju rezultat, roditelj
// ispisuje broj fajlova sa datom ekstenzijom
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

char* ext = NULL;

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    ext = argv[1];

    int pipeFds[2];
    check_error(pipe(pipeFds) != -1, "pipe");

    pid_t cldPid = fork();
    check_error(cldPid != -1, "fork");

    if(cldPid == 0){
        close(pipeFds[RD_END]);

        check_error(dup2(pipeFds[WR_END], STDOUT_FILENO) != -1, "dup2");
        check_error(execlp("find", "find", ".", "-type", "f", NULL) != -1, "execlp");

        close(pipeFds[WR_END]);
    }

    close(pipeFds[WR_END]);
    FILE* f = fdopen(pipeFds[RD_END], "r");
    check_error(f != NULL, "fdopen");

    int ext_num = 0;
    char* line = NULL;
    size_t read_bytes = 0;

    while(getline(&line, &read_bytes, f) != -1){
        char* tmp;
        if((tmp = strrchr(line, '.')) != NULL){
            if(!strncmp(tmp, ext, strlen(ext))){
                ext_num++;
            }
        }
    }
    printf("%d\n", ext_num);

    int status = 0;
    check_error(wait(&status) != -1, "wait");

    free(line);
    fclose(f);

    exit(EXIT_SUCCESS);
}
