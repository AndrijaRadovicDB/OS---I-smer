// Program dobija putanju i neku od niski -w/l/c i u detetu izvrsava komandu
// wc -w/l/c putanja, dete preusmeriti u roditelja i ispisati rezultat na stdout
// u slucaju da dete ne uspe da izvrsi komandu ispisati Neuspeh
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

    check_error(argc == 3, "Argumenti");

    struct stat fInfo;
    int retVal = stat(argv[1], &fInfo);
    if(retVal == -1){
        printf("Neuspeh\n");
        exit(EXIT_SUCCESS);
    }

    int pipeFds[2];
    check_error(pipe(pipeFds) != -1, "pipe");

    pid_t cldPid = fork();
    check_error(cldPid != -1, "fork");

    if(cldPid == 0){ // Dete
        close(pipeFds[RD_END]);

        check_error(dup2(pipeFds[WR_END], STDOUT_FILENO) != -1, "dup2");
        check_error(execlp("wc", "wc", argv[2], argv[1], (char*)NULL) != -1, "execlp");

    }

    close(pipeFds[WR_END]);

    FILE* f = fdopen(pipeFds[RD_END], "r");
    check_error(f != NULL, "fdopen");

    char* line = NULL;
    size_t read_bytes = 0;
    int num;
    check_error(getline(&line, &read_bytes, f) != -1, "getline");
    sscanf(line, "%d", &num);
    printf("%d\n", num);

    free(line);
    fclose(f);
    close(pipeFds[RD_END]);

    int status = 0;
    check_error(waitpid(cldPid, &status, 0) != -1, "waitpid");


    exit(EXIT_SUCCESS);
}
