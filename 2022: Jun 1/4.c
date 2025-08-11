// Program iz fajla cita broj redova, a na pocetku reda cita broj elemenata reda.
// Pokrece se n niti i svaka trazi maksimum niza. Na kraju ispisati maksimum.
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
int** matrix;
pthread_mutex_t global_lock;

void* OsThreadFunc(void* args){

    OsThreadArg* arg = (OsThreadArg*)args;

    int value = INT_MIN;
    for(int j = 0; j < arg->len; j++){
        if(value < matrix[arg->row][j]){
            value = matrix[arg->row][j];
        }
    }

    OsPthreadCheck(pthread_mutex_lock(&global_lock), "lock");
    if(value > global_max){
        global_max = value;
    }
    OsPthreadCheck(pthread_mutex_unlock(&global_lock), "unlock");

    return NULL;
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "stat");
    check_error(S_ISREG(fInfo.st_mode), "Nije regularan fajl");

    FILE* f = fopen(argv[1], "r");
    check_error(f != NULL, "fopen");

    int n;
    fscanf(f, "%d", &n);

    int* row_size = malloc(n*sizeof(int));
    check_error(row_size != NULL, "malloc");
    matrix = malloc(n*sizeof(int*));
    check_error(matrix != NULL, "malloc");
    for(int i = 0; i < n; i++){
        int m;
        fscanf(f, "%d", &m);
        row_size[i] = m;
        matrix[i] = malloc(m*sizeof(int));
        check_error(matrix[i] != NULL, "malloc");
        for(int j = 0; j < m; j++){
            fscanf(f, "%d", &matrix[i][j]);
        }
    }

    fclose(f);

    pthread_t* tids = malloc(n*sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    OsThreadArg* data = malloc(n*sizeof(OsThreadArg));
    check_error(data != NULL, "malloc");

    OsPthreadCheck(pthread_mutex_init(&global_lock, NULL), "init");

    for(int i = 0; i < n; i++){
        data[i].row = i;
        data[i].len = row_size[i];
        OsPthreadCheck(pthread_create(&tids[i], NULL, OsThreadFunc, &data[i]), "create");
    }

    for(int i = 0; i < n; i++){
        OsPthreadCheck(pthread_join(tids[i], NULL), "join");
    }

    OsPthreadCheck(pthread_mutex_destroy(&global_lock), "destroy");

    printf("%d\n", global_max);

    for(int i = 0; i < n; i++){
        free(matrix[i]);
    }
    free(matrix);
    free(row_size);
    free(tids);
    free(data);

    exit(EXIT_SUCCESS);
}
