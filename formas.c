
#include "formas.h"
#include "circulo.h"
#include "retangulo.h"
#include "linha.h"
#include "texto.h"
#include "text_style.h"
#include "cor_complementar.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


char* copia_string(const char* s) {
    if (s == NULL) return NULL;
    size_t len = strlen(s) + 1;
    char* copy = malloc(len);
    if (copy) {
        memcpy(copy, s, len);
    }
    return copy;
}


typedef struct {
    TipoForma tipo;
    void* data;
} FormaImpl;

Forma cria_forma(TipoForma tipo, void* data) {
    FormaImpl* f = malloc(sizeof(FormaImpl));
    if (f == NULL) return NULL;
    f->tipo = tipo;
    f->data = data;
    return (Forma)f;
}

TipoForma get_tipo_forma(Forma f) {
    if (f == NULL) return -1;
    return ((FormaImpl*)f)->tipo;
}

void* get_data_forma(Forma f) {
    if (f == NULL) return NULL;
    return ((FormaImpl*)f)->data;
}

void desaloca_forma(Forma f) {
    if (f != NULL) {
        free(f);
    }
}

Forma clona_forma(Forma forma) {
    if (forma == NULL) return NULL;

    FormaImpl* f_orig = (FormaImpl*)forma;
    void* novo_data = NULL;

    switch (f_orig->tipo) {
        case CIRCLE: {
            CIRCULO c = (CIRCULO)f_orig->data;
            novo_data = criaCirculo(
                getIDCirculo(c),
                getXCirculo(c),
                getYCirculo(c),
                getRaioCirculo(c),
                getCorPCirculo(c),
                getCorBCirculo(c)
            );
            break;
        }
        case RECTANGLE: {
            RETANGULO r = (RETANGULO)f_orig->data;
            novo_data = criaRetangulo(
                getIDRetangulo(r),
                getXRetangulo(r),
                getYRetangulo(r),
                getAlturaRetangulo(r),
                getLarguraRetangulo(r),
                getCorBRetangulo(r),
                getCorPRetangulo(r)
            );
            break;
        }
        case LINE: {
            LINHA l = (LINHA)f_orig->data;
            novo_data = criaLinha(
                getIDLinha(l),
                getX1Linha(l),
                getY1Linha(l),
                getX2Linha(l),
                getY2Linha(l),
                getCorLinha(l)
            );
            break;
        }
        case TEXT: {
            TEXTO t = (TEXTO)f_orig->data;
            novo_data = criaTexto(
                getIDTexto(t),
                getXTexto(t),
                getYTexto(t),
                getCorBTexto(t),
                getCorPTexto(t),
                getAncoraTexto(t),
                getTxtTexto(t)
            );
            break;
        }
        case TEXT_STYLE: {
            TEXTSTYLE ts = (TEXTSTYLE)f_orig->data;
            novo_data = criaTextStyle(
                getTextFF(ts),
                getTextFW(ts),
                getTextFS(ts)
            );
            break;
        }
        default:
            return NULL;
    }

    if (novo_data == NULL) return NULL;

    return cria_forma(f_orig->tipo, novo_data);
}

float calcula_area_forma(Forma forma) {
    if (forma == NULL) return 0.0;
    
    FormaImpl* f = (FormaImpl*)forma;
    if (f->data == NULL) return 0.0;

    switch (f->tipo) {
        case CIRCLE:
            return calculoAreaCirculo((CIRCULO)f->data);
        case RECTANGLE:
            return calculoAreaRetangulo((RETANGULO)f->data);
        case LINE:
            return calculoAreaLinha((LINHA)f->data);
        case TEXT:
            return calculoAreaTexto((TEXTO)f->data);
        case TEXT_STYLE:
            return 0.0;
        default:
            return 0.0;
    }
}



void definir_posicao_forma(Forma f, double x, double y) {
    TipoForma t = get_tipo_forma(f);
    void *d = get_data_forma(f);
    if (t == CIRCLE) { setXCirculo(d, x); setYCirculo(d, y); }
    else if (t == RECTANGLE) { setXRetangulo(d, x); setYRetangulo(d, y); }
    else if (t == TEXT) { setXTexto(d, x); setYTexto(d, y); }
    else if (t == LINE) {
        LINHA l = (LINHA)d;
        double dx = getX2Linha(l) - getX1Linha(l);
        double dy = getY2Linha(l) - getY1Linha(l);
        setX1Linha(l, x); setY1Linha(l, y);
        setX2Linha(l, x+dx); setY2Linha(l, y+dy);
    }
}

void obter_posicao_forma(Forma f, double *x, double *y) {
    TipoForma t = get_tipo_forma(f);
    void *d = get_data_forma(f);
    if (t == CIRCLE) { *x = getXCirculo(d); *y = getYCirculo(d); }
    
    else if (t == RECTANGLE) { *x = getXRetangulo(d); *y = getYRetangulo(d); }

    else if (t == TEXT) { *x = getXTexto(d); *y = getYTexto(d); }
    else if (t == LINE) { *x = getX1Linha(d); *y = getY1Linha(d); }
}

