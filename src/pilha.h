#ifndef PILHA_H
#define PILHA_H

/**
 * Este módulo define uma estrutura de dados do tipo pilha (stack) com
 * alocação dinâmica. A pilha segue o comportamento LIFO (Last In, First Out),
 * permitindo inserções e remoções apenas no topo.
 *
 * Os elementos armazenados na pilha são ponteiros genéricos,
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
 * @brief Tipo abstrato que representa a pilha como um ponteiro genérico.
 */
typedef void* Pilha;

/**
 * @brief Cria uma nova pilha vazia.
 * 
 * @return Pilha criada (ponteiro opaco).
 */
Pilha criaPilha();

/**
 * @brief Insere um elemento no topo da pilha.
 * 
 * @param pilha Pilha na qual o elemento será inserido.
 * @param conteudo Ponteiro para o conteúdo a ser inserido.
 * @return true se a inserção for bem-sucedida, false caso contrário.
 */
bool pushPilha(Pilha pilha, void *conteudo);

/**
 * @brief Remove o elemento do topo da pilha.
 * 
 * @param pilha Pilha de onde o elemento será removido.
 * @return Ponteiro para o conteúdo removido ou NULL se a pilha estiver vazia.
 */
void* popPilha(Pilha pilha);

/**
 * @brief Retorna o conteúdo do topo da pilha sem removê-lo.
 * 
 * @param pilha Pilha a ser consultada.
 * @return Ponteiro para o conteúdo do topo ou NULL se a pilha estiver vazia.
 */
void* topoPilha(Pilha pilha);


/**


 * Retorna o elemento da pilha no índice especificado sem remove-lo


 * @param stack Poteiro para a pilha


 * @param index Indice do elemento ( 0 é o topo da pilha)


 * @return Conteúdo do elemento no indice ou NULL se a pilha for vazia ou o indice inválido


 */

 void *pilhaElemento(Pilha pilha, int id);

/**
 * @brief Verifica se a pilha está vazia.
 * 
 * @param pilha Pilha a ser verificada.
 * @return true se estiver vazia, false caso contrário.
 */
bool pilhaVazia(Pilha pilha);

/**
 * @brief Retorna a quantidade de elementos na pilha.
 * 
 * @param pilha Pilha a ser consultada.
 * @return Número de elementos na pilha.
 */
int tamanhoPilha(Pilha pilha);

/**
 * @brief Libera toda a memória alocada pela pilha.
 * 
 * @param pilha Pilha a ser desalocada.
 */
void desalocaPilha(Pilha pilha);

#endif
