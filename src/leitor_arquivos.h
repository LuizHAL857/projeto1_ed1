/**
 * @file dados_arquivo.h
 * @brief Biblioteca para leitura de arquivos texto em memória, linha por linha.
 *
 * Esta biblioteca define a estrutura e as funções necessárias para carregar
 * um arquivo texto em memória, mantendo suas linhas armazenadas em uma fila (Fila)
 */

 #ifndef LEITOR_ARQUIVOS_H
 #define LEITOR_ARQUIVOS_H
 
 #include <stdio.h>
 #include "pilha.h"
 #include "fila.h"



 // Tipo opaco para DadosDoArquivo
 typedef void* DadosDoArquivo;
 
 /**
  * Cria uma nova instância de DadosDoArquivo e lê o conteúdo do arquivo.
  *
  * @param caminhoArquivo Caminho completo para o arquivo.
  * @return Instância de DadosDoArquivo ou NULL em caso de erro.
  */
 DadosDoArquivo criar_dados_arquivo( char *caminhoArquivo);
 
 /**
  * Destroi uma instância de DadosDoArquivo e libera toda a memória associada.
  *
  * @param dadosArquivo A instância a ser destruída.
  */
 void destruir_dados_arquivo(DadosDoArquivo dadosArquivo);
 
 /**
  * Obtém o caminho completo do arquivo.
  *
  * @param dadosArquivo Instância de DadosDoArquivo.
  * @return Ponteiro para string com o caminho do arquivo.
  */
  char *obter_caminho_arquivo( DadosDoArquivo dadosArquivo);
 
 /**
  * Obtém o nome do arquivo (sem o caminho).
  *
  * @param dadosArquivo Instância de DadosDoArquivo.
  * @return Ponteiro para string com o nome do arquivo.
  */
  char *obter_nome_arquivo( DadosDoArquivo dadosArquivo);
 
 /**
  * Obtém a fila com as linhas do arquivo.
  *
  * @param dadosArquivo Instância de DadosDoArquivo.
  * @return Fila contendo as linhas do arquivo.
  */
 Fila obter_fila_linhas( DadosDoArquivo dadosArquivo);
 
 #endif // DADOS_ARQUIVO_H
 