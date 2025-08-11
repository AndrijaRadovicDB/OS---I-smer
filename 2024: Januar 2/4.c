// Program prima putanju do direktorijuma koju treba da nadgleda. Za svaki fajl
// koji je premesten (IN_MOVED_TO) u zadati direktorijum potrebno je ispisati
// sadrzaj na stout. Za nadgledanje koristi se inotify.
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/inotify.h> // man 7 inotify

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

// NAME_MAX je iz limits.h
#define MAX_SIZE (10*(sizeof(struct inotify_event) + NAME_MAX + 1))

char* dir_name = NULL;

// Dogadjaji su serijalizovani pa ih moramo izvlaciti iz bafera 1 po 1
void proccess_events(char* buffer, ssize_t bytes_read){

    char* p = NULL;
    for(p = buffer; p < buffer + bytes_read; ){
        struct inotify_event* event = (struct inotify_event*)p;
        if(event->mask & IN_MOVED_TO){
            char* fPath = calloc(strlen(dir_name) + strlen(event->name) + 2, sizeof(char));
            check_error(fPath != NULL, "calloc");
            strcat(fPath, dir_name);
            strcat(fPath, "/");
            strcat(fPath, event->name);
            printf("%s\n", fPath);

            FILE* f = fopen(fPath, "r");
            check_error(f != NULL, "fopen");

            char* line = NULL;
            size_t len = 0;
            while(getline(&line, &len, f) > 0){
                fprintf(stdout, "%s", line);
            }

            free(fPath);
            free(line);
            fclose(f);
        }
        p += sizeof(struct inotify_event) + event->len;
    }
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "stat");
    check_error(S_ISDIR(fInfo.st_mode), "Nije dir");

    dir_name = argv[1];

    // Incijalizujemo instancu
    int inotifyFd = inotify_init();
    check_error(inotifyFd != -1, "inotify_init");
    // IN_MOVED_TO nas zanima pa njega dodajemo
    int wd = inotify_add_watch(inotifyFd, argv[1], IN_MOVED_TO);
    check_error(wd != -1, "inotify_add_watch");

    char buffer[MAX_SIZE];
    for(;;){
        ssize_t bytes_read = read(inotifyFd, buffer, MAX_SIZE);
        check_error(bytes_read != -1, "read");
        proccess_events(buffer, bytes_read);
    }

    exit(EXIT_SUCCESS);
}
