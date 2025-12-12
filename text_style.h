/* text_style.h
//
// Módulo para gerenciamento de estilos de texto (TextStyle).
// Define uma interface para criação, acesso e destruição de objetos que armazenam
// informações sobre estilo de texto: família da fonte (font family), espessura da fonte
// (font weight) e tamanho da fonte (font size).
//
// A estrutura interna do TextStyle é oculta (opaca), garantindo encapsulamento e
// evitando acesso direto aos campos. A manipulação é feita apenas por meio das funções
// fornecidas neste header.
*/

#ifndef TEXT_STYLE_H
#define TEXT_STYLE_H

typedef void* TEXTSTYLE;

/**
 * @brief Cria um novo objeto TextStyle.
 * 
 * @param ff Ponteiro para string com o nome da família da fonte (font family). Não pode ser NULL.
 * @param fw Ponteiro para string com o peso da fonte (font weight), ex: "bold", "normal".
 * @param fs Inteiro representando o tamanho da fonte (font size).
 * 
 * @return void* Retorna um ponteiro genérico para o objeto criado, ou NULL em caso de falha na alocação
 *         ou se ff for NULL.
 */
TEXTSTYLE criaTextStyle(char* ff, char* fw, int fs);

/**
 * @brief Libera toda a memória associada a um objeto TextStyle.
 * 
 * @param textstyle Ponteiro retornado por criaTextStyle. Se for NULL, nada acontece.
 */
void desalocaTextStyle(void* textstyle);

/**
 * @brief Retorna a font family de um objeto TextStyle.
 * 
 * @param textstyle Ponteiro para o objeto TextStyle.
 * 
 * @return char* Ponteiro para a string contendo a font family, ou NULL se textstyle for NULL.
 */
char* getTextFF(void* textstyle);

/**
 * @brief Retorna a font weight de um objeto TextStyle.
 * 
 * @param textstyle Ponteiro para o objeto TextStyle.
 * 
 * @return char* Ponteiro para a string contendo a font weight, ou NULL se textstyle for NULL.
 */
char* getTextFW(void* textstyle);

/**
 * @brief Retorna o font size de um objeto TextStyle.
 * 
 * @param textstyle Ponteiro para o objeto TextStyle.
 * 
 * @return int Valor inteiro do tamanho da fonte, ou 0 se textstyle for NULL.
 */
int getTextFS(void* textstyle);

#endif 