char* obter_cor_preenchimento(Forma f) {

    TipoForma t = get_tipo_forma(f);

    void *d = get_data_forma(f);
    if (t == CIRCLE) return copia_string(getCorPCirculo(d));

    if (t == RECTANGLE) return copia_string(getCorPRetangulo(d));

    if (t == TEXT) return copia_string(getCorPTexto(d));
    
    if (t == LINE) return copia_string(getCorLinha(d)); 

    return copia_string("black");
}

void definir_cor_borda(Forma f, char *cor) {
    TipoForma t = get_tipo_forma(f);
    void *d = get_data_forma(f);
    if (t == CIRCLE) setCorBCirculo(d, cor);
    else if (t == RECTANGLE) setCorBRetangulo(d, cor);
    else if (t == TEXT) setCorBTexto(d, cor);
    else if (t == LINE) setCorLinha(d, cor);
}

Forma clona_e_troca_cores(Forma f) {
    Forma clone = clona_forma(f);
    TipoForma t = get_tipo_forma(clone);
    void *d = get_data_forma(clone);
    
    if (t == CIRCLE) {
        char *cb = getCorBCirculo(d);
        char *cp = getCorPCirculo(d);
        char *novo_cb = copia_string(cp);
        char *novo_cp = copia_string(cb);
        setCorBCirculo(d, novo_cb);
        setCorPCirculo(d, novo_cp);
        free(novo_cb); free(novo_cp);
    } else if (t == RECTANGLE) {
        char *cb = getCorBRetangulo(d);
        char *cp = getCorPRetangulo(d);
        char *novo_cb = copia_string(cp);
        char *novo_cp = copia_string(cb);
        setCorBRetangulo(d, novo_cb);
        setCorPRetangulo(d, novo_cp);
        free(novo_cb); free(novo_cp);
    } else if (t == TEXT) {
        char *cb = getCorBTexto(d);
        char *cp = getCorPTexto(d);
        char *novo_cb = copia_string(cp);
        char *novo_cp = copia_string(cb);
        setCorBTexto(d, novo_cb);
        setCorPTexto(d, novo_cp);
        free(novo_cb); free(novo_cp);
    } else if (t == LINE) {
        char *c = getCorLinha(d);
        char *comp = cor_complementar(c);
        setCorLinha(d, comp);
        free(comp);
    }
    return clone;
}

BoundingBox obter_bounding_box(Forma f) {

    BoundingBox caixa = {0,0,0,0};
    TipoForma t = get_tipo_forma(f);
    void *d = get_data_forma(f);
    
    if (t == CIRCLE) {


        double r = getRaioCirculo(d);
        double x = getXCirculo(d);
        double y = getYCirculo(d);
        caixa.minX = x - r; caixa.maxX = x + r;
        caixa.minY = y - r; caixa.maxY = y + r;
    } else if (t == RECTANGLE) {


        double x = getXRetangulo(d);
        double y = getYRetangulo(d);
        double w = getLarguraRetangulo(d);
        double h = getAlturaRetangulo(d);
        caixa.minX = x; caixa.maxX = x + w;
        caixa.minY = y; caixa.maxY = y + h;
    } else if (t == LINE) {
        double x1 = getX1Linha(d);
        double y1 = getY1Linha(d);
        double x2 = getX2Linha(d);
        double y2 = getY2Linha(d);
        caixa.minX = (x1 < x2) ? x1 : x2;
        caixa.maxX = (x1 > x2) ? x1 : x2;
        caixa.minY = (y1 < y2) ? y1 : y2;
        caixa.maxY = (y1 > y2) ? y1 : y2;
        
        
        caixa.minX -= 1; caixa.maxX += 1;
        caixa.minY -= 1; caixa.maxY += 1;
    } else if (t == TEXT) {
        double x1 = getX1Texto(d);
        double x2 = getX2Texto(d);
        double y = getYTexto(d);
        
        caixa.minX = (x1 < x2) ? x1 : x2;
        caixa.maxX = (x1 > x2) ? x1 : x2;
        caixa.minY = y; 
        caixa.maxY = y; 
    }
    return caixa;
}

int verifica_sobreposicao(Forma f1, Forma f2) {
    BoundingBox b1 = obter_bounding_box(f1);
    BoundingBox b2 = obter_bounding_box(f2);
    
    if (b1.maxX < b2.minX || b2.maxX < b1.minX || 
        b1.maxY < b2.minY || b2.maxY < b1.minY) return 0;
    return 1;
}



const char* tipo_forma_to_string(TipoForma t) {
    switch(t) {
        case CIRCLE: return "circulo";
        case RECTANGLE: return "retangulo";
        case LINE: return "linha";
        case TEXT: return "texto";
        default: return "desconhecido";
    }
}

