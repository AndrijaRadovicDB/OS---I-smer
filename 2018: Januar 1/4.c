// Program dobija FIFO fajlove, cita iz njih karaktere i sve ih istovremeno prati
// preko poll, ispisati naziv FIFO fajla iz kog je procitano najvise karaktera
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <poll.h>

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

    int numFds = argc - 1;
    struct pollfd* fds = malloc(numFds*sizeof(struct pollfd));
    check_error(fds != NULL, "malloc");
    int* array_count = malloc(numFds*sizeof(int));
    check_error(array_count != NULL, "malloc");

    // Postavljanje fajlova
    for(int i = 0; i < numFds; i++){
        int fd = open(argv[i + 1], O_RDONLY | O_NONBLOCK);
        check_error(fd != -1, "open");
        fds[i].fd = fd;
        fds[i].events = POLLIN | POLLERR | POLLHUP;
        fds[i].revents = 0;
    }

    int actFds = numFds;
    int max_ch = 0;
    char* name;
    char buffer[4096];

    while(actFds){
        int retVal = poll(fds, numFds, -1);
        check_error(retVal != -1, "poll");
        for(int i = 0; i < numFds; i++){
            array_count[i] = 0;
            // POLLIN - postoje podaci za citanje
            if(fds[i].revents & POLLIN){
                int read_bytes = 0;
                while((read_bytes = read(fds[i].fd, buffer, 4096)) > 0){
                    array_count[i] += read_bytes;
                }
                if(read_bytes == -1){
                    // EAGAIN - neko drugi ima pristup fajlu
                    if(errno != EAGAIN){
                        check_error(0, "read");
                    }
                }
                if(array_count[i] > max_ch){
                    max_ch = array_count[i];
                    name = argv[i + 1];
                }
                fds[i].revents = 0;
            }
            // U slucaju greske ili prekidanja veze smanjujemo broj aktivnih fd
            // vracamo sve na 0, a fd na -1
            else if(fds[i].revents & (POLLERR | POLLHUP)){
                close(fds[i].fd);
                fds[i].fd = -1;
                fds[i].events = 0;
                fds[i].revents = 0;
                actFds--;
            }
        }
    }

    printf("%s\n", name);

    free(fds);
    free(array_count);

    exit(EXIT_SUCCESS);
}
