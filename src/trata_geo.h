#ifndef TRATA_GEO_H
#define TRATA_GEO_H

#include "fila.h"
#include "leitor_arquivos.h" 

/**
  Módulo responsável por interpretar e executar comandos do arquivo `.geo`, 
 *        armazenando as formas e gerando a saída em SVG.
 * 
 * Esse módulo processa o conteúdo lido de arquivos `.geo`, cria objetos geométricos 
 * correspondentes (círculo, retângulo, linha), e os armazena 
 * em estruturas auxiliares. Ele também é responsável por gerar a saída gráfica em formato SVG.
 */

/**
 * @typedef Chao
 * @brief Representa um contexto interno onde todas as formas geométricas e estilos são armazenados.
 * 
 */
typedef void* Chao;

/**
 * @brief Processa os comandos presentes no arquivo `.geo`.
 * 
 * Essa função percorre cada linha do arquivo `.geo`, interpreta os comandos (como "c", "r", "l", "t", "ts"),
 * e armazena as formas correspondentes nas filas e pilhas internas. Ao final, gera um arquivo `.svg`
 * com os elementos gráficos resultantes.
 * 
 * @param fileData Estrutura contendo os dados lidos do arquivo `.geo`, incluindo suas linhas.
 * @param caminho_output Caminho para o diretório onde o arquivo SVG de saída será criado.
 * @param sufixo_comando Sufixo a ser adicionado ao nome do arquivo de saída SVG, antes da extensão.
 * 
 * @return Um ponteiro opaco para o contexto (Chao) contendo todas as formas e estruturas alocadas.
 *         Esse ponteiro deve ser usado para operações posteriores e precisa ser desalocado com `desaloca_geo`.
 */
Chao executa_comando_geo(DadosDoArquivo fileData, char *caminho_output,  char *sufixo_comando);

/**
 * @brief Retorna a fila contendo todas as formas geométricas criadas no contexto `Chao`.
 * 
 * Essa fila pode ser usada para processamento adicional ou visualização das formas
 * fora do módulo `trata_geo`.
 * 
 * @param chao Contexto de execução retornado por `executa_comando_geo`.
 * @return Fila com os elementos gráficos processados.
 */
Fila get_fila_chao(Chao chao);

/**
 * @brief Libera toda a memória alocada para o contexto `Chao`.
 * 
 * Essa função desaloca todas as formas geométricas, estilos de texto, bem como
 * as estruturas auxiliares (fila, pilha) utilizadas durante o processamento.
 * 
 * @param chao Contexto a ser desalocado, retornado por `executa_comando_geo`.
 */
void desaloca_geo(Chao chao);

/** *


    Obtem a pilha para liberar formas do chão


    @param Chao chao- O chão do qual se vai obter a pilha para liberar formas

    @return A pilha para liberar formas


*/
Pilha obtem_pilha_para_desalocar(Chao chao); 

#endif 
