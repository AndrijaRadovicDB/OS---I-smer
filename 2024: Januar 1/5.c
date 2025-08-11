// Program sa standardnog ulaza ucitava putanje i za svaki fajl pokrece nit koja
// racuna sumu duzina svih reci u fajlu, a onda azurira globalnu sumu koja se na
// kraju ispisuje. Za sinhronizaciju se korist muteks.
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

#define OsPthreadCheck(pthreadErr, msg)\
    do{\
        int _pthreadErr = pthreadErr;\
        if(_pthreadErr > 0){\
            errno = _pthreadErr;\
            check_error(false, msg);\
        }\
    } while(0)

#define MAX_SIZE 256

typedef struct{
    char* fPath;
} OsThreadArg;

int global_sum = 0;
pthread_mutex_t global_lock;

// Umesto fdopen, moze da se cita karakter po karakter, pa ako je razlicit od
// beline da se poveca value
void* OsThreadFunc(void* args){

    OsThreadArg* arg = (OsThreadArg*)args;

    int value = 0;
    int fd = open(arg->fPath, O_RDONLY);
    check_error(fd != -1, "open");
    FILE* f = fdopen(fd, "r");
    check_error(f != NULL, "fdopen");

    char buffer[MAX_SIZE];
    while(fscanf(f, "%s", buffer) == 1){
        value += strlen(buffer);
    }

    fclose(f);

    OsPthreadCheck(pthread_mutex_lock(&global_lock), "lock");
    global_sum += value;
    OsPthreadCheck(pthread_mutex_unlock(&global_lock), "unlock");

    return NULL;
}

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    char word[MAX_SIZE];
    char** paths = malloc(MAX_SIZE*sizeof(char*));
    check_error(paths != NULL, "malloc");
    int n = 0;
    while(scanf("%s", word) == 1){
        paths[n] = malloc((strlen(word) + 1)*sizeof(char));
        check_error(paths[n] != NULL, "malloc");
        strcpy(paths[n], word);
        n++;
        if(n == MAX_SIZE){
            int k = 2*n;
            paths = realloc(paths, k*sizeof(char*));
            check_error(paths != NULL, "realloc");
        }
    }

    pthread_t* tids = malloc(n*sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    OsThreadArg* data = malloc(n*sizeof(OsThreadArg));
    check_error(data != NULL, "malloc");

    OsPthreadCheck(pthread_mutex_init(&global_lock, NULL), "init");

    for(int i = 0; i < n; i++){
        data[i].fPath = paths[i];
        OsPthreadCheck(pthread_create(&tids[i], NULL, OsThreadFunc, &data[i]), "create");
    }

    for(int i = 0; i < n; i++){
        OsPthreadCheck(pthread_join(tids[i], NULL), "join");
    }

    OsPthreadCheck(pthread_mutex_destroy(&global_lock), "destroy");

    printf("%d\n", global_sum);

    for(int i = 0; i < n; i++){
        free(paths[i]);
    }
    free(paths);
    free(tids);
    free(data);

    exit(EXIT_SUCCESS);
}
