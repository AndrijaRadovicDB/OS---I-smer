// Program cita fajl liniju po liniju, svaka linije je komanda koja se pokrece u
// dete procesu, izlaz se preusmerava na stderr, roditelj broj koliko karaktera
// je ispisano na stderr. Na kraju se ispisuje komanda koja je ispisala najvise
// karaktera na stderr i njihov broj.
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

#define MAX_SIZE 256
#define RD_END (0)
#define WR_END (1)

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    FILE* f = fopen(argv[1], "r");
    check_error(f != NULL, "fopen");

    char* line = NULL;
    size_t len = 0;
    char max_command[MAX_SIZE];
    int max_chars = -1;

    while(getline(&line, &len, f) > 0){

        char** commands = malloc(MAX_SIZE*sizeof(char*));
        check_error(commands != NULL, "malloc");

        int n = 0, cur_read = 0, global_read = 0;
        char tmp[MAX_SIZE];
        while(sscanf(line + global_read, "%s%*c%n", tmp, &cur_read) == 1){
            commands[n] = malloc(MAX_SIZE*sizeof(char));
            check_error(commands[n] != NULL, "malloc");
            strcpy(commands[n], tmp);
            n++;
            global_read += cur_read;
        }
        commands[n] = NULL;

        int pipeFds[2];
        check_error(pipe(pipeFds) != -1, "pipe");
        pid_t cldPid = fork();
        check_error(cldPid != -1, "fork");

        if(cldPid == 0){
            close(pipeFds[RD_END]);

            int fd = open("/dev/null", O_WRONLY);
            check_error(fd != -1, "open");

            check_error(dup2(fd, STDOUT_FILENO) != -1, "dup2");
            check_error(dup2(pipeFds[WR_END], fd) != -1, "dup2");
            check_error(execvp(commands[0], commands) != -1, "execvp");

            close(fd);
            exit(EXIT_SUCCESS);
        }

        close(pipeFds[WR_END]);

        int status = 0;
        check_error(wait(&status) != -1, "status");
        if(!(WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS)){
            char buffer[MAX_SIZE];
            int total_read = 0, read_bytes = 0;

            // Ovaj while ne radi, ne znam odakle da citamo
            while((read_bytes = read(pipeFds[RD_END], buffer, MAX_SIZE)) > 0){
                total_read += read_bytes;
            }
            check_error(read_bytes != -1, "read");

            if(total_read > max_chars){
                max_chars = total_read;
                strcpy(max_command, commands[0]);
            }
        }

        for(int i = 0; i < n; i++){
            free(commands[i]);
        }
        free(commands);
    }

    if(max_chars != -1){
        printf("%s %d\n", max_command, max_chars);
    }
    else{
        printf("unknown 0\n");
    }

    free(line);
    fclose(f);

    exit(EXIT_SUCCESS);
}
