#ifndef TRATA_ARGUMENTOS_H
#define TRATA_ARGUMENTOS_H

/*
    Este módulo contém as funções que serão
    utilizadas para tratar os argumentos
    que serão passados para o programa
*/


// Declaração da função que estará no módulo tratador de argumentos
char *obter_valor_opcao(int argc, char *argv[], char *nome_opcao);

// Função para extrair o sufixo do comando (última string que não começa com '-')
char *obter_sufixo_comando(int argc, char *argv[]);

#endif 


