#include "pilha.h"
#include <stdio.h>


typedef struct NoPilha {
    void *conteudo;
    struct NoPilha *baixo;
} NoPilha;


struct Pilha {
    NoPilha *topo;
    int tamanho;
};

Pilha* criaPilha() {
    Pilha *pilha = (Pilha *)malloc(sizeof(Pilha));
    if (!pilha) {
        printf("[ERRO] Falha ao alocar memória para a pilha.\n");
        return NULL;
    }

    pilha->topo = NULL;
    pilha->tamanho = 0;
    return pilha;
}

bool pushPilha(Pilha *pilha, void *conteudo) {
    if (!pilha) {
        return false;
    }
    NoPilha *novo = (NoPilha *)malloc(sizeof(NoPilha));
    if (!novo) {
        printf("[ERRO] Falha ao alocar memória para novo nó da pilha.\n");
        return false;
    }

    novo->conteudo = conteudo;
    novo->baixo = pilha->topo;
    pilha->topo = novo;
    pilha->tamanho++;
    return true;
}

void* popPilha(Pilha *pilha) {
    if (!pilha || pilha->topo == NULL) {
        
        return NULL;
    }

    NoPilha *remover = pilha->topo;
    void *conteudo = remover->conteudo;

    pilha->topo = remover->baixo;
    free(remover);
    pilha->tamanho--;

    return conteudo;
}

void* topoPilha(Pilha* pilha) {
    if (!pilha || pilha->topo == NULL) {
        return NULL;
    }
    return pilha->topo->conteudo;
}

bool pilhaVazia(Pilha* pilha) {
    return (pilha == NULL || pilha->tamanho == 0);
}

int tamanhoPilha(Pilha* pilha) {
    if (!pilha) {
        return 0;
    }
    return pilha->tamanho;
}

void desalocaPilha(Pilha *pilha) {
    if (!pilha) {
        return;
    }
    
    NoPilha *atual = pilha->topo;
    while (atual) {
        NoPilha *anterior = atual->baixo;
        free(atual);
        atual = anterior;
    }

    free(pilha);
}
