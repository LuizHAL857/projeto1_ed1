#ifndef FILA_H
#define FILA_H

/**
 * @file fila.h
 * @brief Módulo de implementação de uma fila genérica em C.
 *
 * Este módulo fornece a definição e as funções para manipular uma estrutura
 * de dados do tipo fila (queue) com alocação dinâmica. A fila é implementada
 * como uma lista encadeada simples, permitindo inserção no final e remoção
 * no início (política FIFO — First In, First Out).
 *
 * Os dados armazenados na fila são do tipo `void*`, permitindo o uso genérico
 * com qualquer tipo de dado. O gerenciamento da memória dos dados armazenados
 * (conteúdo dos ponteiros) é responsabilidade do usuário.
 *
 * Funcionalidades oferecidas:
 *  - Criação e destruição de filas
 *  - Inserção e remoção de elementos
 *  - Consulta ao elemento do início
 *  - Verificação se a fila está vazia
 *  - Consulta ao tamanho da fila
 */

#include <stdbool.h>
#include <stdlib.h>

/**
 * Tipo opaco que representa a estrutura da Fila.
 */
typedef struct Fila Fila;

/**
 * @brief Cria uma nova fila vazia.
 * 
 * @return Ponteiro para a fila criada.
 */
Fila* criaFila();

/**
 * @brief Insere um elemento no final da fila.
 * 
 * @param fila Ponteiro para a fila.
 * @param conteudo Ponteiro para o conteúdo a ser inserido.
 * @return true se a inserção for bem-sucedida, false caso contrário.
 */
bool enqueueFila(Fila *fila, void *conteudo);

/**
 * @brief Remove o primeiro elemento da fila.
 * 
 * @param fila Ponteiro para a fila.
 * @return Ponteiro para o conteúdo removido ou NULL se a fila estiver vazia.
 */
void* dequeueFila(Fila *fila);

/**
 * @brief Retorna o conteúdo do início da fila sem removê-lo.
 * 
 * @param fila Ponteiro para a fila.
 * @return Ponteiro para o conteúdo do início da fila ou NULL se a fila estiver vazia.
 */
void* inicioFila(Fila *fila);

/**
 * @brief Verifica se a fila está vazia.
 * 
 * @param fila Ponteiro para a fila.
 * @return true se a fila estiver vazia, false caso contrário.
 */
bool filaVazia(Fila *fila);

/**
 * @brief Retorna a quantidade de elementos na fila.
 * 
 * @param fila Ponteiro para a fila.
 * @return Número de elementos na fila.
 */
int tamanhoFila(Fila *fila);

/**
 * @brief Libera toda a memória alocada pela fila.
 * 
 * @param fila Ponteiro para a fila a ser desalocada.
 */
void desalocaFila(Fila* fila);

#endif // FILA_H
