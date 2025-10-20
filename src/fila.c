#include "fila.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct NoFila {
    void *conteudo;
    struct NoFila *prox;
} NoFila;

// Estrutura interna da fila (opaca externamente)
typedef struct stFila {
    NoFila *inicio;
    NoFila *fim;
    int tamanho;
} stFila;


Fila criaFila() {
    stFila *fila = (stFila *)malloc(sizeof(stFila));
    if (!fila) {
        printf("[ERRO] Falha ao alocar memória para a fila.\n");
        return NULL;
    }

    fila->inicio = NULL;
    fila->fim = NULL;
    fila->tamanho = 0;
    return (Fila)fila;
}


bool enqueueFila(Fila fila, void *conteudo) {
    if (!fila) return false;

    stFila *f = (stFila *)fila;

    NoFila *novo = (NoFila *)malloc(sizeof(NoFila));
    if (!novo) {
        printf("[ERRO] Falha ao alocar memória para o novo nó da fila.\n");
        return false;
    }

    novo->conteudo = conteudo;
    novo->prox = NULL;

    if (f->fim) {
        f->fim->prox = novo;
    } else {
        f->inicio = novo;
    }

    f->fim = novo;
    f->tamanho++;
    return true;
}


void* dequeueFila(Fila fila) {
    if (!fila) return NULL;

    stFila *f = (stFila *)fila;
    if (!f->inicio) return NULL;

    NoFila *remover = f->inicio;
    void *conteudo = remover->conteudo;

    f->inicio = remover->prox;
    if (!f->inicio) {
        f->fim = NULL;
    }

    free(remover);
    f->tamanho--;

    return conteudo;
}


void* inicioFila(Fila fila) {
    if (!fila) return NULL;

    stFila *f = (stFila *)fila;
    if (!f->inicio) return NULL;

    return f->inicio->conteudo;
}


bool filaVazia(Fila fila) {
    if (!fila) return true;

    stFila *f = (stFila *)fila;
    return (f->tamanho == 0);
}


int tamanhoFila(Fila fila) {
    if (!fila) return 0;

    stFila *f = (stFila *)fila;
    return f->tamanho;
}


void desalocaFila(Fila fila) {
    if (!fila) return;

    stFila *f = (stFila *)fila;

    NoFila *atual = f->inicio;
    while (atual) {
        NoFila *prox = atual->prox;
        free(atual);
        atual = prox;
    }

    free(f);
}
