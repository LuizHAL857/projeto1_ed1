#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "circulo.h"

typedef struct {

    int id;
    float x;
    float y;
    float raio;
    char *corP;
    char *corB;

}circulo;


CIRCULO criaCirculo(int id, float x, float y, float raio, char *corP, char *corB){

    circulo *c = (circulo*) malloc(sizeof(circulo));

    if(c == NULL){

        printf("Erro na alocação de memória\n");
        
        exit(1);
    }

    c->id = id;
    c->x = x;
    c->y = y;
    c->raio = raio;
    
    c->corB = (char*) malloc(sizeof(char)*strlen(corB) + 1);
    if(c->corB == NULL){

        printf("Erro de alocação de memória");
        exit(1);
    }

    c->corP = (char*) malloc(sizeof(char)*strlen(corP) + 1);
    if(c->corP == NULL){

        printf("Erro de alocação de memória");
        exit(1);
    }

    strcpy(c->corB, corB);
    strcpy(c->corP, corP);

    return c;
}

int getIDCirculo(CIRCULO c){

    circulo *cTemp = ((circulo*)c);

    return cTemp->id;
}

float getXCirculo(CIRCULO c){

    circulo *cTemp = ((circulo*)c);

    return cTemp->x;
}

float getYCirculo(CIRCULO c){

    circulo *cTemp = ((circulo*)c);

    return cTemp->y;
}

float getRaioCirculo(CIRCULO c){

    circulo *cTemp = ((circulo*)c);

    return cTemp->raio;
}

char* getCorBCirculo(CIRCULO c){

    circulo *cTemp = ((circulo*)c);

    return cTemp->corB;
}

char* getCorPCirculo(CIRCULO c){

    circulo *cTemp = ((circulo*)c);

    return cTemp->corP;
}

float calculoAreaCirculo(CIRCULO c){

    circulo *cTemp = ((circulo*)c);
    
    float area = (cTemp->raio) * (cTemp->raio) * 3.14;

    return area;
}

void setIDCirculo(CIRCULO c, int id){

    circulo *cTemp = ((circulo*)c);

    cTemp->id = id;

}

void setXCirculo(CIRCULO c, float x){

    circulo *cTemp = ((circulo*)c);

    cTemp->x = x;

}

void setYCirculo(CIRCULO c, float y){

    circulo *cTemp = ((circulo*)c);

    cTemp->y = y;

}

void setRaioCirculo(CIRCULO c, float raio){

    circulo *cTemp = ((circulo*)c);

    cTemp->raio = raio;

}

void setCorBCirculo(CIRCULO c, char *corB){

    circulo *cTemp = ((circulo*)c);
    free(cTemp->corB);

    cTemp->corB = (char*)malloc(sizeof(char) * strlen(corB) + 1);

    if(cTemp->corB == NULL){

        printf("Erro de alocação");
        exit(1);
    }

    strcpy(cTemp->corB, corB);
}

void setCorPCirculo(CIRCULO c, char *corP){

    circulo *cTemp = ((circulo*)c);
    free(cTemp->corP);

    cTemp->corP = (char*)malloc(sizeof(char) * strlen(corP) + 1);

    if(cTemp->corP == NULL){

        printf("Erro de alocação");
        exit(1);
    }

    strcpy(cTemp->corP, corP);
}

void desalocaCirculo(CIRCULO c){

    circulo *cTemp = ((circulo*)c);
    free(cTemp->corB);
    free(cTemp->corP);
    free(cTemp);

}