// Program za svaki prosledjen fajl pokrece nit, odredjuje broj reci i na kraju
// ispisuje ukupan broj reci i naziv fajla sa najsvise reci. Muteks za sinhro.
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

#define OsPthreadCheck(pthreadErr, msg)\
    do{\
        int _pthreadErr = pthreadErr;\
        if(_pthreadErr > 0){\
            errno = _pthreadErr;\
            check_error(false, msg);\
        }\
    } while(0)

#define MAX_SIZE 256

typedef struct{
    char* fPath;
} OsThreadArg;

int global_words_count = 0;
int max_words = -1;
char* max_name = NULL;
pthread_mutex_t global_lock;

void* OsThreadFunc(void* args){

    OsThreadArg* arg = (OsThreadArg*)args;

    FILE* f = fopen(arg->fPath, "r");
    check_error(f != NULL, "fopen");

    int words_count = 0;
    char word[MAX_SIZE];
    while(fscanf(f, "%s", word) == 1){
        words_count++;
    }

    fclose(f);

    OsPthreadCheck(pthread_mutex_lock(&global_lock), "lock");
    if(words_count > max_words){
        max_words = words_count;
        max_name = arg->fPath;
    }
    global_words_count += words_count;
    OsPthreadCheck(pthread_mutex_unlock(&global_lock), "unlock");

    return NULL;
}

int main(int argc, char** argv){

    check_error(argc > 1, "Argumenti");

    int k = argc - 1;
    pthread_t* tids = malloc(k*sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    OsThreadArg* data = malloc(k*sizeof(OsThreadArg));
    check_error(data != NULL, "malloc");

    OsPthreadCheck(pthread_mutex_init(&global_lock, NULL), "init");

    for(int i = 0; i < k; i++){
        // Ova provera mozda mora mozda ne
        struct stat fInfo;
        check_error(stat(argv[i + 1], &fInfo) != -1, "stat");
        check_error(S_ISREG(fInfo.st_mode), "Nije regularan");
        data[i].fPath = argv[i + 1];
        OsPthreadCheck(pthread_create(&tids[i], NULL, OsThreadFunc, &data[i]), "create");
    }

    for(int i = 0; i < k; i++){
        OsPthreadCheck(pthread_join(tids[i], NULL), "join");
    }

    OsPthreadCheck(pthread_mutex_destroy(&global_lock), "destroy");

    char* tmp = strrchr(max_name, '/');
    char* max_real_name;
    if(tmp != NULL){
        max_real_name = tmp + 1;
    }
    else{
        max_real_name = max_name;
    }
    printf("%d %s\n", global_words_count, max_real_name);

    free(tids);
    free(data);

    exit(EXIT_SUCCESS);
}
