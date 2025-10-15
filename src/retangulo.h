#ifndef RETANGULO_H
#define RETANGULO_H

/**
 * @file retangulo.h
 * @brief Interface para o módulo de Retângulo.
 *
 * Este módulo define uma estrutura abstrata para representar um retângulo em um plano 2d, seguindo os padrões svg
 * e fornece funções para criação, acesso, modificação e desalocação de retângulos,
 * além de cálculo de área.
 */

/// Tipo abstrato para representar um retângulo.

typedef void* RETANGULO;

/**
 * @brief Cria um novo retângulo com os atributos especificados.
 *
 * @param id Identificador único do retângulo.
 * @param x Coordenada X da posição do retângulo.
 * @param y Coordenada Y da posição do retângulo.
 * @param altura Altura do retângulo.
 * @param largura Largura do retângulo.
 * @param corB Cor da borda (string).
 * @param corP Cor de preenchimento (string).
 * @return Um ponteiro para o retângulo criado.
 */
RETANGULO criaRetangulo(int id, float x, float y, float altura, float largura, char *corB, char *corP);

/**
 * @brief Retorna o ID do retângulo.
 *
 * @param r Retângulo.
 * @return ID do retângulo.
 */
int getIDRetangulo(RETANGULO r);

/**
 * @brief Retorna a coordenada X do retângulo.
 *
 * @param r Retângulo.
 * @return Valor de X.
 */
float getXRetangulo(RETANGULO r);

/**
 * @brief Retorna a coordenada Y do retângulo.
 *
 * @param r Retângulo.
 * @return Valor de Y.
 */
float getYRetangulo(RETANGULO r);

/**
 * @brief Retorna a altura do retângulo.
 *
 * @param r Retângulo.
 * @return Altura.
 */
float getAlturaRetangulo(RETANGULO r);

/**
 * @brief Retorna a largura do retângulo.
 *
 * @param r Retângulo.
 * @return Largura.
 */
float getLarguraRetangulo(RETANGULO r);

/**
 * @brief Retorna a cor da borda do retângulo.
 *
 * @param r Retângulo.
 * @return String com a cor da borda.
 */
char* getCorBRetangulo(RETANGULO r);

/**
 * @brief Retorna a cor de preenchimento do retângulo.
 *
 * @param r Retângulo.
 * @return String com a cor de preenchimento.
 */
char* getCorPRetangulo(RETANGULO r);

/**
 * @brief Define um novo ID para o retângulo.
 *
 * @param r Retângulo.
 * @param id Novo ID.
 */
void setIDRetangulo(RETANGULO r, int id);

/**
 * @brief Define uma nova coordenada X para o retângulo.
 *
 * @param r Retângulo.
 * @param x Nova coordenada X.
 */
void setXRetangulo(RETANGULO r, float x);

/**
 * @brief Define uma nova coordenada Y para o retângulo.
 *
 * @param r Retângulo.
 * @param y Nova coordenada Y.
 */
void setYRetangulo(RETANGULO r, float y);

/**
 * @brief Define uma nova cor de borda para o retângulo.
 *
 * @param r Retângulo.
 * @param corB Nova cor da borda (string).
 */
void setCorBRetangulo(RETANGULO r, char *corB);

/**
 * @brief Define uma nova cor de preenchimento para o retângulo.
 *
 * @param r Retângulo.
 * @param corP Nova cor de preenchimento (string).
 */
void setCorPRetangulo(RETANGULO r, char *corP);

/**
 * @brief Calcula a área do retângulo.
 *
 * @param r Retângulo.
 * @return Área calculada (altura * largura).
 */
float calculoAreaRetangulo(RETANGULO r);

/**
 * @brief Libera a memória alocada para o retângulo.
 *
 * @param r Retângulo a ser desalocado.
 */
void desalocarRetangulo(RETANGULO r);




#endif 
