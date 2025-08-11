// Program ispisuje vrednosti svih promenjivih okruzenja koja pocinju sa w, gde
// se w sastoji samo od velikih slova sto treba proveriti
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

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

int all_caps(char* word){

    for(int i = 0; word[i]; i++){
        if(!(isalpha(word[i]) && isupper(word[i]))){
            return 0;
        }
    }

    return 1;
}

extern char** environ;

void print_env(char* word){

    int n = strlen(word);
    char str[n + 1];
    char** ep;
    for(ep = environ; *ep != NULL; ep++){
        // Kopiramo pocetak imena da bi znali da li pocinje time
        strncpy(str, *ep, n);
        // Ako pocinje trazimo prvo pojavljivanje =
        if(!strcmp(str, word)){
            char* tmp = strchr(*ep, '=');
            // Ispisavanje niske posle =
            printf("%s ", tmp + 1);
        }
    }

}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    char* word = argv[1];
    check_error(all_caps(word), "Los format");

    print_env(word);
    printf("\n");

    exit(EXIT_SUCCESS);
}
