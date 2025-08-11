// Program ucitava iz fajla nizove elemenata, i za svaki red pokrece nit koja
// odredjuje maksimum tog reda i azurira globalni maksimum ako je potrebno.
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
#include <limits.h>

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

typedef struct{
    int row;
    int len;
} OsThreadArg;

int global_max = INT_MIN;
int** arrays;
pthread_mutex_t global_lock;

void* OsThreadFunc(void* args){

    OsThreadArg* arg = (OsThreadArg*)args;

    int value = INT_MIN;
    for(int j = 0; j < arg->len; j++){
        if(value < arrays[arg->row][j]){
            value = arrays[arg->row][j];
        }
    }

    OsPthreadCheck(pthread_mutex_lock(&global_lock), "lock");
    if(global_max < value){
        global_max = value;
    }
    OsPthreadCheck(pthread_mutex_unlock(&global_lock), "unlock");

    return NULL;
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "stat");
    check_error(S_ISREG(fInfo.st_mode), "Nije reg");

    FILE* f = fopen(argv[1], "r");
    check_error(f != NULL, "fopen");

    int n;
    fscanf(f, "%d", &n);
    arrays = malloc(n*sizeof(int*));
    check_error(arrays != NULL, "malloc");
    int* lens = malloc(n*sizeof(int));
    for(int i = 0; i < n; i++){
        fscanf(f, "%d", &lens[i]);
        arrays[i] = malloc(lens[i]*sizeof(int));
        check_error(arrays[i] != NULL, "malloc");
        for(int j = 0; j < lens[i]; j++){
            fscanf(f, "%d", &arrays[i][j]);
        }
    }

    pthread_t* tids = malloc(n*sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    OsThreadArg* data = malloc(n*sizeof(OsThreadArg));
    check_error(data != NULL, "malloc");

    OsPthreadCheck(pthread_mutex_init(&global_lock, NULL), "init");

    for(int i = 0; i < n; i++){
        data[i].row = i;
        data[i].len = lens[i];
        OsPthreadCheck(pthread_create(&tids[i], NULL, OsThreadFunc, &data[i]), "create");
    }

    for(int i = 0; i < n; i++){
        OsPthreadCheck(pthread_join(tids[i], NULL), "join");
    }

    OsPthreadCheck(pthread_mutex_destroy(&global_lock), "destroy");

    printf("%d\n", global_max);

    for(int i = 0; i < n; i++){
        free(arrays[i]);
    }
    free(arrays);
    free(lens);
    free(tids);
    free(data);

    exit(EXIT_SUCCESS);
}
