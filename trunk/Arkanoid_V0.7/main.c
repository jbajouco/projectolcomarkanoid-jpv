#include "arkanoid.h"

//variavel para o scancode do teclado
volatile unsigned char scd = 0;

//variaveis para o rato
volatile unsigned char indice = 0;
volatile char array_bytes[3];

//Rotinas em assembly
void handler_teclado(void);
void handler_rato(void);

//musica
int notas[] = {Sol5, Sol5, Mi5, Fa5, Sol5, La5, Sol5, Fa5, Mi5, Re5, Mi5, Mi5, Do5, Re5, Mi5, Fa5, Mi5, Re5, Do5, Si4, La4, Do5, Fa5, La5, Sol5, Mi5, Fa5, Mi5, Re5, Do5, Re5, Re5, Do5, Si4, Do5};
int tempos[] = {500, 500, 300, 300, 300, 300, 500, 500, 500, 800, 500, 500, 300, 300, 300, 300, 500, 500, 500, 800, 300, 300, 500, 500, 800, 800, 300, 300, 300, 300, 300, 500, 300, 300, 800};
Song song = {sizeof(tempos)/sizeof(int), 10, notas, tempos};
//int notas[] = {Do5, Mi5, Do5, Fa5, La5, Sol5, Sol4, Si4, Re5, Fa5, Fa5, Mi5, Re5};
//int tempos[] = {500, 300, 300, 500, 300, 800, 300, 300, 300, 300, 300, 300, 300};
//Song song = {sizeof(tempos)/sizeof(int), 10, notas, tempos};
int state = FIM;

//rotinas do rtc
void rtc_intc(void)
{	
	static int n_note = 0, time_elapsed = 0;
	
	outportb(RTC_ADDR_REG, RTC_STAT_C);
	Byte causa = inportb(RTC_DATA_REG);
	
	if (causa & RTC_PF)
		time_elapsed++;
	
	switch(state)
	{
		case INICIO :
			state = PLAYING;
			programa_timer(song.notes[n_note]);
			speaker_on();
			break;
		case PLAYING: 
			if (song.time[n_note] < time_elapsed)
			{
				state = PAUSE; 
				time_elapsed = 0;
				n_note++; 
				speaker_off();
			}
			break;
		case PAUSE:
			if (song.pause < time_elapsed) 
				time_elapsed = 0;
			if (n_note > song.lenght) 
				state = REPEAT;
			else
			{
				state = PLAYING;
				programa_timer(song.notes[n_note]);
				speaker_on();
			}
			break;
		case REPEAT:
			time_elapsed = 0;
			n_note = 0;
			state = INICIO;
            break;
		case FIM:
			speaker_off();
			time_elapsed = 0;
			n_note = 0;
			break;
	}	
	outportb(PIC1_CMD, EOI);
	outportb(PIC2_CMD, EOI);
}

//apresentacao
void arkaprint(){
	printf("                      M`                                              MM   \n");
	printf("      MM              M                                               Mh   \n");
	printf("     MNM              M                                    NM         M`   \n");
	printf("    s .M             .M                                               M    \n");
	printf("    h  Md            /M                                               M    \n");
	printf("   M   sM     sM M M md M` Mo  M   Mo   Mo M NM    M  `Md  Mo   /M    M    \n");
	printf("  MMMMMMM     hNM    MoN  .M   M   Mh   M`M  `M   M    -M  M`  .M    MM    \n");
	printf(" sN     MM    ms     M- M/     M/  M-   Mm   sM  -M    `M  M   Mo   /-M    \n");
	printf(" M      .M    MM     MM  M`   M-  mM`   Mh   mM   M    N-  M`  MM  M /M    \n");
	printf("Mh       MMMM Mh     M`   MMMdMMMd NM N M    sMMM /MN M    MMMd.MMM  `MMM  \n");
}

//menu e submenus
void menu() {
	//printf("\n ARKANOID\n\n");
	arkaprint();
	printf(" \n ----- Menu de Opcoes -----\n\n");
	printf(" 1 - Jogar\n");
	printf(" 2 - Instrucoes\n");
	printf(" 3 - Opcoes\n");
	printf(" 4 - Pontuacoes\n");
	printf(" 0 - Sair\n");
	printf(" \n");
	printf(" Escolha a sua opcao: ");
}

