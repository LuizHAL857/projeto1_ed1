#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "retangulo.h"

typedef struct {

    
    int id;
    float x;
    float y;
    float altura;
    float largura;
    char *corP;
    char *corB;
  

}retangulo;


RETANGULO criaRetangulo(int id, float x, float y, float altura, float largura, char *corB, char *corP){

    
    retangulo *r = (retangulo*) malloc(sizeof(retangulo));

    if(r == NULL){
       
         printf("Erro na alocação de memória\n");
        
        exit(1);
        }

    r->id = id;
    r->x = x;
    r->y = y;
    r->altura = altura;
    r->largura = largura;
    


    r->corB =(char*)malloc(sizeof(char) * strlen(corB) +1);
    r->corP =(char*)malloc(sizeof(char) * strlen(corP) +1);

    if(r->corB == NULL || r->corP == NULL){

        printf("Erro de alocação");
        exit(1);
    }

    strcpy(r->corB, corB);
    strcpy(r->corP, corP);

    return r;

}

int getIDRetangulo(RETANGULO r){

  
    retangulo *rTemp = ((retangulo*)r);


    return rTemp->id;
}

float getXRetangulo(RETANGULO r){

    retangulo *rTemp = ((retangulo*)r);

    return rTemp->x;
}

float getYRetangulo(RETANGULO r){

    retangulo *rTemp = ((retangulo*)r);

    return rTemp->y;
}

float getAlturaRetangulo(RETANGULO r){

    retangulo *rTemp = ((retangulo*)r);

    return rTemp->altura;
}

float getLarguraRetangulo(RETANGULO r){

    retangulo *rTemp = ((retangulo*)r);

    return rTemp->largura;
}

char*  getCorBRetangulo(RETANGULO r){

    retangulo *rTemp = ((retangulo*)r);

    return rTemp->corB;
}

char*  getCorPRetangulo(RETANGULO r){

    retangulo *rTemp = ((retangulo*)r);

    return rTemp->corP;
}


void setIDRetangulo(RETANGULO r, int id){

    retangulo *rTemp = ((retangulo*)r);

    rTemp->id = id;

}

void setXRetangulo(RETANGULO r, float x){

    retangulo *rTemp = ((retangulo*)r);

    rTemp->x = x;
}

void setYRetangulo(RETANGULO r, float y){

    retangulo *rTemp = ((retangulo*)r);

    rTemp->y = y;
}

void setAlturaRetangulo(RETANGULO r, float altura){

    retangulo *rTemp = ((retangulo*)r);

    rTemp->altura = altura;
}

void setLarguraRetangulo(RETANGULO r, float largura){

    retangulo *rTemp = ((retangulo*)r);

    rTemp->largura = largura;
}

void setCorBRetangulo(RETANGULO r, char* corB){

    retangulo *rTemp = ((retangulo*)r);

    free(rTemp->corB);

    rTemp->corB = (char*)malloc(sizeof(char) * strlen(corB) + 1);

    if(rTemp->corB == NULL){

        printf("Erro de alocação");
        exit(1);
    }

    strcpy(rTemp->corB, corB);
}

void setCorPRetangulo(RETANGULO r, char* corP){

    retangulo *rTemp = ((retangulo*)r);

    free(rTemp->corP);

    rTemp->corP = (char*)malloc(sizeof(char) * strlen(corP) + 1);

    if(rTemp->corP == NULL){

        printf("Erro de alocação");
        exit(1);
    }

    strcpy(rTemp->corP, corP);
}


float calculoAreaRetangulo(RETANGULO r){

    retangulo *rTemp = ((retangulo*)r);

    float area = (rTemp->largura) * (rTemp->altura);

    return area;

}

void desalocarRetangulo(RETANGULO r){

    retangulo *rTemp = ((retangulo*)r);

    free(rTemp->corB);
    free(rTemp->corP);
    free(rTemp);

}

