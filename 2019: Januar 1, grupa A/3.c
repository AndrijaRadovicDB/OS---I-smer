// Program ucitava cele brojeve M, N, K, pa onda M vektora duzine N. Pokrecemo K
// nuti od kojih svaka racuna m/k vektora. Niti vracaju lokalni maks normi.
// Ispisati indeks niti koja je izracunala maks normu i maks normu.
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
    int start;
    int index;
    float max_norm;
} osThreadArg;

int M, N, K;
int max_index;
float** vectors;
float global_max = -1;
pthread_mutex_t global_lock;

void* osThreadFunc(void* args){

    osThreadArg* arg = (osThreadArg*)args;

    float max = -1.0;
    for(int i = 0; i < M/K; i++){
        float value = 0;
        for(int j = 0; j < N; j++){
            value += vectors[i + arg->start][j]*vectors[i + arg->start][j];
        }
        if(value > max){
            max = value;
        }
    }

    osThreadArg* data = malloc(sizeof(osThreadArg));
    check_error(data != NULL, "malloc");

    data->max_norm = sqrt(max);

    return data;
}

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    scanf("%d%d%d", &M, &N, &K);

    vectors = malloc(M*sizeof(float*));
    check_error(vectors != NULL, "malloc");
    for(int i = 0; i < M; i++){
        vectors[i] = malloc(N*sizeof(float));
        check_error(vectors[i] != NULL, "malloc");
    }

    for(int i = 0; i < M; i++){
        for(int j = 0; j < N; j++){
            scanf("%f", &vectors[i][j]);
        }
    }

    pthread_t* tids = malloc(K*sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    osThreadArg* data = malloc(K*sizeof(osThreadArg));
    check_error(data != NULL, "malloc");

    for(int i = 0; i < K; i++){
        data[i].index = i;
        data[i].start = i*(M/K);
        osPthreadCheck(pthread_create(&tids[i], NULL, osThreadFunc, &data[i]), "create");
    }

    for(int i = 0; i < K; i++){
        osThreadArg* data1 = NULL;
        // Ako umesto NULL stavimo ne NULL vrednost u nju ce se sacuvati
        // podaci stringa
        osPthreadCheck(pthread_join(tids[i], (void**)&data1), "join");
        if(data1->max_norm > global_max){
            global_max = data1->max_norm;
            max_index = i;
        }
        free(data1);
    }

    printf("%d %f\n", max_index, global_max);

    for(int i = 0; i < M; i++){
        free(vectors[i]);
    }
    free(vectors);
    free(tids);
    free(data);

    exit(EXIT_SUCCESS);
}
