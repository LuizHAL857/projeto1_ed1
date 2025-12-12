#include "text_style.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {

    char* ff;
    char* fw;
    int fs;

}TextStyle;


TEXTSTYLE criaTextStyle(char* ff, char* fw, int fs) {

    if(!ff){

        return NULL;
    }

    TextStyle *textstyle = malloc(sizeof(TextStyle));

    if(!textstyle){
        
        return NULL;
    }

    textstyle->ff = (char*) malloc(strlen(ff) + 1);
    if(!textstyle->ff) {

        free(textstyle);
    }

    strcpy(textstyle->ff, ff);


    textstyle->fw = (char*) malloc(strlen(fw) + 1);

    if(!textstyle->fw){

        free(textstyle->ff);
        free(textstyle);

    }

    strcpy(textstyle->fw, fw);

    textstyle->fs = fs;
    return textstyle;
}


void desalocaTextStyle(void* textstyle){

    if(!textstyle){

        return; 
    }

    TextStyle *ts = (TextStyle*)textstyle;

    free(ts->ff);
    free(ts->fw);
    free(ts);

}

char *getTextFF(void* textstyle) {


    if(!textstyle){

        return NULL;
    }

    return ((TextStyle*)textstyle)->ff;
} 

char *getTextFW(void* textstyle) {


    if(!textstyle){

        return NULL;
    }

    return ((TextStyle*)textstyle)->fw;
} 

int   getTextFS(void* textstyle) {


    if(!textstyle){

        return 0;
    }

    return ((TextStyle*)textstyle)->fs;
} 