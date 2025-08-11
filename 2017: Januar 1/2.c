// Program pokrece dete proces i u njemu komandu terminala stat, preusmeriti
// standardni izlaz stata u obraditi ga tako da se u roditeljskom procesu ispise
// velicina fajla, a ako se stat ne izvrsi uspesno ispisati u roditelju Neuspeh
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
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

#define READ_END (0)
#define WRITE_END (1)
#define MAX_SIZE (8192)

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    struct stat fInfo;
    if(stat(argv[1], &fInfo) == -1){
        printf("Neuspeh\n");
        exit(EXIT_SUCCESS);
    }

    int pipeFds[2];
    check_error(pipe(pipeFds) != -1, "pipe");

    pid_t childPid = fork();
    check_error(childPid != -1, "fork");

    if(childPid == 0){ // Dete
        close(pipeFds[READ_END]); // Zatvaramo sta nam ne treba

        // Preusmeravanje ispisa
        check_error(dup2(pipeFds[WRITE_END], STDOUT_FILENO) != -1, "dup2");

        // --format=%s vraca velicinu
        check_error(execlp("stat", "stat", "--format=%s", argv[1], NULL) != -1, "execlp");
    }

    close(pipeFds[WRITE_END]);
    char buffer[MAX_SIZE];
    check_error(read(pipeFds[READ_END], buffer, MAX_SIZE) != -1, "read");
    close(pipeFds[READ_END]);

    int status = 0;
    check_error(waitpid(childPid, &status, 0) != -1, "wait");

    if(WIFEXITED(status)){
        if(WEXITSTATUS(status) != EXIT_SUCCESS){
            printf("Neuspeh\n");
        }
    }
    else{
        printf("Neuspeh\n");
    }

    int size;
    sscanf(buffer, "%d", &size);
    printf("%d\n", size);

    exit(EXIT_SUCCESS);
}
