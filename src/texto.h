
 #ifndef TEXTO_H
 #define TEXTO_H
 

/**
 * Interface para o módulo de manipulação de textos gráficos.
 * Define o tipo abstrato TEXTO e uma série de funções para criação,
 * acesso e modificação de objetos de texto utilizados em contextos gráficos,
 * mais especificamente svg
 */






 /**
  * Tipo opaco que representa um objeto de texto gráfico.
  * 
  * A estrutura interna é escondida (abstraída) do usuário.
  */
 typedef void* TEXTO;
 
 /**
  * @brief Cria um novo objeto TEXTO com os atributos especificados.
  * 
  * @param id Identificador único do texto.
  * @param x Coordenada X da posição do texto.
  * @param y Coordenada Y da posição do texto.
  * @param corB Cor da borda (cor do contorno, geralmente aplicada ao traçado do texto).
  * @param corP Cor de preenchimento (cor da fonte, o "fill").
  * @param ancora Caractere que define a âncora de alinhamento do texto:
  *               - 'i' (início / left)
  *               - 'm' (meio / center)
  *               - 'f' (fim / right)
  * @param txt String de texto a ser exibido.
  * @param ff Família da fonte 
  * @param fw Peso da fonte 
  * @param fs Tamanho da fonte 
  * 
  * @return Ponteiro para o TEXTO criado.
  */
 TEXTO criaTexto(int id, float x, float y, char* corB, char* corP, char ancora, char* txt, char* ff, char* fw, char* fs);
 

 
 /**
  * @brief Obtém o ID do texto.
  */
 int getIDTexto(TEXTO t);
 
 /**
  * @brief Obtém a coordenada X da ancora 
  */
 float getXTexto(TEXTO t);
 
 /**
  * @brief Obtém a coordenada Y da ancora
  */
 float getYTexto(TEXTO t);
 
 /**
  * @brief Obtém o caractere de âncora do texto.
  */
 char getAncoraTexto(TEXTO t);
 
 /**
  * @brief Obtém a cor da borda do texto.
  */
 char* getCorBTexto(TEXTO t);
 
 /**
  * @brief Obtém a cor de preenchimento do texto.
  */
 char* getCorPTexto(TEXTO t);
 
 /**
  * @brief Obtém o conteúdo textual (string).
  */
 char* getTxtTexto(TEXTO t);
 
 /**
  * @brief Obtém a família da fonte do texto.
  */
 char* getFFTexto(TEXTO t);
 
 /**
  * @brief Obtém o peso da fonte do texto.
  */
 char* getFWTexto(TEXTO t);
 
 /**
  * @brief Obtém o tamanho da fonte 
  */
 char* getFSTexto(TEXTO t);
 

 
 /**
  * @brief Obtém a coordenada X inicial do texto
  */
 float getX1Texto(TEXTO t);
 

 
 /**
  * @brief Obtém a coordenada X fdo final do texto 
  */
 float getX2Texto(TEXTO t);
 
 
 /**
  * @brief Define o ID do texto.
  */
 void setIDTexto(TEXTO t, int id);
 
 /**
  * @brief Define a coordenada X do texto.
  */
 void setXTexto(TEXTO t, float x);
 
 /**
  * @brief Define a coordenada Y do texto.
  */
 void setYTexto(TEXTO t, float y);
 
 /**
  * @brief Define a cor da borda do texto.
  */
 void setCorBTexto(TEXTO t, char* corB);
 
 /**
  * @brief Define a cor de preenchimento do texto.
  */
 void setCorPTexto(TEXTO t, char* corP);
 
 /**
  * @brief Define o caractere de âncora do texto.
  */
 void setAncoraTexto(TEXTO t, char ancora);
 
 /**
  * @brief Define o conteúdo textual.
  */
 void setTxtTexto(TEXTO t, char* txt);
 
 /**
  * @brief Define a família da fonte do texto.
  */
 void setFFTexto(TEXTO t, char* ff);
 
 /**
  * @brief Define o peso da fonte.
  */
 void setFWTexto(TEXTO t, char* fw);
 
 /**
  * @brief Define o tamanho da fonte 
  */
 void setFSTexto(TEXTO t, char* fs);
 
 
 /**
  * @brief Retorna o comprimento de linha do texto conforme o especificado (10 * número de caracteres)
  */
 float comprimentoTexto(TEXTO t);
 
 /**
  * @brief Calcula a área aproximada ocupada pelo texto.
  * 
  * @return Área do Texto.
  */
 float calculoAreaTexto(TEXTO t);
 
 /**
  * @brief Libera a memória alocada para o objeto TEXTO.
  */
 void desalocaTexto(TEXTO t);
 
 #endif 
 