void instrucoes() {
	printf(" \n ----- Instrucoes -----\n\n");
	printf(" O jogo e formado por uma bola, uma raquete e blocos coloridos.\n");
	printf(" A bola e atirada e comeca a andar pelo ecra reflectindo\n");
	printf(" sempre que bate num obstaculo.\n");
	printf(" Com a raquete tenta-se bater na bola, movimentando-se para os lados.\n");
	printf(" A bola ao bater nos blocos faz somar pontos e\n");
	printf(" o nivel termina quando nao houver mais blocos.\n");
	printf(" \n Prima enter para continuar\n");
}

void submenu() {
	printf(" \n ----- Opcoes -----\n\n");
	printf(" 1 - Musica e efeitos sonoros on/off\n");
	printf(" 2 - Velocidade da bola\n");
	printf(" ? - Tecla secreta\n");
	printf(" 0 - Retroceder\n");
	printf(" \n");
	printf(" Escolha a sua opcao: ");
}


int colisao_bloco(int Bx, int By,int x,int y, Sprite *bola)
{

	// 11-0
	
	//detectar embates nos lados
	if (y >= By+BLOC_ALT && x+BOLA_TAM >= Bx && x <= Bx+BLOC_LRG){
		bola->yspeed = - bola->yspeed; // inv y POS6
		return 1;
		}
	if (x+BOLA_TAM >= Bx && y <= By+BLOC_ALT && y+BOLA_TAM >= By){
		bola->xspeed = -bola->xspeed; //inv x POS5
		return 1;
		}
	if (x <= Bx+BLOC_LRG && y <= By+BLOC_ALT && y+BOLA_TAM >= By){
		bola->xspeed = -bola->xspeed; //inv x POS7
		return 1;
		}
	if (y+BOLA_TAM >= By && x+BOLA_TAM >= Bx && x <= Bx+BLOC_LRG){
		bola->yspeed = - bola->yspeed; // inv y POS8
		return 1;
		}
	//detectar embates nos vertices
	if (x+BOLA_TAM == Bx && y == By+BLOC_ALT){
		bola->xspeed = -bola->xspeed; //inv x,y POS1
		bola->yspeed = -bola->yspeed;
		return 1;
		}
	if (x == Bx+BOLA_TAM && y == By+BLOC_ALT){
		bola->xspeed = -bola->xspeed; //inv x,y POS2
		bola->yspeed = -bola->yspeed;
		return 1;
		}
	if (x == Bx+BOLA_TAM && y+BOLA_TAM == By){
		bola->xspeed = -bola->xspeed; //inv x,y POS3
		bola->yspeed = -bola->yspeed;
		return 1;
		}
	if (x+BOLA_TAM == Bx && y+BOLA_TAM == By){
		bola->xspeed = -bola->xspeed; //inv x,y POS POS4
		bola->yspeed = -bola->yspeed;
		return 1;
		}
	return 0;
}

