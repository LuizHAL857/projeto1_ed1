#ifndef DISPARADOR_H
#define DISPARADOR_H

#include "formas.h"
#include <stdbool.h>

/**
 * @brief Tipo opaco para o Carregador (pilha de formas).
 */
typedef void* Carregador;

/**
 * @brief Tipo opaco para o Disparador.
 */
typedef void* Disparador;

/**
 * @brief Cria um novo carregador.
 * @param id Identificador do carregador.
 * @return O carregador criado.
 */
Carregador cria_carregador(int id);

/**
 * @brief Adiciona uma forma ao carregador.
 * @param c Carregador.
 * @param f Forma a ser adicionada.
 */
void carregador_adiciona_forma(Carregador c, Forma f);

/**
 * @brief Verifica se o carregador está vazio.
 * @param c Carregador.
 * @return true se vazio, false caso contrário.
 */
bool carregador_vazio(Carregador c);

/**
 * @brief Remove e retorna a forma do topo do carregador.
 * @param c Carregador.
 * @return Forma removida ou NULL se vazio.
 */
Forma carregador_remover_forma(Carregador c);

/**
 * @brief Obtém o ID do carregador.
 */
int obter_id_carregador(Carregador c);

/**
 * @brief Desaloca o carregador e suas formas.
 */
void desaloca_carregador(Carregador c);


/**
 * @brief Cria um novo disparador.
 * @param id Identificador do disparador.
 * @param x Coordenada X.
 * @param y Coordenada Y.
 * @return O disparador criado.
 */
Disparador cria_disparador(int id, double x, double y);

/**
 * @brief Conecta carregadores ao disparador.
 * @param d Disparador.
 * @param cesq Carregador esquerdo (pode ser NULL).
 * @param cdir Carregador direito (pode ser NULL).
 */
void disparador_conecta_carregadores(Disparador d, Carregador cesq, Carregador cdir);

/**
 * @brief Realiza a operação de shift (botão de carga).
 * @param d Disparador.
 * @param lado 'e' para esquerdo, 'd' para direito.
 * @param n Número de vezes.
 * @return Forma que está na posição de disparo após a operação (pode ser NULL).
 */
Forma disparador_shift(Disparador d, char lado, int n);

/**
 * @brief Obtém a forma atualmente na posição de disparo.
 * @param d Disparador.
 * @return Forma ou NULL.
 */
Forma disparador_obter_forma_disparo(Disparador d);

/**
 * @brief Remove a forma da posição de disparo (para disparar).
 * @param d Disparador.
 * @return Forma que estava pronta para disparo ou NULL.
 */
Forma disparador_remover_forma_disparo(Disparador d);

/**
 * @brief Obtém o ID do disparador.
 */
int obter_id_disparador(Disparador d);

/**
 * @brief Obtém a posição do disparador.
 */
void obter_posicao_disparador(Disparador d, double *x, double *y);

/**
 * @brief Desaloca o disparador (não desaloca os carregadores conectados, pois podem ser compartilhados ou gerenciados externamente).
 */
void desaloca_disparador(Disparador d);

#endif
