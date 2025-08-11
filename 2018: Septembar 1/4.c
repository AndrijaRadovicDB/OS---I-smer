// Program motri sve FIFO fajlove preko poll API i sve dok u njima ima podataka
// cita ih. Broji broj 'a'. Na kraju ispisuje ime FIFO fajla koji ima najvise 'a'
// kao i broj pojavljivanja 'a'
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <poll.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>

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

    for(int i = 0; i < numFds; i++){
        int fd = open(argv[i + 1], O_RDONLY | O_NONBLOCK);
        check_error(fd != -1, "open");
        fds[i].fd = fd;
        fds[i].events = POLLIN | POLLERR | POLLHUP;
        fds[i].revents = 0;
    }

    int actFds = numFds;
    int max_a = -1;
    char* name;

    while(actFds > 0){
        check_error(poll(fds, numFds, -1) != -1, "poll");
        for(int i = 0; i < numFds; i++){
            if(fds[i].revents & POLLIN){
                int num_a = 0;
                char c;
                while(read(fds[i].fd, &c, 1) > 0){
                    if(c == 'a'){
                        num_a++;
                    }
                }

                if(num_a > max_a){
                    max_a = num_a;
                    name = argv[i + 1];
                }
            }
            else if(fds[i].revents & (POLLERR | POLLHUP)){
                close(fds[i].fd);
                fds[i].fd = -1;
                fds[i].events = 0;
                actFds--;
            }
            fds[i].revents = 0;
        }
    }
    printf("%s %d\n", name, max_a);

    free(fds);

    exit(EXIT_SUCCESS);
}
