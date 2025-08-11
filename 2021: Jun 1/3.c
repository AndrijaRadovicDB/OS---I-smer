// Program ucitava m, n, k, pa m vektora duzine n. Potrebno pokrenuti k niti
// koja racuna normu od m/k vektora. Niti vracaju lokalnu maksimalnu normu. U
// mainu racunamo maksimalnu normu i ispisujemo indeks niti koji ju je vratio i
// maks normu.
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
    float lmax_norm;
} OsThreadOutput;

int m, n, k;
float** vectors;

void* OsThreadFunc(void* args){

    OsThreadArg* arg = (OsThreadArg*)args;

    float local_max = -1.0;
    // Ako ocemo da i krece od arg->starta, moramo start da dodamo na duzinu
    for(int i = 0; i < m/k; i++){
        float value = 0.0;
        for(int j = 0; j < n; j++){
            value += vectors[i + arg->start][j]*vectors[i + arg->start][j];
        }
        if(value > local_max){
            local_max = value;
        }
    }

    OsThreadOutput* output = malloc(sizeof(OsThreadOutput));
    check_error(output != NULL, "malloc");
    output->lmax_norm = local_max;

    return output;
}

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    scanf("%d%d%d", &m, &n, &k);

    vectors = malloc(m*sizeof(float*));
    check_error(vectors != NULL, "malloc");
    for(int i = 0; i < m; i++){
        vectors[i] = malloc(n*sizeof(float));
        check_error(vectors[i] != NULL, "malloc");
    }

    for(int i = 0; i < m; i++){
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

    float max_norm = -1.0;
    int max_index;
    for(int i = 0; i < k; i++){
        OsThreadOutput* data_output = NULL;
        OsPthreadCheck(pthread_join(tids[i], (void**)&data_output), "join");
        if(data_output->lmax_norm > max_norm){
            max_norm = data_output->lmax_norm;
            max_index = i;
        }
        free(data_output);
    }
    printf("%d %f\n", max_index, sqrt(max_norm));

    for(int i = 0; i < m; i++){
        free(vectors[i]);
    }
    free(vectors);
    free(tids);
    free(data);

    exit(EXIT_SUCCESS);
}
