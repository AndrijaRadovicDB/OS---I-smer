// Program ucitava cele brojeve M, N, K i onda M + 1 vektor (float) duzine N.
// Pokrenuti K niti koje racunaj skalarni proizvod M/K vektora, i one vracaju
// maks skalarni proizvod. U main-u orediti maks skalarni proizvod i ispisati
// indeks njegove niti i njega.
#define _XOPEN_SORUCE 700
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
    int start;
    float max_scalar;
} OsThreadArg;

int M, N, K;
float** vectors;

void* OsThreadFunc(void* args){

    OsThreadArg* arg = (OsThreadArg*)args;

    float max_scalar = INT_MIN;
    for(int i = 0; i < M/K; i++){
        float value = 0;
        for(int j = 0; j < N; j++){
            value += vectors[i + arg->start][j]*vectors[M][j];
        }
        if(max_scalar < value){
            max_scalar = value;
        }
    }

    OsThreadArg* output = malloc(sizeof(OsThreadArg));
    check_error(output != NULL, "malloc");
    output->max_scalar = max_scalar;

    return output;
}

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    scanf("%d%d%d", &M, &N, &K);

    vectors = malloc((M + 1)*sizeof(float*));
    check_error(vectors != NULL, "malloc");
    for(int i = 0; i <= M; i++){
        vectors[i] = malloc(N*sizeof(float));
        check_error(vectors[i] != NULL, "malloc");
    }

    for(int i = 0; i <= M; i++){
        for(int j = 0; j < N; j++){
            scanf("%f", &vectors[i][j]);
        }
    }

    pthread_t* tids = malloc(K*sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    OsThreadArg* data = malloc(K*sizeof(OsThreadArg));
    check_error(data != NULL, "malloc");

    for(int i = 0; i < K; i++){
        data[i].start = i*(M/K);
        osPthreadCheck(pthread_create(&tids[i], NULL, OsThreadFunc, &data[i]), "create");
    }

    float global_max_scalar = INT_MIN;
    int max_index;
    for(int i = 0; i < K; i++){
        OsThreadArg* data = NULL;
        osPthreadCheck(pthread_join(tids[i], (void**)&data), "join");
        if(global_max_scalar < data->max_scalar){
            global_max_scalar = data->max_scalar;
            max_index = i + 1;
        }
        free(data);
    }
    printf("%d %f\n", max_index, global_max_scalar);

    for(int i = 0; i <= M; i++){
        free(vectors[i]);
    }
    free(vectors);
    free(tids);
    free(data);

    exit(EXIT_SUCCESS);
}
