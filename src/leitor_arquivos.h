#ifndef LEITOR_ARQUIVOS_H
#define LEITOR_ARQUIVOS_H

#include "fila.h"
#include <stdio.h>

/**
 * @file leitor_arquivos.h
 * @brief Módulo para leitura de arquivos e armazenamento das linhas em uma fila.
 * 
 * Este módulo fornece funções para criar uma estrutura que representa um arquivo,
 * contendo seu caminho, nome e as linhas lidas armazenadas em uma fila dinâmica.
 * Permite acessar o caminho e nome do arquivo, bem como iterar pelas linhas carregadas.
 * Também é responsável por liberar corretamente toda a memória alocada.
 */

/**
 * @brief Estrutura que representa um arquivo lido, contendo seu caminho, nome e as linhas em uma fila.
 */
typedef struct DadosArquivo DadosArquivo;

/**
 * @brief Cria uma nova instância de DadosArquivo, lendo o conteúdo do arquivo especificado.
 * 
 * @param caminho_arquivo Caminho completo do arquivo a ser lido.
 * @return Ponteiro para DadosArquivo criado, ou NULL em caso de erro.
 */
DadosArquivo *dados_arquivo_criar(const char *caminho_arquivo);

/**
 * @brief Libera a memória alocada para DadosArquivo e todas as linhas armazenadas.
 * 
 * @param dados Ponteiro para DadosArquivo a ser destruído.
 */
void dados_arquivo_destruir(DadosArquivo *dados);

/**
 * @brief Obtém o caminho completo do arquivo armazenado em DadosArquivo.
 * 
 * @param dados Ponteiro para DadosArquivo.
 * @return Ponteiro constante para a string com o caminho do arquivo.
 */
const char *obter_caminho_arquivo(const DadosArquivo *dados);

/**
 * @brief Obtém o nome do arquivo (apenas o nome, sem o caminho).
 * 
 * @param dados Ponteiro para DadosArquivo.
 * @return Ponteiro constante para a string com o nome do arquivo.
 */
const char *obter_nome_arquivo(const DadosArquivo *dados);

/**
 * @brief Obtém a fila contendo as linhas do arquivo.
 * 
 * Cada elemento da fila é uma string alocada dinamicamente contendo uma linha do arquivo.
 * O usuário é responsável por tratar esses dados com cuidado.
 * 
 * @param dados Ponteiro para DadosArquivo.
 * @return Fila contendo as linhas do arquivo.
 */
Fila obter_fila_linhas(const DadosArquivo *dados);

#endif // LEITOR_ARQUIVOS_H
