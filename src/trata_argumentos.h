#ifndef TRATA_ARGUMENTOS_H
#define TRATA_ARGUMENTOS_H

/**
 * @file trata_argumentos.h
 * @brief Módulo para tratamento simples de argumentos da linha de comando.
 *
 * Este módulo fornece uma função para obter o valor associado a uma opção
 * passada via linha de comando no formato "-opcao valor".
 * 
 * Ele facilita a extração de argumentos em programas C, buscando uma opção
 * específica e retornando o valor que está imediatamente após ela nos argumentos.
 */

 /**
  * @brief Obtém o valor associado a uma opção passada via linha de comando.
  *
  * Procura pela opção "-option_name" no vetor de argumentos e retorna o valor
  * que está imediatamente após essa opção.
  *
  * @param argc Número de argumentos passados pela linha de comando.
  * @param argv Vetor de strings contendo os argumentos da linha de comando.
  * @param option_name Nome da opção a ser buscada (sem o prefixo '-').
  * 
  * @return Ponteiro para a string do valor da opção, ou NULL se não encontrada.
  */
char* get_option(int argc, char *argv[], char* option_name);

#endif 
