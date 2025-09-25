#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "texto.h"

typedef struct {

    int id;
    float x;
    float y;
    char* txt;
    char *corB;
    char *corP;
    char ancora;
    char *ff;
    char *fw;
    char *fs;

}texto;


TEXTO criaTexto(int id, float x, float y, char *corB, char *corP, char ancora, char *txt, char *ff, char *fw, char *fs){

    texto* t = (texto*) malloc(sizeof(texto));

    t->id = id;
    t->x = x;
    t->y = y;
    t->ancora = ancora;

    t->corB =(char*)malloc(sizeof(char) * strlen(corB)+1);

    if(t->corB == NULL){

        printf("Erro de alocação");
        exit(1);
    }

    strcpy(t->corB, corB);

    t->corP =(char*)malloc(sizeof(char) * strlen(corP)+1);

    if(t->corP == NULL){

        printf("Erro de alocação");
        exit(1);
    }

    strcpy(t->corP, corP);

    t->txt =(char*)malloc(sizeof(char) * strlen(txt)+1);

    if(t->txt == NULL){

        printf("Erro de alocação");
        exit(1);
    }

    strcpy(t->txt, txt);

    t->ff =(char*)malloc(sizeof(char) * strlen(ff)+1);

    if(t->ff == NULL){

        printf("Erro de alocação");
        exit(1);
    }

    strcpy(t->ff, ff);

    t->fw =(char*)malloc(sizeof(char) * strlen(fw)+1);

    if(t->fw == NULL){

        printf("Erro de alocação");
        exit(1);
    }

    strcpy(t->fw, fw);

    t->fs =(char*)malloc(sizeof(char) * strlen(fs)+1);

    if(t->fs == NULL){

        printf("Erro de alocação");
        exit(1);
    }

    strcpy(t->fs, fs);

    return t;
}

int getIDTexto(TEXTO t){

    texto *tTemp = ((texto*)t);

    return tTemp->id;
}

float getXTexto(TEXTO t){

    texto *tTemp = ((texto*)t);

    return tTemp->x;
}

float getYTexto(TEXTO t){

    texto *tTemp = ((texto*)t);

    return tTemp->y;
}

char getAncoraTexto(TEXTO t){

    texto *tTemp = ((texto*)t);

    return tTemp->ancora;
}

char* getCorBTexto(TEXTO t){

    texto *tTemp = ((texto*)t);

    return tTemp->corB;
}

char* getCorPTexto(TEXTO t){

    texto *tTemp = ((texto*)t);

    return tTemp->corP;
}

char* getTxtTexto(TEXTO t){


    texto *tTemp = ((texto*)t);

    return tTemp->txt;
}

char* getFFTexto(TEXTO t){

    texto *tTemp = ((texto*)t);

    return tTemp->ff;
}

char* getFWTexto(TEXTO t){

    texto *tTemp = ((texto*)t);

    return tTemp->fw;
}

char* getFSTexto(TEXTO t){

    texto *tTemp = ((texto*)t);

    return tTemp->fs;
}


float getComprimentoTexto(TEXTO t){

    texto *tTemp = ((texto*)t);

    float cl = strlen(tTemp->txt) * 10;

    return cl;
}


float getX1Texto(TEXTO t){

    texto *tTemp = ((texto*)t);

    float x1;
    float cl = getComprimentoTexto(t);



    switch(tTemp->ancora){

        case 'i': x1 = tTemp->x;
        break;

        case 'f': x1 = tTemp->x - cl;
        break;

        case 'm': x1 = tTemp->x - cl/2;
        break;

        default:
        printf("Ancora invalida");

    }

    return x1;
}

float getX2Texto(TEXTO t){

    texto *tTemp = ((texto*)t);

    float x2;
    float cl = getComprimentoTexto(t);



    switch(tTemp->ancora){

        case 'i': x2 = tTemp->x +cl;
        break;

        case 'f': x2 = tTemp->x;
        break;

        case 'm': x2 = tTemp->x + cl/2;

    }

    return x2;

}

float calculoAreaTexto(TEXTO t){

    texto *tTemp = ((texto*)t);

    float area = 20 * strlen(tTemp->txt);

    return area;

}

void setIDTexto(TEXTO t, int id){

    texto *tTemp = ((texto*)t);

    tTemp->id = id;
}

void setXTexto(TEXTO t, float x){

    texto *tTemp = ((texto*)t);

    tTemp->x = x;
}

void setYTexto(TEXTO t, float y){

    texto *tTemp = ((texto*)t);

    tTemp->y = y;
}

void setCorBTexto(TEXTO t, char *corB){

    texto *tTemp = ((texto*)t);

    free(tTemp->corB);

    tTemp->corB = (char*)malloc(sizeof(char)* strlen(corB)+1);

    if(tTemp->corB == NULL){


        printf("Erro de alocação");
        exit(1);
    }
    
    strcpy(tTemp->corB, corB);
}

void setCorPTexto(TEXTO t, char *corP){

    texto *tTemp = ((texto*)t);

    free(tTemp->corP);

    tTemp->corP = (char*)malloc(sizeof(char)* strlen(corP)+1);

    if(tTemp->corP == NULL){


        printf("Erro de alocação");
        exit(1);
    }
    
    strcpy(tTemp->corP, corP);
}


void setAncoraTexto(TEXTO t, char ancora){

    texto *tTemp = ((texto*)t);

    tTemp->ancora = ancora;
}

void setTxtTexto(TEXTO t, char *txt){

    texto *tTemp = ((texto*)t);

    free(tTemp->txt);

    tTemp->txt = (char*)malloc(sizeof(char)* strlen(txt)+1);

    if(tTemp->txt == NULL){


        printf("Erro de alocação");
        exit(1);
    }
    
    strcpy(tTemp->txt, txt);
}

void setFFTexto(TEXTO t, char* ff) {

    texto* tTemp = (texto*)t;
    free(tTemp->ff);

    tTemp->ff = (char*)malloc(strlen(ff) + 1);
    if (tTemp->ff == NULL) {
        printf("Erro ao alocar memória \n");
        exit(1);
    }

    strcpy(tTemp->ff, ff);
}

void setFWTexto(TEXTO t, char* fw) {

    texto* tTemp = (texto*)t;
    free(tTemp->fw);

    tTemp->fw = (char*)malloc(strlen(fw) + 1);

    if (tTemp->fw == NULL) {
        printf("Erro ao alocar memória ");
        exit(1);
    }

    strcpy(tTemp->fw, fw);
}

void setFSTexto(TEXTO t, char* fs) {

    texto* tTemp = (texto*)t;
    free(tTemp->fs);

    tTemp->fs = (char*)malloc(strlen(fs) + 1);

    if (tTemp->fs == NULL) {
        printf("Erro ao alocar memória \n");
        exit(1);
    }

    strcpy(tTemp->fs, fs);
}

void desalocaTexto(TEXTO t){

    texto* tTemp = ((texto*)t);

    free(tTemp->txt);
    free(tTemp->corB);
    free(tTemp->corP);
    free(tTemp->ff);
    free(tTemp->fw);
    free(tTemp->fs);

    free(tTemp);

}