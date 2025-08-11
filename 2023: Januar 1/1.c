// Program prima putanje do fajlova i ispisuje korisnicko ime vlasnika, velicinu
// i naziv najveceg regularnog fajla. Ako ih je vise ispisati prvi. Ako fajl na
// putanji ne postoji on se preskace. Ako nijedna putanja nije ispravna ispisati
// neuspeh.
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <pwd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

int main(int argc, char** argv){

    check_error(argc > 1, "Argumenti");

    int max_size = -1;
    char* max_name = NULL;
    struct passwd* usr = NULL;
    for(int i = 0; i < argc; i++){
        struct stat fInfo;
        int ret_val = stat(argv[i + 1], &fInfo);
        if(ret_val != -1 && S_ISREG(fInfo.st_mode)){
            if(fInfo.st_size > max_size){
                max_size = fInfo.st_size;
                usr = getpwuid(fInfo.st_uid);
                check_error(usr != NULL, "getpwuid");
                max_name = argv[i + 1];
            }
        }
    }

    if(max_size == -1){
        printf("neuspeh\n");
    }
    else{
        printf("%s %d %s\n", usr->pw_name, max_size, max_name);
    }

    exit(EXIT_SUCCESS);
}
