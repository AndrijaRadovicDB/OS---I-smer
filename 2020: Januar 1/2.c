// Program ucitava n i m, a zatim n*m brojeva (float). Pokrenuti n niti i svaka
// nit racuna proizvod 1 reda i vraca ga kao povratnu vrednostu. U mainu ispisati
// maksimum svih proizvoda.
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

#define osPthreadCheck(pthreadErr, msg)\
    do{\
        int _pthreadErr = pthreadErr;\
        if(_pthreadErr > 0){\
            errno = _pthreadErr;\
            check_error(false, msg);\
        }\
    } while(0)

typedef struct{
    int row;
    float max;
} OsThreadArg;

float** matrix;
int n, m;

void* OsThreadFunc(void* args){

    OsThreadArg* arg = (OsThreadArg*)args;

    float value = 1.0;
    for(int i = 0; i < m; i++){
        value *= matrix[arg->row][i];
    }

    OsThreadArg* data = malloc(sizeof(OsThreadArg));
    check_error(data != NULL, "malloc");
    data->max = value;

    return data;
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

    for(int i = 0; i < n; i++){
        data[i].row = i;
        osPthreadCheck(pthread_create(&tids[i], NULL, OsThreadFunc, &data[i]), "pthread_create");
    }

    float global_max = INT_MIN;
    for(int i = 0; i < n; i++){
        OsThreadArg* tmp = NULL;
        osPthreadCheck(pthread_join(tids[i], (void**)&tmp), "pthread_join");
        if(tmp->max > global_max){
            global_max = tmp->max;
        }
        free(tmp);
    }

    printf("%f\n", global_max);

    for(int i = 0; i < n; i++){
        free(matrix[i]);
    }
    free(matrix);
    free(tids);
    free(data);

    exit(EXIT_SUCCESS);
}
