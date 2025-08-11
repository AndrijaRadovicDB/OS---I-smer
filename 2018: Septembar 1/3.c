// Program ispisuje p normu matrice, ucitava m i n ma matricu m*n, i za svaku
// vrstu pokrece po 1 nit. Niti sinhronizovati muteksom.
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
#include <math.h>

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
} osThreadArg;

int M, N;
double** matrix;
double P;
double norm = 0;
pthread_mutex_t global_lock;

void* osThreadFunc(void* args){

    osThreadArg* arg = (osThreadArg*)args;

    double value = 0;
    for(int j = 0; j < N; j++){
        value += pow(fabs(matrix[arg->row][j]), P);
    }

    osPthreadCheck(pthread_mutex_lock(&global_lock), "lock");
    norm += value;
    osPthreadCheck(pthread_mutex_unlock(&global_lock), "unlock");

    return NULL;
}


int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    scanf("%lf%d%d", &P, &M, &N);

    matrix = malloc(M*sizeof(double*));
    check_error(matrix != NULL, "malloc");
    for(int i = 0; i < M; i++){
        matrix[i] = malloc(N*sizeof(double));
        check_error(matrix[i] != NULL, "malloc");
    }

    for(int i = 0; i < M; i++){
        for(int j = 0; j < N; j++){
            scanf("%lf", &matrix[i][j]);
        }
    }

    pthread_t* tids = malloc(M*sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    osThreadArg* data = malloc(M*sizeof(osThreadArg));
    check_error(data != NULL, "malloc");

    osPthreadCheck(pthread_mutex_init(&global_lock, NULL), "init");

    for(int i = 0; i < M; i++){
        data[i].row = i;
        osPthreadCheck(pthread_create(&tids[i], NULL, osThreadFunc, &data[i]), "create");
    }

    for(int i = 0; i < M; i++){
        osPthreadCheck(pthread_join(tids[i], NULL), "join");
    }

    osPthreadCheck(pthread_mutex_destroy(&global_lock), "destroy");

    printf("%lf\n", pow(norm, 1.0/P));

    for(int i = 0; i < M; i++){
        free(matrix[i]);
    }
    free(matrix);
    free(tids);
    free(data);


    exit(EXIT_SUCCESS);
}
