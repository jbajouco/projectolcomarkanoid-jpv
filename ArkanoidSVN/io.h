#ifndef _IO_
#define _IO_H_

#include <stdio.h>
#include <string.h>

/** @defgroup IO Input/Output
 * 
 * Modulo onde s�o definidas as fun��es
 * para a leitura e escrita do ficheiro com
 * as pontuacoes.
 * 
 */

/** Estrutura de um jogador para efeitos de pontu��o
 *
 */
typedef struct{
	char nome[16];
	int pontos;
	int tempo;
} jogador;
 
/**
  * Le o ficheiro de texto com as pontua��es
  *
  * @param f - Ficheiro a ser lido.
  * @param joga - Array com pontua��es
  */
FILE* readFile(FILE* f, jogador* joga);

/**
  * Escreve no ficheiro as pontu��es
  *
  * @param f Ficheiro a ser escrito.
  * @param joga - Array com pontua��es
  */
FILE* writeFile(FILE* f, jogador* joga);


/** 
  * Ordena a estrututa jogador por ordem de pontua��o
  *
  * @param joga - Array com pontua��es
  */
void insertionSort(jogador* joga);

/** @} end of IO*/
#endif
