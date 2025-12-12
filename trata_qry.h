#ifndef QRY_HANDLER_H
#define QRY_HANDLER_H
#include "leitor_arquivos.h"
#include "trata_geo.h"

typedef void *Qry;

/**
 * @brief Executa o processamento dos comandos presentes em um arquivo `.qry` 
 *        sobre os dados carregados do arquivo `.geo`, manipulando estruturas 
 *        de disparadores, carregadores e o "chão" (arena de simulação), além 
 *        de gerar saídas em formato `.txt` e `.svg`.
 *
 * Esta função lê linha a linha os comandos do arquivo `.qry`, identifica o tipo 
 * de operação (como "pd", "lc", "atch", "shft", "dsp", "rjd", "calc") e executa 
 * a rotina correspondente, alterando o estado interno das estruturas conforme necessário.
 * 
 * Além disso, gera um arquivo `.txt` com o mesmo nome base dos arquivos `.geo` e `.qry`,
 * contendo os resultados textuais das operações, e um arquivo `.svg` final com as 
 * representações gráficas resultantes.
 *
 * @param dados_qry Estrutura contendo informações e filas de comandos lidos do arquivo `.qry`.
 * @param dados_geo Estrutura contendo informações e objetos geométricos lidos do arquivo `.geo`.
 * @param chao Estrutura representando o "chão" (ou arena) onde os elementos são posicionados e manipulados.
 * @param output_path Caminho de diretório onde os arquivos de saída (`.txt` e `.svg`) serão gravados.
 *
 * @return Ponteiro para a estrutura `Qry` alocada e inicializada, contendo as pilhas e arenas utilizadas
 **/
Qry executa_qry(DadosDoArquivo dadosQry, DadosDoArquivo dadosGeo, Chao chao,  char *caminho_output);

/**
 * @brief Libera toda a memória alocada no processo das consultas de executa_qry
 * 
 * Essa função desaloca todas as formas geométricas, estilos de texto, bem como
 * as estruturas auxiliares (fila, pilha) utilizadas durante o processamento.
 * 
 * @param qry Contexto a ser desalocado, retornado por `executa_qry`.
 */
void desaloca_qry(Qry qry);

#endif 