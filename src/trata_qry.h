#ifndef TRATA_QRY_H
#define TRATA_QRY_H

#include "lista.h"
#include "leitor_arquivos.h"
#include "trata_geo.h"

/**
 * @file trata_qry.h
 * @brief Módulo responsável por interpretar e executar comandos do arquivo `.qry`.
 * 
 * Este módulo processa o conteúdo lido de arquivos `.qry`, que contém comandos
 * para manipular as formas geométricas criadas pelo arquivo `.geo`.
 * 
 * Comandos suportados:
 * - a i j [v|h]: Transforma formas em anteparos
 * - d x y sfx: Bomba de destruição
 * - p x y cor sfx: Bomba de pintura
 * - cln x y dx dy sfx: Bomba de clonagem
 */

/**
 * @typedef Qry
 * @brief Representa o contexto de processamento do arquivo .qry
 */
typedef void* Qry;

/**
 * @brief Processa os comandos presentes no arquivo `.qry`.
 * 
 * Essa função percorre cada linha do arquivo `.qry`, interpreta os comandos
 * e executa as operações correspondentes sobre as formas da cidade.
 * 
 * @param fileData Estrutura contendo os dados lidos do arquivo `.qry`.
 * @param cidade Contexto da cidade com as formas geométricas.
 * @param caminho_output Caminho para o diretório de saída.
 * @param maior_id_inicial Maior ID usado no arquivo .geo (para gerar IDs únicos).
 * @return Ponteiro opaco para o contexto Qry.
 */
Qry executa_comando_qry(DadosDoArquivo fileData, Cidade cidade, 
                        char *caminho_output, int maior_id_inicial, char tipo_sort, int threshold);

/**
 * @brief Libera toda a memória alocada para o contexto `Qry`.
 * 
 * @param qry Contexto a ser desalocado.
 */
void desaloca_qry(Qry qry);

#endif
