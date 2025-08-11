// Program izvrsava komandu head -n br_linija fpath u detetu i izlaz preusmerava
// na roditelja koji ispisaju najduzu liniju na standardni izlaz.
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

    int pipeFds[2];
    check_error(pipe(pipeFds) != -1, "pipe");
    pid_t cldPid = fork();
    check_error(cldPid != -1, "fork");

    int max_len = -1;
    char* max_name = NULL;

    if(cldPid == 0){
        close(pipeFds[RD_END]);

        check_error(dup2(pipeFds[WR_END], STDOUT_FILENO) != -1, "dup2");
        check_error(execlp("head", "head", "-n", argv[2], argv[1], NULL) != -1, "execlp");

        exit(EXIT_SUCCESS);
    }

    close(pipeFds[WR_END]);
    FILE* f = fdopen(pipeFds[RD_END], "r");
    check_error(f != NULL, "fdopen");

    int status = 0;
    check_error(wait(&status) != -1, "wait");

    char* line = NULL;
    size_t len = 0;
    int read = 0;
    while((read = getline(&line, &len, f)) > 0){
        // Ako bi samo mallocovali imali bi curenje memorije
        if(max_len == -1){
            max_name = malloc((read + 1)*sizeof(char));
            check_error(max_name != NULL, "malloc");
        }
        if(read > max_len){
            max_len = read;
            max_name = realloc(max_name, (max_len + 1)*sizeof(char));
            check_error(max_name != NULL, "realloc");
            strcpy(max_name, line);
        }
    }

    printf("%s", max_name);

    free(max_name);
    free(line);
    fclose(f);

    exit(EXIT_FAILURE);
}
