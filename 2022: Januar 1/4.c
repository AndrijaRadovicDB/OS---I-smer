// Program ucitava liniju po liniju na stdin. U svakoj liniji se prvo nalazi broj
// koji predstavlja broj reci komande i zatim toliko reci. Za svaku liniju
// pokrece novo dete i njemu izvrsava komandu. Na kraju roditelj treba da ispise
// broj neuspesno izvrsenih komandi i uspesnu komandu koja je ispisala najvise
// karaktera. Ako nema uspesnih komandi ispisati nema
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

#define MAX_LINE 255
#define RD_END (0)
#define WR_END (1)

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    char* line = NULL;
    char* name = malloc((MAX_LINE + 1)*sizeof(char));
    check_error(name != NULL, "malloc");
    size_t len = 0;
    int failed = 0, all_commands = 0, max_bytes = -1;
    while(getline(&line, &len, stdin) > 0){

        all_commands++;
        int n, cur_read = 0, global_read = 0;
        // Za razliku od scanf i fscanf sscanf ne pamti dokle smo stilgi sa
        // citanjem pa moramo sami da se pomeramo. %*c ucitava belinu ali je
        // ignorise, %n kaze da je sledeca promenjiva ona u koju se cuva broj
        // procitanih bajtova. I za ukupo procitano se pomeramo u liniji.
        sscanf(line + global_read, "%d%*c%n", &n, &cur_read);
        global_read += cur_read;

        char** commands = malloc((n + 1)*sizeof(char*));
        check_error(commands != NULL, "malloc");
        for(int i = 0; i < n; i++){
            commands[i] = malloc((MAX_LINE + 1)*sizeof(char));
            check_error(commands[i] != NULL, "malloc");
        }
        commands[n] = NULL;

        for(int i = 0; i < n; i++){
            sscanf(line + global_read, "%s%*c%n", commands[i], &cur_read);
            global_read += cur_read;
        }

        int pipeFds[2];
        check_error(pipe(pipeFds) != -1, "pipe");
        pid_t cldPid = fork();
        check_error(cldPid != -1, "fork");

        if(cldPid == 0){
            close(pipeFds[RD_END]);

            check_error(dup2(pipeFds[WR_END], STDOUT_FILENO) != -1, "dup2");
            check_error(execvp(commands[0], commands) != -1, "execvp");

            exit(EXIT_SUCCESS);
        }

        close(pipeFds[WR_END]);

        int status = 0;
        check_error(wait(&status) != -1, "wait");
        if(WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS){
            int read_bytes = 0, bytes = 0;
            char buffer[MAX_LINE];
            while((read_bytes = read(pipeFds[RD_END], buffer, MAX_LINE)) > 0){
                bytes += read_bytes;
            }
            check_error(read_bytes != -1, "read");
            if(bytes > max_bytes){
                max_bytes = bytes;
                strcpy(name, commands[0]);
            }
        }
        else{
            failed++;
        }

        for(int i = 0; i < n; i++){
            free(commands[i]);
        }
        free(commands);
    }

    if(failed == all_commands){
        printf("%d nema\n", failed);
    }
    else{
        printf("%d %s\n", failed, name);
    }

    free(line);
    free(name);

    exit(EXIT_SUCCESS);
}
