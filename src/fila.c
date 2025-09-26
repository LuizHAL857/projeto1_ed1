#include "fila.h"
#include <stdio.h>

// Estrutura de um nó da fila
typedef struct NoFila {
    void *conteudo;
    struct NoFila *prox;
} NoFila;

// Estrutura da fila
struct Fila {
    NoFila *inicio;
    NoFila *fim;
    int tamanho;
};

Fila* criaFila() {
    Fila *fila = (Fila *)malloc(sizeof(Fila));
    if (!fila) {
        printf("[ERRO] Falha ao alocar memória para a fila.\n");
        return NULL;
    }

    fila->inicio = NULL;
    fila->fim = NULL;
    fila->tamanho = 0;
    return fila;
}

bool enqueueFila(Fila *fila, void *conteudo) {
    if (!fila) return false;

    NoFila *novo = (NoFila *)malloc(sizeof(NoFila));
    if (!novo) {
        printf("[ERRO] Falha ao alocar memória para o novo nó da fila.\n");
        return false;
    }

    novo->conteudo = conteudo;
    novo->prox = NULL;

    if (fila->fim) {
        fila->fim->prox = novo;
    } else {
        fila->inicio = novo;
    }

    fila->fim = novo;
    fila->tamanho++;
    return true;
}

void* dequeueFila(Fila *fila) {
    if (!fila || fila->inicio == NULL) return NULL;

    NoFila *remover = fila->inicio;
    void *conteudo = remover->conteudo;

    fila->inicio = remover->prox;
    if (fila->inicio == NULL) {
        fila->fim = NULL;
    }

    free(remover);
    fila->tamanho--;

    return conteudo;
}

void* inicioFila(Fila *fila) {
    if (!fila || fila->inicio == NULL) return NULL;
    return fila->inicio->conteudo;
}

bool filaVazia(Fila *fila) {
    return (fila == NULL || fila->tamanho == 0);
}

int tamanhoFila(Fila *fila) {
    if (!fila) return 0;
    return fila->tamanho;
}

void desalocaFila(Fila* fila) {
    if (!fila) return;

    NoFila *atual = fila->inicio;
    while (atual) {
        NoFila *prox = atual->prox;
        free(atual);
        atual = prox;
    }

    free(fila);
}
