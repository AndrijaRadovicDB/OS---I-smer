// Program ucitava n*m matricu tipa float, pokrece n niti gde svaka racuna sumu
// reda. U mainu ispisati maks, za sinhronizaciju koristiti mutex.
// Zapravo traze minimalnu
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
} OsThreadArg;

int n, m;
//float max_sum = INT_MIN;
float min_sum = INT_MAX;
float** matrix;
pthread_mutex_t global_lock;

void* OsThreadFunc(void* args){

    OsThreadArg* arg = (OsThreadArg*)args;

    float value = 0;
    for(int j = 0; j < m; j++){
        value += matrix[arg->row][j];
    }

    OsPthreadCheck(pthread_mutex_lock(&global_lock), "lock");
    /*if(value > max_sum){
        max_sum = value;
    }*/
    if(value < min_sum){
        min_sum = value;
    }
    OsPthreadCheck(pthread_mutex_unlock(&global_lock), "unlock");

    return NULL;
}

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    scanf("%d%d", &n, &m);

    matrix = malloc(n*sizeof(float*));
    check_error(matrix != NULL, "malloc");
    for(int i = 0; i < n; i++){
        matrix[i] = malloc(m*sizeof(float));
        check_error(matrix[i] != NULL, "malloc");
    }

    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            scanf("%f", &matrix[i][j]);
        }
    }

    pthread_t* tids = malloc(n*sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    OsThreadArg* data = malloc(n*sizeof(OsThreadArg));
    check_error(data != NULL, "malloc");

    OsPthreadCheck(pthread_mutex_init(&global_lock, NULL), "init");

    for(int i = 0; i < n; i++){
        data[i].row = i;
        OsPthreadCheck(pthread_create(&tids[i], NULL, OsThreadFunc, &data[i]), "create");
    }

    for(int i = 0; i < n; i++){
        OsPthreadCheck(pthread_join(tids[i], NULL), "join");
    }

    //printf("%f\n", max_sum);
    printf("%f\n", min_sum);

    OsPthreadCheck(pthread_mutex_destroy(&global_lock), "destroy");

    for(int i = 0; i < n; i++){
        free(matrix[i]);
    }
    free(matrix);
    free(tids);
    free(data);

    exit(EXIT_SUCCESS);
}
