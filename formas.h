#ifndef FORMAS_H
#define FORMAS_H

/*Módulo com o enum que cuida da tipagem dos tipos genéricos de formas que municiarão os disparadores*/

enum TipoForma{ CIRCLE, RECTANGLE, LINE, TEXT, TEXT_STYLE };
typedef enum TipoForma TipoForma;

/**
 * @brief Tipo opaco que representa uma forma genérica.
 */
typedef void* Forma;

/**
 * @brief Cria uma nova forma genérica.
 * 
 * @param tipo O tipo da forma (CIRCLE, RECTANGLE, etc).
 * @param data Ponteiro para os dados da forma específica.
 * @return Forma O ponteiro para a forma criada.
 */
Forma cria_forma(TipoForma tipo, void* data);

/**
 * @brief Obtém o tipo da forma.
 * 
 * @param f A forma.
 * @return TipoForma O tipo da forma.
 */
TipoForma get_tipo_forma(Forma f);

/**
 * @brief Obtém os dados da forma.
 * 
 * @param f A forma.
 * @return void* Ponteiro para os dados da forma.
 */
void* get_data_forma(Forma f);

/**
 * @brief Clona uma forma genérica.
 * 
 * @param forma Ponteiro para a forma a ser clonada.
 * @return Forma Ponteiro para a nova forma clonada.
 */
Forma clona_forma(Forma forma);

/**
 * @brief Calcula a área de uma forma genérica.
 * 
 * @param forma Ponteiro para a forma.
 * @return float Área da forma.
 */
float calcula_area_forma(Forma forma);

/**
 * @brief Desaloca a estrutura da forma (mas não necessariamente os dados, dependendo da implementação).
 * Neste caso, desaloca apenas o wrapper Forma.
 * 
 * @param f A forma a ser desalocada.
 */
void desaloca_forma(Forma f);

/**
 * @brief Define a posição de uma forma genérica.
 */
void definir_posicao_forma(Forma f, double x, double y);

/**
 * @brief Obtém a posição de uma forma genérica.
 */
void obter_posicao_forma(Forma f, double *x, double *y);

/**
 * @brief Obtém a cor de preenchimento de uma forma genérica.
 * Retorna uma nova string que deve ser liberada pelo chamador.
 */
char* obter_cor_preenchimento(Forma f);

/**
 * @brief Define a cor da borda de uma forma genérica.
 */
void definir_cor_borda(Forma f, char *cor);

/**
 * @brief Clona uma forma e troca suas cores de borda e preenchimento.
 */
Forma clona_e_troca_cores(Forma f);

/**
 * @brief Estrutura para BoundingBox.
 */
typedef struct {
    double minX, minY, maxX, maxY;
} BoundingBox;

/**
 * @brief Obtém a BoundingBox de uma forma.
 */
BoundingBox obter_bounding_box(Forma f);

/**
 * @brief Verifica sobreposição entre duas formas (baseado em BoundingBox).
 */
int verifica_sobreposicao(Forma f1, Forma f2);



/**
 * @brief Converte o tipo da forma para string.
 */
const char* tipo_forma_to_string(TipoForma t);

/**
 * @brief Copia uma string (alternativa C99 para strdup).
 */
char* copia_string(const char* s);

#endif
