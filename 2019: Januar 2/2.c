// Program dobija putanje do vise fajlova i za svaki pokrece komandu du -sch
// fPath u posebnom dete procesu, roditelj cita izlaz i ispisuje velicinu fajla,
// ako je komanda neuspesna, ispisuje neuspeh.
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/wait.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

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

    check_error(argc >= 2, "Argumenti");

    for(int i = 1; i < argc; i++){

        int pipeFds[2];
        check_error(pipe(pipeFds) != -1, "pipe");

        pid_t cldPid = fork();
        check_error(cldPid != -1, "fork");

        if(cldPid == 0){
            close(pipeFds[RD_END]);

            check_error(dup2(pipeFds[WR_END], STDOUT_FILENO) != -1, "dup2");
            check_error(execlp("du", "du", "-sch", argv[i], NULL) != -1, "execlp");

            close(pipeFds[WR_END]);
            exit(EXIT_SUCCESS);
        }
        close(pipeFds[WR_END]);

        int status = 0;
        check_error(wait(&status) != -1, "waitpid");
        if(!(WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS)){
            printf("neuspeh ");
        }
        else{
            while(1){
                char c;
                read(pipeFds[RD_END], &c, 1);
                if(isspace(c)){
                    break;
                }
                if(c == '.'){
                    c = ',';
                }
                putchar(c);
            }

        /*  Radi i ovo ali je manje lepo
            FILE* f = fdopen(pipeFds[RD_END], "r");
            check_error(f != NULL, "fdopen");

            char* line = NULL;
            size_t read_bytes = 0;
            getline(&line, &read_bytes, f);

            int i = 0;
            char str[5];
            while(!isspace(line[i])){
                if(line[i] == '.'){
                    line[i] = ',';
                }

                str[i] = line[i];

                i++;
            }
            if(i == 3){
                str[3] = '\0';
            }
            else{
                str[4] = '\0';
            }
            printf("%s ", str);

            free(line);
            fclose(f);
        */
        }
        putchar(' ');

        close(pipeFds[RD_END]);
    }

    exit(EXIT_SUCCESS);
}
