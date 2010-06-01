#ifndef _TABULEIRO_H_
#define _TABULEIRO_H_

#include "utypes.h"
#include "video-graphics.h"
#include "sprite.h"
#include "pixmap.h"

#define BLOC_LRG	50
#define BLOC_ALT	10
#define BOLA_TAM	25

//desenha tabuleiro
void drawTabuleiro(int VLIM, int HLIM, char *video_mem);

//detector de colisoes
int colisao_bloco(int Bx, int By,int x,int y, Sprite *bola);

#endif


