// Program pokrece dete proces onoliko puta koliko fajl ima filinija, i u svakoj
// liniji broji koliko se javilo slovo a, na kraju ispisati broj slova a i broj
// pokrenutih deca proces.
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

    int fd = open(argv[1], O_RDONLY);
    check_error(fd != -1, "open");
    FILE* f = fdopen(fd, "r");
    check_error(f != NULL, "fdopen");

    char* line = NULL;
    size_t read_bytes = 0;
    int cld_num = 0, a_num = 0;
    while(getline(&line, &read_bytes, f) != -1){

        int pipeFds[2];
        check_error(pipe(pipeFds) != -1, "pipe");

        pid_t cldPid = fork();
        check_error(cldPid != -1, "fork");

        if(cldPid == 0){
            close(pipeFds[RD_END]);

            int tmp_a_num = 0;
            for(int i = 0; line[i]; i++){
                if(line[i] == 'a'){
                    tmp_a_num++;
                }
            }

            int* buffer = malloc(sizeof(int));
            check_error(buffer != NULL, "malloc");
            *buffer = tmp_a_num;
            check_error(write(pipeFds[WR_END], buffer, sizeof(int)) != -1, "write");

            free(buffer);
            close(pipeFds[WR_END]);
            _exit(EXIT_SUCCESS);
        }

        close(pipeFds[WR_END]);

        int* buffer = malloc(sizeof(int));
        check_error(buffer != NULL, "malloc");
        // read smesta u buffer procitanu vrednost, da smo koristili ret_val
        // onda bi dobili resenje n*4, tj broj liniija * sizeof(int), jer read
        // vraca broj procitanih bajtova
        check_error(read(pipeFds[RD_END], buffer, sizeof(int)) != -1, "read");
        a_num += *buffer;
        cld_num++;

        int status = 0;
        check_error(wait(&status) != -1, "status");

        free(buffer);
        close(pipeFds[RD_END]);

    }

    printf("%d %d\n", a_num, cld_num);

    free(line);
    fclose(f);

    exit(EXIT_SUCCESS);
}
