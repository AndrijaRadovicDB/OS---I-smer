// Program za svaki prosledjen fajl ispisuje ime korisnika koji je vlasnik i sve
// grupe kojima on pripada, ako neki fajl ne postoji ispisati odgovarajucu poruku
#define _GNU_SOURCE
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <pwd.h>
#include <grp.h>

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

    for(int i = 1; i < argc; i++){
        struct stat fInfo;
        int ret_val = stat(argv[i], &fInfo);
        if(ret_val == -1){
            printf("%s: nepostojeci fajl\n", argv[i]);
        }
        else{
            struct passwd* user = getpwuid(fInfo.st_uid);
            check_error(user != NULL, "getpwuid");
            printf("%s: ", user->pw_name);

            // Maksimalni broj grupa
            int group_num = 100;
            gid_t* groups = malloc(group_num*sizeof(*groups));
            check_error(groups != NULL, "malloc");
            // getgrouplist nam vraca listu grupa
            check_error(getgrouplist(user->pw_name, user->pw_gid, groups, &group_num) != -1, "getgrouplist");
            for(int i = 0; i < group_num; i++){
                struct group* grp = getgrgid(groups[i]);
                printf("%s ", grp->gr_name);
            }
            printf("\n");

            free(groups);

        }
    }

    exit(EXIT_SUCCESS);
}
