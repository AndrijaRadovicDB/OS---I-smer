// Program ucitava broj n i n celih brojeva, pokrece se n niti i svaka racuna
// zbir cifara datog broha i azurira globalnu sumu cifara. Zbir cifara se vraca
// kao povratna vrednost niti. Sinhronizacija preko muteksa.
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
    int pos;
} OsThreadArg;

typedef struct{
    int sum;
} OsThreadOutput;

int global_sum = 0;
int* array;
pthread_mutex_t global_lock;

void* OsThreadFunc(void* args){

    OsThreadArg* arg = (OsThreadArg*)args;

    int value = 0;
    int num = abs(array[arg->pos]);
    while(num){
        value += num % 10;
        num /= 10;
    }

    OsPthreadCheck(pthread_mutex_lock(&global_lock), "lock");
    global_sum += value;
    OsPthreadCheck(pthread_mutex_unlock(&global_lock), "unlock");

    OsThreadOutput* output = malloc(sizeof(OsThreadOutput));
    check_error(output != NULL, "malloc");
    output->sum = value;

    return output;
}

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    int n;
    scanf("%d", &n);
    check_error(n > 0, "n mora biti vece od 0");

    array = malloc(n*sizeof(int));
    check_error(array != NULL, "malloc");
    for(int i = 0; i < n; i++){
        scanf("%d", &array[i]);
    }

    pthread_t* tids = malloc(n*sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    OsThreadArg* data = malloc(n*sizeof(OsThreadArg));
    check_error(data != NULL, "malloc");

    OsPthreadCheck(pthread_mutex_init(&global_lock, NULL), "init");

    for(int i = 0; i < n; i++){
        data[i].pos = i;
        OsPthreadCheck(pthread_create(&tids[i], NULL, OsThreadFunc, &data[i]), "create");
    }

    for(int i = 0; i < n; i++){
        OsThreadOutput* data_output = NULL;
        OsPthreadCheck(pthread_join(tids[i], (void**)&data_output), "join");
        free(data_output);
    }

    OsPthreadCheck(pthread_mutex_destroy(&global_lock), "destroy");

    printf("%d\n", global_sum);

    free(array);
    free(tids);
    free(data);

    exit(EXIT_SUCCESS);
}
