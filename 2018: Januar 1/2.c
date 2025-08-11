// Program pokrece K niti u fajlu velicine N gde trazi sekvencu karaktera W,
// svaka nit broji broj pojavljivanja W u svojoj sekvenci i dodaje je na
// globalnu sumu, na kraju ispisati broj pojavljivanja W
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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
    char* fPath;
    char* word;
    int start;
    int len;
} OsThreadArg;

int global_sum = 0;
pthread_mutex_t global_lock;

void* OsThreadFunc(void* args){

    OsThreadArg* arg = (OsThreadArg*)args;

    int fd = open(arg->fPath, O_RDONLY);
    check_error(fd != -1, "open");

    // lseek pomera fd u odnosu na 2. argument, a SEEK_SET to gleda u odnosu
    // na offset bytes, tj postavlja fd na 2. argument
    check_error(lseek(fd, arg->start, SEEK_SET) != -1, "lseek");

    char buffer[arg->len + 1];
    check_error(read(fd, buffer, arg->len) != -1, "read");
    buffer[arg->len] = '\0';

    close(fd);

    int value = 0;
    char* s = buffer;
    while((s = strstr(s, arg->word)) != NULL){
        value++;
        s++;
    }

    osPthreadCheck(pthread_mutex_lock(&global_lock), "lock");
    global_sum += value;
    osPthreadCheck(pthread_mutex_unlock(&global_lock), "unlock");

    return NULL;
}

int main(int argc, char** argv){

    check_error(argc == 4, "Argumenti");

    char* W = argv[2];
    int K = atoi(argv[3]);
    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "stat");
    int N = fInfo.st_size;

    pthread_t* tids = malloc(K*sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    OsThreadArg* data = malloc(K*sizeof(OsThreadArg));
    check_error(data != NULL, "malloc");

    osPthreadCheck(pthread_mutex_init(&global_lock, NULL), "init");

    for(int i = 0; i < K; i++){
        data[i].fPath = argv[1];
        data[i].word = W;
        data[i].start = i*(N/K);
        data[i].len = N/K;
        osPthreadCheck(pthread_create(&tids[i], NULL, OsThreadFunc, &data[i]), "create");
    }

    for(int i = 0; i < K; i++){
        osPthreadCheck(pthread_join(tids[i], NULL), "join");
    }

    osPthreadCheck(pthread_mutex_destroy(&global_lock), "destroy");

    printf("%d\n", global_sum);

    free(tids);
    free(data);

    exit(EXIT_SUCCESS);
}

