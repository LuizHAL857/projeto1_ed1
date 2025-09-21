#ifndef CIRCULO_H
#define CIRCULO_H




/**
 * @file circulo.h
 * @brief Interface para o módulo de Circulo.
 *
 * Este módulo define uma estrutura abstrata para representar um circulo
 * e fornece funções para criação, acesso, modificação e desalocação de circulos,
 * além de cálculo de área.
 */




/**
 * @typedef CIRCULO
 * Tipo opaco que representa um círculo.
 */
typedef void* CIRCULO;




//FUNÇÃO CONSTRUTORA

/**
 * @brief Cria e inicializa um novo círculo.
 * 
 * @param id Identificador do círculo.
 * @param x Coordenada X do centro do círculo.
 * @param y Coordenada Y do centro do círculo.
 * @param raio Raio do círculo.
 * @param corP Cor de preenchimento do círculo (string).
 * @param corB Cor da borda do círculo (string).
 * @return CIRCULO Retorna um ponteiro para o novo círculo criado.
 */
CIRCULO criaCirculo(int id, float x, float y, float raio, char* corP, char* corB);



//MÉTODOS DE GET DO CIRCULO


/**
 * @brief Retorna o ID do círculo.
 * 
 * @param c Ponteiro para o círculo.
 * @return int ID do círculo.
 */
int getIDCirculo(CIRCULO c);

/**
 * @brief Retorna a coordenada X do círculo.
 * 
 * @param c Ponteiro para o círculo.
 * @return float Coordenada X.
 */
float getXCirculo(CIRCULO c);

/**
 * @brief Retorna a coordenada Y do círculo.
 * 
 * @param c Ponteiro para o círculo.
 * @return float Coordenada Y.
 */
float getYCirculo(CIRCULO c);

/**
 * @brief Retorna o raio do círculo.
 * 
 * @param c Ponteiro para o círculo.
 * @return float Raio do círculo.
 */
float getRaioCirculo(CIRCULO c);

/**
 * @brief Retorna a cor de preenchimento do círculo.
 * 
 * @param c Ponteiro para o círculo.
 * @return char* String contendo a cor de preenchimento.
 */
char* getCorPCirculo(CIRCULO c);

/**
 * @brief Retorna a cor da borda do círculo.
 * 
 * @param c Ponteiro para o círculo.
 * @return char* String contendo a cor da borda.
 */
char* getCorBCirculo(CIRCULO c);




//MÉTODOS DE SET DO CÍRCULO



/**
 * @brief Define o ID do círculo.
 * 
 * @param c Ponteiro para o círculo.
 * @param id Novo ID a ser definido.
 */
void setIDCirculo(CIRCULO c, int id);

/**
 * @brief Define a coordenada X do círculo.
 * 
 * @param c Ponteiro para o círculo.
 * @param x Novo valor para a coordenada X.
 */
void setXCirculo(CIRCULO c, float x);

/**
 * @brief Define a coordenada Y do círculo.
 * 
 * @param c Ponteiro para o círculo.
 * @param y Novo valor para a coordenada Y.
 */
void setYCirculo(CIRCULO c, float y);

/**
 * @brief Define o raio do círculo.
 * 
 * @param c Ponteiro para o círculo.
 * @param raio Novo valor de raio.
 */
void setRaioCirculo(CIRCULO c, float raio);

/**
 * @brief Define a cor de preenchimento do círculo.
 * 
 * @param c Ponteiro para o círculo.
 * @param corP Nova cor de preenchimento (string).
 */
void setCorPCirculo(CIRCULO c, char* corP);

/**
 * @brief Define a cor da borda do círculo.
 * 
 * @param c Ponteiro para o círculo.
 * @param corB Nova cor da borda (string).
 */
void setCorBCirculo(CIRCULO c, char* corB);


//MÉTODOS ADICIONAIS

/**
 * @brief Calcula e retorna a área do círculo.
 * 
 * @param c Ponteiro para o círculo.
 * @return float Área do círculo (π * r²).
 */
float calculoAreaCirculo(CIRCULO c);


/**
 * @brief Libera toda a memória alocada para o círculo.
 * 
 * @param c Ponteiro para o círculo a ser desalocado.
 */
void desalocaCirculo(CIRCULO c);


#endif