int main()
{
	//Variaveis
	//---------------------------------------------------------------------
	
	//modo gráfico
	__dpmi_meminfo map;
	char *video_mem;
	int mode=0x101;
	int HLIM=640, VLIM=480;
	
	//interrupções
	_go32_dpmi_seginfo old_irq_kbd, old_irq_mouse, old_irq_rtc, new_irq_rtc;
	
	//controlo da musica (on/off)
	int sfx=1;
	
	//notas intro
	int notas_intro[] = {Sol4, La4, Si4};
	int tempos_intro[] = {300, 300, 300};
	Song intro = {sizeof(tempos_intro)/sizeof(int), 10, notas_intro, tempos_intro };
	
	//musica final (game over)
	int notas_final[] = {La5, Mi5, Mi5, Fas5, Mi5, Sols5, La5};
	int tempos_final[] = {500, 300, 300, 300, 800, 300, 300};
	Song final = {sizeof(tempos_final)/sizeof(int), 5, notas_final, tempos_final };
	
	//musica ao perder uma vida
	int notas_vida[] = {Do6, La5, Fa5, Re5};
	int tempos_vida[] = {100, 100, 100, 100};
	Song vida = {sizeof(tempos_vida)/sizeof(int), 3, notas_vida, tempos_vida };
	
	//som ao bater nos tijolos
	int notas_bate[] = {Sol6};
	int tempos_bate[] = {5};
	Song bate = {sizeof(tempos_bate)/sizeof(int), 10, notas_bate, tempos_bate };
	
	//tempo e delay
	clock_t ti, tf;
	double tempo_gasto;
	time_t start;
	time_t current;
	
	//REAL TIME CLOCK
	RTC_TIME tempo_init, tempo_current, tempo_end;
	Byte c;
	float tempo_inicial = 0, tempo = 0, tempo_corrente = 0, tempo_fim = 0;
	
	//jogador
	jogador joga[10];
	jogador j;
	int vidas=3;
	
	//sprites
	Sprite *bola;
	Sprite *raquete;
	//Sprite **blocos;
	
	//ficheiro de pontuações
	FILE * f;
	
	//menu
	int opcao=-1, op=-1;
	
	//incremento
	int i=0;
	
	//Corpo do Programa
	//---------------------------------------------------------------------
	
	//inicializa o vector
	while (i<10) {
		joga[i].pontos=0;
		joga[i].tempo=0;
		strcpy(joga[i].nome, "Nome");
		i++;
	}
	
	//abrir ficheiro para leitura com as pontuações, colocar no array e ordenar
	f = fopen("pontuacoes.txt", "r");
	if(f == NULL)
		return 1;
	readFile(f, joga);
	fclose(f);
	insertionSort(joga);
	
	//inicia a musica do menu
	//play_song(&intro);
	
	//inicializacao do handler RTC
	mask_pic(RTC_IRQ);
	install_c_irq_handler(RTC_IRQ,&rtc_intc,&old_irq_rtc,&new_irq_rtc);
	c = read_rtcv(RTC_STAT_B);
	c = c | RTC_PIE | RTC_AIE | RTC_UIE;
	write_rtcv(RTC_STAT_B , c);
	unmask_pic(RTC_IRQ);
	
	state = INICIO;
	
	//menu
	do {
		menu();
		scanf("%i", &opcao);
		switch (opcao)
		{
			case 1:
				printf("\n Escolha uma das seguintes resolucoes:\n");
				printf(" 1-640x480\n");
				printf(" 2-800x600\n");
				printf(" 3-1024x768\n");
				printf(" 4-1280x1024\n");
				scanf("%i", &op);
				switch (op)
				{
					case 1:
						mode=0x101;
						HLIM=640;
						VLIM=480;
						break;
					case 2:
						mode=0x103;
						HLIM=800;
						VLIM=600;
						break;
					case 3: 
						mode=0x105;
						HLIM=1024;
						VLIM=768;
						break;
					case 4: 
						mode=0x107;
						HLIM=1280;
						VLIM=1024;
						break;
					default: 
						mode=0x101;
						HLIM=640;
						VLIM=480;
						break;
				}
				
				//termina musica e (re)inicia handlers
				state = FIM;
				speaker_off();
				reinstall_c_irq_handler(RTC_IRQ,&old_irq_rtc,&new_irq_rtc);
				
				//teclado e rato
				mask_pic(KBD_IRQ);
				mask_pic(MOUSE_IRQ);
				install_asm_irq_handler(KBD_IRQ,handler_teclado,&old_irq_kbd);
				if(kbc_init(0) != 1)
				{
					printf(" ERRO. Não detectou rato.");
			       	mouse_disable();
				}
				install_asm_irq_handler(MOUSE_IRQ,handler_rato,&old_irq_mouse);
				unmask_pic(MOUSE_IRQ);
				unmask_pic(KBD_IRQ);
				
				//entra em modo grafico
				clear();
				video_mem=enter_graphics(mode, &map);
				
				//musica do jogo
				if (sfx==1) {
					
					mask_pic(RTC_IRQ);
					install_c_irq_handler(RTC_IRQ,&rtc_intc,&old_irq_rtc,&new_irq_rtc);
					c = read_rtcv(RTC_STAT_B);
					c = c | RTC_PIE | RTC_AIE | RTC_UIE;
					write_rtcv(RTC_STAT_B , c);
					unmask_pic(RTC_IRQ);
					state = INICIO;
				}
				
				//leitura do tempo
				mask_pic(RTC_IRQ);
				rtc_read_time(&tempo_init);
				tempo_inicial = tempo_init.hour*3600 + tempo_init.min*60 + tempo_init.sec;
				unmask_pic(RTC_IRQ);
				ti = clock();
				
				//desenha tabuleiro
				drawTabuleiro(VLIM, HLIM, video_mem);
				int posBx = 20;
				int posBy = 100;
				
				//cria sprites e inicializa variaveis
				vidas=3;
				bola = create_sprite(bol);
				raquete = create_sprite(bar);
				Sprite *bloc[4] = {	create_sprite(block),
									create_sprite(block),
									create_sprite(block),
									create_sprite(block)};
									
				for (i=0; i < 4; i++){
					bloc[i]->x = posBx + (i+1)*BLOC_LRG;
					bloc[i]->y = posBy;
					draw_sprite(bloc[i], video_mem);
				}
				
				bola->x = HLIM/2;
				bola->y = VLIM/2;
				raquete->x = HLIM/2 - 32 ;
				raquete->y = VLIM - 30;
				
				bola->xspeed = 0;
				bola->yspeed = 0;
				raquete->xspeed = 0;
				raquete->yspeed = 0;
				
				//draw_sprite_scaled(raquete, video_mem, 70, 15);
				draw_sprite(bola, video_mem);
				draw_sprite(raquete, video_mem);				
				
				//bate no tijolo
				if (sfx==1) play_song(&bate);
				
				//perde vida
				if (sfx==1) play_song(&vida);
				vidas--;
				
				scd = 0;
				mask_pic(RTC_IRQ);
				rtc_read_time(&tempo_current);
				tempo=tempo_current.hour*3600 + tempo_current.min*60 + tempo_current.sec;
				
				//jogo
				while ((scd != ESC) && (vidas>0)) {
					
					//leitura do tempo corrente
					rtc_read_time(&tempo_current);
					tempo_corrente = tempo_current.hour*3600 + tempo_current.min*60 + tempo_current.sec;
					if (tempo_corrente-tempo>0.001) {
						rtc_read_time(&tempo_current);
						tempo=tempo_current.hour*3600 + tempo_current.min*60 + tempo_current.sec;
						
					}
					animate_sprite(bola, video_mem);
					for (i = 0; i < sizeof(bloc); i++)
						if ( colisao_bloco(posBx+(i*BLOC_LRG), posBy, bola->x, bola->y, bola) )
							
							delete_sprite( bloc[i], video_mem);
							
					//detector de colisoes da bola (na raquete e limites do cenario)
					if (bola->x == 10) {
						bola->xspeed = -bola->xspeed;
						play_song(&bate);
					}
					if (bola->y == 10) { 
						bola->yspeed = -bola->yspeed;
						play_song(&bate);
					}
					if (bola->x + 31 >= HLIM - 20) {
						bola->xspeed = -bola->xspeed;
						play_song(&bate);
					}
					if ( (bola->x + 15 >= raquete->x) && (bola->x + 18 <= raquete->x+32))
						if (bola->y + 31 >= VLIM - 30) {
							bola->yspeed = -bola->yspeed;
							play_song(&bate);
						}
					if (bola->y + 31 >= VLIM - 10)
						{
							bola->yspeed = -bola->yspeed;
							play_song(&bate);
						}
					
					switch (scd)
					{
						case ESQUERDA:
							//printf("Tecla e");
							if (raquete->x <= 20) break;
							else raquete->xspeed = -1;
							animate_sprite(raquete, video_mem);
							break;
						case DIREITA:
							//printf("Tecla d");
							if (raquete->x + 32 >= HLIM - 20) break;
							else raquete->xspeed = 1;
							animate_sprite(raquete, video_mem);
							break;
						case ESPACO:
							bola->xspeed = -1;
							bola->yspeed = -1;
							//printf("Tecla s");
							break;
						default: 
							
						break;
					}
					
					//scd=0;
				}
				unmask_pic(RTC_IRQ);
				//printf("Video memory address %c\n", video_mem);
				
				//leitura do tempo final
				mask_pic(RTC_IRQ);
				rtc_read_time(&tempo_end);
				tempo_fim = tempo_end.hour*3600 + tempo_end.min*60 + tempo_end.sec;
				unmask_pic(RTC_IRQ);
				j.tempo=tempo_fim-tempo_inicial;
				tf = clock();
				tempo_gasto = ( (double) (tf - ti) ) / CLOCKS_PER_SEC;
				printf(" Tempo gasto: %lf s\n", tempo_gasto);
				
				//Sai do modo gráfico
				leave_graphics(map);
				
				//repoe handlers
				mask_pic(KBD_IRQ);
				mask_pic(MOUSE_IRQ);
				reinstall_asm_irq_handler(KBD_IRQ,&old_irq_kbd);    
				reinstall_asm_irq_handler(MOUSE_IRQ,&old_irq_mouse);
				unmask_pic(MOUSE_IRQ);
				unmask_pic(KBD_IRQ);
				
				//mostra estatisticas
				printf(" Tempo: %i\n", j.tempo);
				printf(" Pontos: %i\n", j.pontos);
				
				//escreve no ficheiro a pontução
				play_song(&final);
				printf(" Introduza o seu nome (limite maximo de 15 caracteres): ");
				scanf("%s", j.nome);
				getchar();
				
				i=0;
				while ((i<10) && (joga[i].tempo!=0) && (joga[i].pontos!=0)) {
					i++;
				}
				
				if (i<10) {
					joga[i].tempo=j.tempo;
					joga[i].pontos=j.pontos;
					strcpy(joga[i].nome, j.nome);
					insertionSort(joga);
				}				
				
				f = fopen("pontuacoes.txt", "w");
				if(f == NULL)
					return 1;
				writeFile(f, joga);
				fclose(f);
				
				//inicializacao do handler RTC				
				mask_pic(RTC_IRQ);
				install_c_irq_handler(RTC_IRQ,&rtc_intc,&old_irq_rtc,&new_irq_rtc);
				c = read_rtcv(RTC_STAT_B);
				c = c | RTC_PIE | RTC_AIE | RTC_UIE;
				write_rtcv(RTC_STAT_B , c);
				unmask_pic(RTC_IRQ);
				
				//inicia a musica do menu
				play_song(&intro);
				state = INICIO;
				
				break;
			case 2:
				instrucoes();
				getchar();
				getchar();
				//clear();
				break;
			case 3:
				//clear();
				submenu();
				do {
					scanf("%i", &op);
					switch (op)
					{
						case 1:
							if (sfx==0) {
								sfx=1;
								printf(" SFX ON\n");
								state = INICIO;
								printf(" \n");
								printf(" Escolha a sua opcao: ");
							} else {
								sfx=0;
								printf(" SFX OFF\n");
								state = FIM;
								printf(" \n");
								printf(" Escolha a sua opcao: ");
							}
							break;
						case 2:
							printf(" 1 - Lenta\n");
							printf(" 2 - Normal\n");
							printf(" 3 - Rapida\n");
							scanf("%i", &opcao);
							break;
						case 7:
							printf(" Parabens! Achou a tecla secreta! Prima Enter");
							getchar();
							clear();
							break;
						case 0:
							printf(" OK\n");
							break;
						default:
							clear();
							printf(" Opcao invalida. Por favor escolha uma nova opcao.\n");
							submenu();
							break;
					}
				} while (op !=0);
				//clear();
				break;
			case 4:
				printf(" \n ----- Pontuacoes -----\n\n");
				printf(" Nome            Pontos   Tempo\n");
				int p=0;
				for (p=0; p<10;p++) {
					printf(" %s %i %i\n", joga[p].nome, joga[p].pontos, joga[p].tempo);
				}
				printf(" \n Prima uma tecla para continuar\n");
				getchar();
				getchar();
				//clear();
				break;
			case 0: 
				printf(" Saiu\n");
				break;
			default:
				printf(" Opcao invalida. Por favor escolha uma nova opcao.\n");
				clear();
				break;
		}
	} while (opcao !=0);
	
	state = FIM;
	speaker_off();
	
	//delay de 1 segundo
	time(&start);
	do{
		time(&current);
	}while(difftime(current,start) < 1.0);
	
	return 0;
	
}
