#include "trata_argumentos.h"

#include <stdio.h>
#include <string.h>



char* get_option(int argc, char *argv[], char* option_name){

    char option_format[64];

    snprintf(option_format, sizeof(option_format), "-%s", option_name);

    for(int i = 1; i < argc -1 ; ++i) {

        if(strcmp(argv[i], option_format) == 0) {

            return argv[i + 1];

        }
    }
 return NULL;

}