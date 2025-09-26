#ifndef PILHA_H
#define PILHA_H

/**
 
 * Este módulo define uma estrutura de dados do tipo pilha (stack) com
 * alocação dinâmica. A pilha segue o comportamento LIFO (Last In, First Out),
 * permitindo inserções e remoções apenas no topo.
 *
 * Os elementos armazenados na pilha são ponteiros genéricos ,
 * permitindo o uso com diferentes tipos de dados. O gerenciamento da memória
 * dos conteúdos apontados é responsabilidade do usuário.
 *
 * Funcionalidades oferecidas:
 *  - Criação e destruição da pilha
 *  - Inserção (push) e remoção (pop) de elementos
 *  - Consulta ao topo da pilha
 *  - Verificação se a pilha está vazia
 *  - Consulta ao tamanho atual da pilha
 */

#include <stdbool.h>
#include <stdlib.h>

/**
 * Tipo opaco que representa a estrutura da Pilha.
 */
typedef struct Pilha Pilha;

/**
 * @brief Cria uma nova pilha vazia.
 * 
 * @return Ponteiro para a pilha criada.
 */
Pilha* criaPilha();

/**
 * @brief Insere um elemento no topo da pilha.
 * 
 * @param pilha Ponteiro para a pilha.
 * @param conteudo Ponteiro para o conteúdo a ser inserido.
 * @return true se a inserção for bem-sucedida, false caso contrário.
 */
bool pushPilha(Pilha *pilha, void *conteudo);

/**
 * @brief Remove o elemento do topo da pilha.
 * 
 * @param pilha Ponteiro para a pilha.
 * @return Ponteiro para o conteúdo removido ou NULL se a pilha estiver vazia.
 */
void* popPilha(Pilha *pilha);

/**
 * @brief Retorna o conteúdo do topo da pilha sem removê-lo.
 * 
 * @param pilha Ponteiro para a pilha.
 * @return Ponteiro para o conteúdo do topo ou NULL se a pilha estiver vazia.
 */
void* topoPilha(Pilha* pilha);

/**
 * @brief Verifica se a pilha está vazia.
 * 
 * @param pilha Ponteiro para a pilha.
 * @return true se estiver vazia, false caso contrário.
 */
bool pilhaVazia(Pilha* pilha);

/**
 * @brief Retorna a quantidade de elementos na pilha.
 * 
 * @param pilha Ponteiro para a pilha.
 * @return Número de elementos na pilha.
 */
int tamanhoPilha(Pilha* pilha);

/**
 * @brief Libera toda a memória alocada pela pilha.
 * 
 * @param pilha Ponteiro para a pilha a ser desalocada.
 */
void desalocaPilha(Pilha *pilha);

#endif // PILHA_H
