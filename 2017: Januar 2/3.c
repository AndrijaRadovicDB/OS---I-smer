// Program u posebnim procesima vrsi operacije nad niskama, roditelj ucitava rec
// po rec i komandu, dete dobija rec i komandu i izvrsava je, duzina reci 63,
// komande: l - prebaci sva slova u mala, u - sva u velika, r - obrni nisku
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/wait.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

#define RD_END (0)
#define WR_END (1)
#define MAX_SIZE (64)

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    while(1){

        int par2cld[2];
        check_error(pipe(par2cld) != -1, "pipe");
        int cld2par[2];
        check_error(pipe(cld2par) != -1, "pipe");

        pid_t childPid = fork();
        check_error(childPid != -1, "fork");

        if(childPid > 0){ // Roditelj
            char word[MAX_SIZE];
            char command[MAX_SIZE];

            char data2send[2*MAX_SIZE];

            close(par2cld[RD_END]);
            close(cld2par[WR_END]);

            if(fscanf(stdin, "%s%s", word, command) != 2){
                break;
            }

            // memsend prvih 2*MAX_SIZE popunjava sa 0 u data2send
            memset(data2send, 0, 2*MAX_SIZE);

            if(strlen(command) > 1){
                printf("Inappropriate command\n");
                continue;
            }

            if(command[0] == 'l' || command[0] == 'u' || command[0] == 'r'){
                sprintf(data2send, "%s %s", word, command);
            }
            else{
                fprintf(stderr, "Wrong command\n");
                continue;
            }
        }
        else{ // Dete
            close(par2cld[WR_END]);
            close(cld2par[RD_END]);

            char data2rec[2*MAX_SIZE];
            memset(data2rec, 0, 2*MAX_SIZE);

            char data2send[2*MAX_SIZE];
            memset(data2send, 0, 2*MAX_SIZE);

            check_error(read(par2cld[RD_END], data2rec, 2*MAX_SIZE) != -1, "read");

            int position = (int)(strchr(data2rec, ' ') - data2rec);

            if(data2rec[position + 1] == 'l'){
                for(int i = 0; i < position; i++){
                    data2send[i] = tolower(data2rec[i]);
                }
            }
            else if(data2rec[position + 1] == 'u'){
                for(int i = 0; i < position; i++){
                    data2send[i] = toupper(data2rec[i]);
                }
            }
            else if(data2rec[position + 1] == 'r'){
                for(int i = 0; i < position; i++){
                    data2send[i] = data2rec[position - i - 1];
                }
            }

            check_error(write(cld2par[WR_END], data2send, 2*MAX_SIZE) != -1, "write");

            close(par2cld[RD_END]);
            close(cld2par[WR_END]);

            exit(EXIT_SUCCESS);
        }
    }

    check_error(wait(NULL) != -1, "wait");

    exit(EXIT_SUCCESS);
}
