#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "linha.h"

typedef struct {

    int id;
    float x1;
    float y1;
    float x2;
    float y2;
    char* cor;

    
}linha;

LINHA criaLinha(int id, float x1, float y1, float x2, float y2, char* cor){

    linha *l = (linha*) malloc(sizeof(linha));

    if(l == NULL){

        printf("Erro na alocação de memória\n");
        
        exit(1);
    }

    l->id = id;
    l->x1 = x1;
    l->y1 = y1;
    l->x2 = x2;
    l->y2 = y2;

    l->cor = (char*)malloc(sizeof(char) * strlen(cor) +1);

    if(l->cor == NULL){

        printf("Erro na alocação de memória\n");
        
        exit(1);

    }

    strcpy(l->cor, cor);

    return l;
}

int getIDLinha(LINHA l){

    linha *lTemp = ((linha*)l);

    return lTemp->id;

}

float getX1Linha(LINHA l){

    linha *lTemp = ((linha*)l);

    return lTemp->x1;
}

float getX2Linha(LINHA l){

    linha *lTemp = ((linha*)l);

    return lTemp->x2;
}

float getY1Linha(LINHA l){

    linha *lTemp = ((linha*)l);

    return lTemp->y1;
}

float getY2Linha(LINHA l){

    linha *lTemp = ((linha*)l);

    return lTemp->y2;
}

char* getCorLinha(LINHA l){

    linha *lTemp = ((linha*)l);

    return lTemp->cor;
}

void setIDLinha(LINHA l, int id){

    linha *lTemp = ((linha*)l);

    lTemp->id = id;
}

void setX1Linha(LINHA l, float x1){

    linha *lTemp = ((linha*)l);

    lTemp->x1 = x1;
}

void setY1Linha(LINHA l, float y1){

    linha *lTemp = ((linha*)l);

    lTemp->y1 = y1;
}

void setX2Linha(LINHA l, float x2){

    linha *lTemp = ((linha*)l);

    lTemp->x2 = x2;
}

void setY2Linha(LINHA l, float y2){

    linha *lTemp = ((linha*)l);

    lTemp->y2 = y2;
}

void setCorLinha(LINHA l, char* cor){

    linha *lTemp = ((linha*)l);

    free(lTemp->cor);

    lTemp->cor = (char*)malloc(sizeof(char) * strlen(cor)+1);

    if(lTemp->cor == NULL){

        printf("Erro na alocação de memória\n");
        
        exit(1);
    }

    strcpy(lTemp->cor, cor);

}

float calculoAreaLinha(LINHA l){

    linha *lTemp = ((linha*)l);
    float dx = lTemp->x1 - lTemp->x2;
    float dy = lTemp->y1 - lTemp->y2;

    float area = 2 * sqrt(pow(dx, 2) + pow(dy, 2));
    
    return area;
}

void desalocaLinha(LINHA l){

    linha *lTemp = ((linha*)l);

    free(lTemp->cor);
    free(lTemp);

}