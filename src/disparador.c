#include "disparador.h"
#include "pilha.h"
#include "formas.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct CarregadorImpl {
    int id;
    Pilha formas; // Pilha de Forma
};

struct DisparadorImpl {
    int id;
    double x, y;
    struct CarregadorImpl *cesq;
    struct CarregadorImpl *cdir;
    Forma posicao_disparo;
};


Carregador cria_carregador(int id) {
    struct CarregadorImpl *c = malloc(sizeof(struct CarregadorImpl));
    if (c) {
        c->id = id;
        c->formas = criaPilha();
    }
    return (Carregador)c;
}

void carregador_adiciona_forma(Carregador c, Forma f) {
    struct CarregadorImpl *impl = (struct CarregadorImpl*)c;
    if (impl && impl->formas) {
        pushPilha(impl->formas, f);
    }
}

bool carregador_vazio(Carregador c) {
    struct CarregadorImpl *impl = (struct CarregadorImpl*)c;
    if (!impl || !impl->formas) return true;
    return pilhaVazia(impl->formas);
}

Forma carregador_remover_forma(Carregador c) {
    if (carregador_vazio(c)) return NULL;
    struct CarregadorImpl *impl = (struct CarregadorImpl*)c;
    return popPilha(impl->formas);
}

int obter_id_carregador(Carregador c) {
    struct CarregadorImpl *impl = (struct CarregadorImpl*)c;
    return impl ? impl->id : -1;
}

void desaloca_carregador(Carregador c) {
    struct CarregadorImpl *impl = (struct CarregadorImpl*)c;
    if (impl) {
        if (impl->formas) {
            while (!pilhaVazia(impl->formas)) {
                popPilha(impl->formas);
            }
            desalocaPilha(impl->formas);
        }
        free(impl);
    }
}



Disparador cria_disparador(int id, double x, double y) {
    struct DisparadorImpl *d = malloc(sizeof(struct DisparadorImpl));
    if (d) {
        d->id = id;
        d->x = x;
        d->y = y;
        d->cesq = NULL;
        d->cdir = NULL;
        d->posicao_disparo = NULL;
    }
    return (Disparador)d;
}

void disparador_conecta_carregadores(Disparador d, Carregador cesq, Carregador cdir) {
    struct DisparadorImpl *impl = (struct DisparadorImpl*)d;
    if (impl) {
        if (cesq) impl->cesq = (struct CarregadorImpl*)cesq;
        if (cdir) impl->cdir = (struct CarregadorImpl*)cdir;
    }
}

Forma disparador_shift(Disparador d, char lado, int n) {
    struct DisparadorImpl *impl = (struct DisparadorImpl*)d;
    if (!impl) return NULL;

    for (int i = 0; i < n; i++) {
        if (lado == 'd') { 
            if (impl->posicao_disparo) {
                if (impl->cdir) carregador_adiciona_forma((Carregador)impl->cdir, impl->posicao_disparo);
                impl->posicao_disparo = NULL;
            }
            if (impl->cesq && !carregador_vazio((Carregador)impl->cesq)) {
                impl->posicao_disparo = carregador_remover_forma((Carregador)impl->cesq);
            }
        } else {
            if (impl->posicao_disparo) {
                if (impl->cesq) carregador_adiciona_forma((Carregador)impl->cesq, impl->posicao_disparo);
                impl->posicao_disparo = NULL;
            }
            if (impl->cdir && !carregador_vazio((Carregador)impl->cdir)) {
                impl->posicao_disparo = carregador_remover_forma((Carregador)impl->cdir);
            }
        }
    }
    return impl->posicao_disparo;
}

Forma disparador_obter_forma_disparo(Disparador d) {
    struct DisparadorImpl *impl = (struct DisparadorImpl*)d;
    return impl ? impl->posicao_disparo : NULL;
}

Forma disparador_remover_forma_disparo(Disparador d) {
    struct DisparadorImpl *impl = (struct DisparadorImpl*)d;
    if (!impl) return NULL;
    Forma f = impl->posicao_disparo;
    impl->posicao_disparo = NULL;
    return f;
}

int obter_id_disparador(Disparador d) {
    struct DisparadorImpl *impl = (struct DisparadorImpl*)d;
    return impl ? impl->id : -1;
}

void obter_posicao_disparador(Disparador d, double *x, double *y) {
    struct DisparadorImpl *impl = (struct DisparadorImpl*)d;
    if (impl) {
        *x = impl->x;
        *y = impl->y;
    }
}

void desaloca_disparador(Disparador d) {
    struct DisparadorImpl *impl = (struct DisparadorImpl*)d;
    if (impl) {
        free(impl);
    }
}
