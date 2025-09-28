

#include "leitor_arquivos.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fila.h"

// Funções privadas
static Fila ler_arquivo_para_fila(const char *caminho_arquivo);
static char *ler_linha(FILE *arquivo, char *buffer, size_t tamanho);
static char *duplicate_string(const char *str);

struct DadosArquivo {
    const char *caminho;
    const char *nome_arquivo;
    Fila fila_linhas;
};

// Cria uma nova instância DadosArquivo e lê o arquivo
DadosArquivo *dados_arquivo_criar(const char *caminho_arquivo) {
    DadosArquivo *arquivo = malloc(sizeof(DadosArquivo));
    if (arquivo == NULL) {
        printf("[ERRO] Falha ao alocar memória para DadosArquivo\n");
        return NULL;
    }

    arquivo->caminho = caminho_arquivo;
    arquivo->nome_arquivo = 
        strrchr(caminho_arquivo, '/') ? strrchr(caminho_arquivo, '/') + 1 : caminho_arquivo;

    Fila fila = ler_arquivo_para_fila(caminho_arquivo);
    if (fila == NULL) {
        printf("[ERRO] Falha ao ler as linhas do arquivo\n");
        free(arquivo);
        return NULL;
    }
    arquivo->fila_linhas = fila;
    return arquivo;
}

// Destroi DadosArquivo e libera memória
void dados_arquivo_destruir(DadosArquivo *dados) {
    if (dados != NULL) {
        if (dados->fila_linhas != NULL) {
            // Liberar cada string dentro da fila antes de desalocar a fila
            while (!filaVazia(dados->fila_linhas)) {
                char *linha = (char *)dequeueFila(dados->fila_linhas);
                free(linha);
            }
            desalocaFila(dados->fila_linhas);
        }
        free(dados);
    }
}

// Lê arquivo e retorna fila com linhas duplicadas
static Fila ler_arquivo_para_fila(const char *caminho_arquivo) {
    Fila fila = criaFila();
    if (!fila) return NULL;

    FILE *arquivo = fopen(caminho_arquivo, "r");
    if (arquivo == NULL) {
        desalocaFila(fila);
        return NULL;
    }

    char buffer[1024];
    while (ler_linha(arquivo, buffer, sizeof(buffer)) != NULL) {
        char *linha_dup = duplicate_string(buffer);
        if (!linha_dup) {
            printf("[ERRO] Falha ao alocar memória para linha\n");
            fclose(arquivo);
            while (!filaVazia(fila)) {
                free(dequeueFila(fila));
            }
            desalocaFila(fila);
            return NULL;
        }
        enqueueFila(fila, linha_dup);
    }

    fclose(arquivo);
    return fila;
}

// Lê uma linha do arquivo, removendo '\n' se presente
static char *ler_linha(FILE *arquivo, char *buffer, size_t tamanho) {
    if (fgets(buffer, tamanho, arquivo) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        return buffer;
    }
    return NULL;
}

// Duplica string (substituto para strdup)
static char *duplicate_string(const char *str) {
    if (!str) return NULL;
    size_t len = strlen(str) + 1;
    char *copy = malloc(len);
    if (copy) memcpy(copy, str, len);
    return copy;
}

// Retorna caminho completo do arquivo
const char *obter_caminho_arquivo(const DadosArquivo *dados) {
    if (!dados) return NULL;
    return dados->caminho;
}

// Retorna nome do arquivo (sem caminho)
const char *obter_nome_arquivo(const DadosArquivo *dados) {
    if (!dados) return NULL;
    return dados->nome_arquivo;
}

// Retorna fila com linhas do arquivo
Fila obter_fila_linhas(const DadosArquivo *dados) {
    if (!dados) return NULL;
    return dados->fila_linhas;
}
