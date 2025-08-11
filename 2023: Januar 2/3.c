// Program ucitava m i n, i onda m*n realnih brojeva. Za svaki red se pokrece po
// matrica i oredjuje maksimum reda. Ispsiuje se maksimum reda i njegova pozicija
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
} OsThreadArg;

int m, n;
int max_i, max_j;
double max = INT_MIN;
double** matrix;
pthread_mutex_t global_lock;

void* OsThreadFunc(void* args){

    OsThreadArg* arg = (OsThreadArg*)args;

    double value = INT_MIN;
    int tmax_i, tmax_j;
    for(int j = 0; j < n; j++){
        if(value < matrix[arg->row][j]){
            value = matrix[arg->row][j];
            tmax_j = j;
        }
    }

    OsPthreadCheck(pthread_mutex_lock(&global_lock), "lock");
    if(value > max){
        max = value;
        max_i = arg->row;
        max_j = tmax_j;
    }
    OsPthreadCheck(pthread_mutex_unlock(&global_lock), "unlock");

    return NULL;
}

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    scanf("%d%d", &m, &n);
    matrix = malloc(m*sizeof(double*));
    check_error(matrix != NULL, "malloc");
    for(int i = 0; i < m; i++){
        matrix[i] = malloc(n*sizeof(double));
        check_error(matrix[i] != NULL, "malloc");
    }

    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            scanf("%lf", &matrix[i][j]);
        }
    }

    pthread_t* tids = malloc(m*sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    OsThreadArg* data = malloc(m*sizeof(OsThreadArg));
    check_error(data != NULL, "malloc");

    OsPthreadCheck(pthread_mutex_init(&global_lock, NULL), "init");

    for(int i = 0; i < m; i++){
        data[i].row = i;
        OsPthreadCheck(pthread_create(&tids[i], NULL, OsThreadFunc, &data[i]), "create");
    }

    for(int i = 0; i < m; i++){
        OsPthreadCheck(pthread_join(tids[i], NULL), "join");
    }

    OsPthreadCheck(pthread_mutex_destroy(&global_lock), "destroy");

    printf("%g %d %d\n", max, max_i, max_j);

    for(int i = 0; i < m; i++){
        free(matrix[i]);
    }
    free(tids);
    free(data);

    exit(EXIT_SUCCESS);
}
