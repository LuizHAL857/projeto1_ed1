#ifndef LINHA_H
#define LINHA_H

/**
 * @file linha.h
 * @brief Interface para a estrutura de uma linha geométrica.
 *
 * Este módulo define um tipo abstrato de dado para representar uma linha em um plano 2D,
 * juntamente com funções para criar, acessar, modificar e desalocar linhas.
 */

/// Tipo opaco que representa uma linha
typedef void* LINHA;

/**
 * @brief Cria uma nova linha.
 * 
 * @param id Identificador único da linha.
 * @param x1 Coordenada X do ponto inicial da linha.
 * @param y1 Coordenada Y do ponto inicial da linha.
 * @param x2 Coordenada X do ponto final da linha.
 * @param y2 Coordenada Y do ponto final da linha.
 * @param cor Cor da linha (string).
 * @return Um ponteiro para a linha criada (tipo LINHA).
 */
LINHA criaLinha(int id, float x1, float y1, float x2, float y2, char* cor);

/**
 * @brief Obtém o identificador da linha.
 * 
 * @param l Ponteiro para a linha.
 * @return O identificador da linha.
 */
int getIDLinha(LINHA l);

/**
 * @brief Obtém a coordenada X do ponto inicial da linha.
 * 
 * @param l Ponteiro para a linha.
 * @return A coordenada X do ponto inicial.
 */
float getX1Linha(LINHA l);

/**
 * @brief Obtém a coordenada Y do ponto inicial da linha.
 * 
 * @param l Ponteiro para a linha.
 * @return A coordenada Y do ponto inicial.
 */
float getY1Linha(LINHA l);

/**
 * @brief Obtém a coordenada X do ponto final da linha.
 * 
 * @param l Ponteiro para a linha.
 * @return A coordenada X do ponto final.
 */
float getX2Linha(LINHA l);

/**
 * @brief Obtém a coordenada Y do ponto final da linha.
 * 
 * @param l Ponteiro para a linha.
 * @return A coordenada Y do ponto final.
 */
float getY2Linha(LINHA l);

/**
 * @brief Obtém a cor da linha.
 * 
 * @param l Ponteiro para a linha.
 * @return Ponteiro para a string que representa a cor da linha.
 */
char* getCorLinha(LINHA l);

/**
 * @brief Define o identificador da linha.
 * 
 * @param l Ponteiro para a linha.
 * @param id Novo identificador.
 */
void setIDLinha(LINHA l, int id);

/**
 * @brief Define a coordenada X do ponto inicial da linha.
 * 
 * @param l Ponteiro para a linha.
 * @param x1 Nova coordenada X inicial.
 */
void setX1Linha(LINHA l, float x1);

/**
 * @brief Define a coordenada Y do ponto inicial da linha.
 * 
 * @param l Ponteiro para a linha.
 * @param y1 Nova coordenada Y inicial.
 */
void setY1Linha(LINHA l, float y1);

/**
 * @brief Define a coordenada X do ponto final da linha.
 * 
 * @param l Ponteiro para a linha.
 * @param x2 Nova coordenada X final.
 */
void setX2Linha(LINHA l, float x2);

/**
 * @brief Define a coordenada Y do ponto final da linha.
 * 
 * @param l Ponteiro para a linha.
 * @param y2 Nova coordenada Y final.
 */
void setY2Linha(LINHA l, float y2);

/**
 * @brief Define a cor da linha.
 * 
 * @param l Ponteiro para a linha.
 * @param cor Nova cor (string).
 */
void setCorLinha(LINHA l, char* cor);

/**
 * @brief Calcula a "área" associada à linha.
 *
 * No contexto deste programa, define-se a "área" da linha como sendo 10 vezes
 * o seu comprimento, ou seja:
 *
 *     área = 2 * sqrt((x2 - x1)² + (y2 - y1)²)
 *
 * Essa definição permite a comparação com áreas de figuras geométricas reais.
 *
 * @param l Ponteiro para a linha.
 * @return Valor da "área" (comprimento * 10).
 */
float calculoAreaLinha(LINHA l);


/**
 * @brief Libera a memória alocada para a linha.
 * 
 * @param l Ponteiro para a linha.
 */
void desalocaLinha(LINHA l);

#endif
