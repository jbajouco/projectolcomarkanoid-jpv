#include "tabuleiro.h"

void drawTabuleiro(int VLIM, int HLIM, char *video_mem){

				/*draw_line(10, 10, 10, VLIM-10, 150, video_mem);
				draw_line(HLIM-10, 10, HLIM-10, VLIM-10, 150, video_mem);
				draw_line(10, 10, HLIM-10, 10, 150, video_mem);*/
				drawRectangle(10, 10, 20, VLIM-10, 150, video_mem);
				drawRectangle(HLIM-20, 10, HLIM-10, VLIM-10, 150, video_mem);
				drawRectangle(10, 10, HLIM-10, 20, 150, video_mem);
				
				//drawRectangle((HLIM/2)-50, VLIM-20, (HLIM/2)+50, VLIM-10, 300, video_mem);

}

