// Program za svaki regularni fajl u detetu pokrece cat i u roditelju broji broj
// linija. Ako je putanja neispravna rezultat ignorisato, ako su sve putanje
// neispravne ispisati neuspeh. Na kraju ispitati ime fajla sa najvise linija i
// broj linija.
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

    check_error(argc > 1, "Argumenti");

    int max_line_num = 0;
    char* max_name = NULL;
    for(int i = 0; i < argc - 1; i++){

        int pipeFds[2];
        check_error(pipe(pipeFds) != -1, "pipe");
        pid_t cldPid = fork();
        check_error(cldPid != -1, "fork");

        if(cldPid == 0){
            close(pipeFds[RD_END]);

            check_error(dup2(pipeFds[WR_END], STDOUT_FILENO) != -1, "dup2");
            // Da ne exit kod bio 1, ako je n - 1 putanja losa
            int ret_val = execlp("cat", "cat", argv[i + 1], NULL) != -1;

            exit(EXIT_SUCCESS);
        }

        close(pipeFds[WR_END]);
        FILE* f = fdopen(pipeFds[RD_END], "r");
        check_error(f != NULL, "fdopen");

        char* line = NULL;
        size_t len = 0;
        int line_num = 0;
        while(getline(&line, &len, f) > 0){
            line_num++;
        }

        if(line_num > max_line_num){
            max_line_num = line_num;
            char* tmp = strrchr(argv[i + 1], '/');
            if(tmp == NULL){
                max_name = argv[i + 1];
            }
            else{
                max_name = tmp + 1;
            }
        }

        int status = 0;
        check_error(wait(&status) != -1, "wait");

        free(line);
        fclose(f);
    }

    if(max_line_num == 0){
        printf("neuspeh\n");
        exit(EXIT_FAILURE);
    }
    else{
        printf("%s %d\n", max_name, max_line_num);
    }

    exit(EXIT_SUCCESS);
}
