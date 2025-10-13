
#include "leitor_arquivos.h"
#include "fila.h"
#include "pilha.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Estrutura que representa os dados de um arquivo
struct DadosDoArquivo {
    const char *caminhoArquivo;
    const char *nomeArquivo;
    Fila filaDeLinhas;
    // Esta pilha é usada para liberar as linhas do
    // arquivo após a execução dos comandos
    Pilha pilhaLinhasParaLiberar;
  };
  
  // Estrutura auxiliar que guarda uma fila e uma pilha de linhas
  struct FilaEPilhaDeLinhas {
    Fila filaDeLinhas;
    Pilha pilhaLinhasParaLiberar;
  };
  
  // Funções privadas
  static char *ler_linha(FILE *arquivo, char *buffer, size_t tamanho);

  static struct FilaEPilhaDeLinhas *
  ler_arquivo_para_fila_e_pilha(const char *caminhoArquivo);

  char *duplicaString(const char *s) ;
  
 
  DadosDoArquivo criar_dados_arquivo(const char *caminhoArquivo) {
    struct DadosDoArquivo *arquivo = malloc(sizeof(struct DadosDoArquivo));
    if (arquivo == NULL) {
      printf("Erro: Falha ao alocar memória para DadosDoArquivo\n");
      return NULL;
    }
  
    arquivo->caminhoArquivo = caminhoArquivo;
    arquivo->nomeArquivo =
        strrchr(caminhoArquivo, '/') ? strrchr(caminhoArquivo, '/') + 1 : caminhoArquivo;
  
    struct FilaEPilhaDeLinhas *filaEPilha =
        ler_arquivo_para_fila_e_pilha(caminhoArquivo);
  
    if (filaEPilha == NULL || filaEPilha->filaDeLinhas == NULL ||
        filaEPilha->pilhaLinhasParaLiberar == NULL) {
      printf("Erro: Falha ao ler as linhas do arquivo\n");
      if (filaEPilha != NULL) {
        free(filaEPilha);
      }
      return NULL;
    }
  
    arquivo->filaDeLinhas = filaEPilha->filaDeLinhas;
    arquivo->pilhaLinhasParaLiberar = filaEPilha->pilhaLinhasParaLiberar;
    free(filaEPilha);
    return (DadosDoArquivo)arquivo;
  }
  
 
  static struct FilaEPilhaDeLinhas *
  ler_arquivo_para_fila_e_pilha(const char *caminhoArquivo) {
    struct FilaEPilhaDeLinhas *filaEPilha =
        malloc(sizeof(struct FilaEPilhaDeLinhas));
    if (filaEPilha == NULL) {
      printf("Erro: Falha ao alocar memória para Fila E Pilha De Linhas\n");
      return NULL;
    }
  
    Fila fila = criaFila();
    Pilha pilha = criaPilha();
    FILE *arquivo = fopen(caminhoArquivo, "r");
    if (arquivo == NULL) {
    
      if (fila != NULL) {
        desalocaFila(fila);
      }
      if (pilha != NULL) {
        desalocaPilha(pilha);
      }
      free(filaEPilha);
      return NULL;
    }
  
    char buffer[1024];
    while (ler_linha(arquivo, buffer, sizeof(buffer)) != NULL) {
      char *linha = duplicaString(buffer);
      enqueueFila(fila, linha);
      pushPilha(pilha, linha);
    }
  
    fclose(arquivo);
    // Atribui as estruturas criadas à estrutura antes de retornar
    filaEPilha->filaDeLinhas = fila;
    filaEPilha->pilhaLinhasParaLiberar = pilha;
    return filaEPilha;
  }
  
  // Destroi uma instância de DadosDoArquivo e libera memória
  void destruir_dados_arquivo(DadosDoArquivo dadosArquivo) {
    if (dadosArquivo != NULL) {
      struct DadosDoArquivo *arquivo = (struct DadosDoArquivo *)dadosArquivo;
      // Libera as linhas do arquivo
      while (!pilhaVazia(arquivo->pilhaLinhasParaLiberar)) {
        void *linha = popPilha(arquivo->pilhaLinhasParaLiberar);
        linha != NULL ? free(linha) : NULL;
      }
      // Destroi a pilha de linhas
      desalocaPilha(arquivo->pilhaLinhasParaLiberar);
      // Destroi a fila de linhas
      desalocaFila(arquivo->filaDeLinhas);
      // Libera a estrutura de dados do arquivo
      free(dadosArquivo);
    }
  }
  
  // Retorna o caminho do arquivo
  const char *obter_caminho_arquivo(const DadosDoArquivo dadosArquivo) {
    struct DadosDoArquivo *arquivo = (struct DadosDoArquivo *)dadosArquivo;
    return arquivo->caminhoArquivo;
  }
  
  // Retorna o nome do arquivo
  const char *obter_nome_arquivo(const DadosDoArquivo dadosArquivo) {
    struct DadosDoArquivo *arquivo = (struct DadosDoArquivo *)dadosArquivo;
    return arquivo->nomeArquivo;
  }
  
  // Retorna a fila de linhas do arquivo
  Fila obter_fila_linhas(const DadosDoArquivo dadosArquivo) {
    struct DadosDoArquivo *arquivo = (struct DadosDoArquivo *)dadosArquivo;
    return arquivo->filaDeLinhas;
  }
  
  // Lê uma linha do arquivo usando fgets
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
  
  char *duplicaString(const char *s) {
    if (s == NULL)
      return NULL;
  
    size_t len = strlen(s) + 1;
    char *dup = malloc(len);
    if (dup != NULL) {
      strcpy(dup, s);
    }
    return dup;
  }