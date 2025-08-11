// Program pokrece dete i pokrece cat fpath. Preusmeriti izlaz deteta u roditelja
// i ispisati broj linija i broj karaktera najduze linije u fajlu.
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

    int pipeFds[2];
    check_error(pipe(pipeFds) != -1, "pipe");
    pid_t cldPid = fork();
    check_error(cldPid != -1, "fork");

    int max_chars = 0;
    int num_lines = 0;
    int ch_num = 0;

    if(cldPid == 0){
        close(pipeFds[RD_END]);

        check_error(dup2(pipeFds[WR_END], STDOUT_FILENO) != -1, "dup2");
        check_error(execlp("cat", "cat", argv[1], NULL) != -1, "execlp");

        exit(EXIT_SUCCESS);
    }

    close(pipeFds[WR_END]);
    FILE* f = fdopen(pipeFds[RD_END], "r");
    check_error(f != NULL, "fdopen");

    char* line = NULL;
    size_t len = 0;
    int read = 0;

    while((read = getline(&line, &len, f)) > 0){
        num_lines++;
        if(read > max_chars){
            max_chars = read;
            // getline ne bi trebalo da broji /0, ali ako broji ide read - 1
        }
    }

    int status = 0;
    check_error(wait(&status) != -1, "wait");

    printf("%d %d\n", num_lines, max_chars);

    free(line);
    fclose(f);

    exit(EXIT_SUCCESS);
}
