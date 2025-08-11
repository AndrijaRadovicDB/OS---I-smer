// Program u detetu pokrece find . -type f i rezultat salje roditelju, roditelj
// ispisuje fajlove sa trazenom ekstenzijom
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

    int cld2par[2];
    check_error(pipe(cld2par) != -1, "pipe");

    pid_t cldPid = fork();
    check_error(cldPid != -1, "fork");

    if(cldPid == 0){ // Dete

        close(cld2par[RD_END]);

        check_error(dup2(cld2par[WR_END], STDOUT_FILENO) != -1, "dup2");

        // Na kraju ide NULL jer komandi ne saljemo argument
        check_error(execlp("find", "find", ".", "-type", "f", (char*)NULL) != -1, "execlp");

        exit(EXIT_SUCCESS);
    }

    close(cld2par[WR_END]);

    FILE* f = fdopen(cld2par[RD_END] , "r");
    check_error(f != NULL, "fdopen");

    char* line = NULL;
    char* tmp = NULL;
    int ext_len = strlen(argv[1]);
    int br_fajlova = 0;
    size_t read_bytes = 0;

    while(getline(&line, &read_bytes, f) != -1){
        tmp = strrchr(line, '.');
        if(tmp != NULL && !strncmp(tmp, argv[1], ext_len)){
            br_fajlova++;
        }
    }
    printf("%d\n", br_fajlova);

    // Oslobadjamo jer getline vrsi dinamicku alokaciju ako je string NULL
    free(line);
    fclose(f);

    int status = 0;
    check_error(waitpid(cldPid, &status, 0) != -1, "waitpid");

    exit(EXIT_SUCCESS);
}
