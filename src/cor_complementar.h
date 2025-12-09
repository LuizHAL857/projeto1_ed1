#ifndef COR_COMPLEMENTAR_H
#define COR_COMPLEMENTAR_H


/*Módulo com uma função utilitária que encontra uma cor complementar a outra no sistema hexadecimal
*/




/**
 * Produz a cor complementar no sistema hexadecimal
 * Recebe como argumento uma cor no sistema hexadecimal (com H)
 * caso input não seja reconhecido retorna uma cópia da string original
 * a cor complementar deve ser liberada quando for chamada
 */
char *cor_complementar( char *cor);


#endif