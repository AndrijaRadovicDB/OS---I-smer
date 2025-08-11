// Program ucitava putanje do regularnih fajlova koje se salju detetu i dete
// odredjuje broj linija u zadatom fajlu i to salje nazad roditelju. Na kraju
// roditelj ispisuje ukupan broj linija u svim fajlovima.
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

#define MAX_SIZE (256)
#define RD_END (0)
#define WR_END (1)

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    int line_num = 0;
    char tmp[MAX_SIZE];
    while(scanf("%s", tmp) == 1){

        int pipeFds[2];
        check_error(pipe(pipeFds) != -1, "pipe");
        pid_t cldPid = fork();
        check_error(cldPid != -1, "fork");

        if(cldPid == 0){
            close(pipeFds[RD_END]);

            check_error(dup2(pipeFds[WR_END], STDOUT_FILENO) != -1, "dup2");

            FILE* f = fopen(tmp, "r");
            check_error(f != NULL, "fopen");

            char* line = NULL;
            size_t len = 0;
            int lines = 0;
            while(getline(&line, &len, f) > 0){
                lines++;
            }
            check_error(write(pipeFds[WR_END], &lines, sizeof(int)) != -1, "write");

            free(line);
            fclose(f);

            exit(EXIT_SUCCESS);
        }

        close(pipeFds[WR_END]);

        int num = 0;
        check_error(read(pipeFds[RD_END], &num, sizeof(int)) != -1, "read");
        line_num += num;

        int status = 0;
        check_error(wait(&status) != -1, "wait");
    }

    printf("%d\n", line_num);

    exit(EXIT_SUCCESS);
}
