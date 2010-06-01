#include "tabuleiro.h"

void drawTabuleiro(int VLIM, int HLIM, char *video_mem){

				/*draw_line(10, 10, 10, VLIM-10, 150, video_mem);
				draw_line(HLIM-10, 10, HLIM-10, VLIM-10, 150, video_mem);
				draw_line(10, 10, HLIM-10, 10, 150, video_mem);*/
				
				//barra esquerda
				drawRectangle(20, 25, 40, VLIM-10, 150, video_mem);
				//barra direita
				drawRectangle(HLIM-40, 25, HLIM-20, VLIM-10, 150, video_mem);
				//barra topo
				drawRectangle(20, 30, HLIM-20, 50, 150, video_mem);
				
				//drawRectangle((HLIM/2)-50, VLIM-20, (HLIM/2)+50, VLIM-10, 300, video_mem);

}

int colisao_bloco(int Bx, int By,int x,int y, Sprite *bola)
{
	// 11-0
	
	//detectar embates nos lados
	if (y >= By+BLOC_ALT && x+BOLA_TAM >= Bx && x <= Bx+BLOC_LRG)
		bola->yspeed = - bola->yspeed; // inv y POS6
		
	if (x+BOLA_TAM >= Bx && y <= By+BLOC_ALT && y+BOLA_TAM >= By) bola->xspeed = -bola->xspeed; //inv x POS5
	if (x <= Bx+BLOC_LRG && y <= By+BLOC_ALT && y+BOLA_TAM >= By) bola->xspeed = -bola->xspeed; //inv x POS7
	if (y+BOLA_TAM >= By && x+BOLA_TAM >= Bx && x <= Bx+BLOC_LRG) bola->yspeed = - bola->yspeed; // inv y POS8
	
	//detectar embates nos vertices
	if (x+BOLA_TAM == Bx && y == By+BLOC_ALT){
		bola->xspeed = -bola->xspeed; //inv x,y POS1
		bola->yspeed = -bola->yspeed;
		}
	if (x == Bx+BOLA_TAM && y == By+BLOC_ALT){
		bola->xspeed = -bola->xspeed; //inv x,y POS2
		bola->yspeed = -bola->yspeed;
		}
	if (x == Bx+BOLA_TAM && y+BOLA_TAM == By){
		bola->xspeed = -bola->xspeed; //inv x,y POS3
		bola->yspeed = -bola->yspeed;
		}
	if (x+BOLA_TAM == Bx && y+BOLA_TAM == By){
		bola->xspeed = -bola->xspeed; //inv x,y POS POS4
		bola->yspeed = -bola->yspeed;
		}
	return 0;
}
