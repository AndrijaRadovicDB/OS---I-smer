// Program ucitava m, n, k pa m + 1 vektor duzine n tipa float. Pokrece k niti
// koja racuna skalarni proizvod m/k vektora sa poslednjim vektorom. Niti vracaju
// lokalni maks skalarni proizvod, u mainu se trazi maksimalni i ispisuje se
// indeks niti koji ga je izracunao kao i skalarni proizvod.
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
    int start;
} OsThreadArg;

typedef struct{
    float lmax_scalar;
} OsThreadOutput;


int m, n, k;
float** vectors;

void* OsThreadFunc(void* args){

    OsThreadArg* arg = (OsThreadArg*)args;

    float lmax = INT_MIN;
    for(int i = arg->start; i < m/k + arg->start; i++){
        float value = 0;
        for(int j = 0; j < n; j++){
            value += vectors[i][j]*vectors[m][j];
        }
        if(value > lmax){
            lmax = value;
        }
    }

    OsThreadOutput* output = malloc(sizeof(OsThreadOutput));
    check_error(output != NULL, "malloc");
    output->lmax_scalar = lmax;

    return output;
}

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    scanf("%d%d%d", &m, &n, &k);

    vectors = malloc((m + 1)*sizeof(float*));
    check_error(vectors != NULL, "malloc");
    for(int i = 0; i <= m; i++){
        vectors[i] = malloc(n*sizeof(float));
        check_error(vectors[i] != NULL, "malloc");
    }

    for(int i = 0; i <= m; i++){
        for(int j = 0; j < n; j++){
            scanf("%f", &vectors[i][j]);
        }
    }

    pthread_t* tids = malloc(k*sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    OsThreadArg* data = malloc(k*sizeof(OsThreadArg));
    check_error(data != NULL, "malloc");

    for(int i = 0; i < k; i++){
        data[i].start = i*(m/k);
        OsPthreadCheck(pthread_create(&tids[i], NULL, OsThreadFunc, &data[i]), "create");
    }

    float gmax_scalar = INT_MIN;
    int max_index;
    for(int i = 0; i < k; i++){
        OsThreadOutput* data_output = NULL;
        OsPthreadCheck(pthread_join(tids[i], (void**)&data_output), "join");
        if(gmax_scalar < data_output->lmax_scalar){
            gmax_scalar = data_output->lmax_scalar;
            max_index = i + 1;
        }
        free(data_output);
    }
    printf("%d %f\n", max_index, gmax_scalar);

    for(int i = 0; i <= m; i++){
        free(vectors[i]);
    }
    free(vectors);
    free(tids);
    free(data);

    exit(EXIT_SUCCESS);
}
