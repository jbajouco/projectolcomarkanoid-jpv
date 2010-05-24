#include "arkanoid.h"

//variavel para o scancode do teclado
volatile unsigned char scd = 0;

//variaveis para o rato
volatile unsigned char indice = 0;
volatile char array_bytes[3];

//Rotinas em assembly
void handler_teclado(void);
void handler_rato(void);

//musica do menu
int notas[] = {Do5, Mi5, Do5, Fa5, La5, Sol5, Sol4, Si4, Re5, Fa5, Fa5, Mi5, Re5};
int tempos[] = {500, 300, 300, 500, 300, 800, 300, 300, 300, 300, 300, 300, 300};
Song song = {sizeof(tempos)/sizeof(int), 10, notas, tempos};
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

void menu() {
	printf("\n ARKANOID\n\n");
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
	printf(" \n Prima uma tecla para continuar\n");
}

void submenu() {
	printf(" \n ----- Opcoes -----\n\n");
	printf(" 1 - Musica e efeitos sonoros on/off\n");
	printf(" ? - Tecla secreta\n");
	printf(" 0 - Retroceder\n");
	printf(" \n");
	printf(" Escolha a sua opcao: ");
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
	Song intro = {sizeof(tempos_intro)/sizeof(int), 6, notas_intro, tempos_intro };
	
	//musica final
	int notas_final[] = {Do4, Sol4, Fa4, Mi4, Re4, Do4};
	int tempos_final[] = {300, 300, 300, 300, 300, 1000};
	Song final = {sizeof(tempos_final)/sizeof(int), 6, notas_final, tempos_final };
	
	//som ao bater nos tijolos
	int notas_bate[] = {Sol4};
	int tempos_bate[] = {100};
	Song bate = {sizeof(tempos_bate)/sizeof(int), 10, notas_bate, tempos_bate };
	
	//delay
	time_t start;
	time_t current;
	
	//REAL TIME CLOCK
	RTC_TIME tempo_init, tempo_end;
	Byte c;
	int tempo_inicial = 0;
	int tempo_fim = 0;
	
	//jogador
	jogador joga[10];
	jogador j;
	
	//sprites
	Sprite *bola;
	Sprite *raquete;
	
	//ficheiro de pontuações
	FILE * f;
	
	//menu
	int opcao=-1, op=-1;
	
	//Corpo do Programa
	//---------------------------------------------------------------------
	
	//inicializa o vector
	int i=0;
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
	play_song(&intro);
	
	//inicializacao do handler RTC
	mask_pic(RTC_IRQ);
	install_c_irq_handler(RTC_IRQ,&rtc_intc,&old_irq_rtc,&new_irq_rtc);
	c = read_rtcv(RTC_STAT_B);
	c = c | RTC_PIE | RTC_AIE | RTC_UIE;
	write_rtcv(RTC_STAT_B , c);
	unmask_pic(RTC_IRQ);
	
	//state = INICIO;
		
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
				
				//leitura do tempo
				mask_pic(RTC_IRQ);
				rtc_read_time(&tempo_init);
				tempo_inicial = tempo_init.hour*3600 + tempo_init.min*60 + tempo_init.sec;
				unmask_pic(RTC_IRQ);
				
				//desenha tabuleiro
				drawTabuleiro(VLIM, HLIM, video_mem);
				bola = create_sprite(bol);
				raquete = create_sprite(bar);
				bola->x = 100;
				bola->y = 100;
				bola->xspeed = 1;
				bola->yspeed = 1;
				draw_sprite(bola, video_mem);

				scd = 0;
				while(scd != ESC) {
				
					if( scd == 0 ) continue;
					
					//printf("\r%d", scd);
					animate_sprite(bola, video_mem);
					switch (scd)
					{
						case ESQUERDA:
							printf("Tecla e");
							break;
						case DIREITA:
							printf("Tecla d");
							break;
						case ESPACO:
							printf("Tecla s");
							break;
						default: 
						break;
					}
					
					scd=0;
				}
				
				//printf("Video memory address %c\n", video_mem);
				//jogo();
				
				//leitura do tempo final
				mask_pic(RTC_IRQ);
				rtc_read_time(&tempo_end);
				tempo_fim = tempo_end.hour*3600 + tempo_end.min*60 + tempo_end.sec;
				unmask_pic(RTC_IRQ);
				j.tempo=tempo_fim-tempo_inicial;
				
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
				printf(" Tempo: %i\n ", j.tempo);
				printf(" Pontos: %i\n ", j.pontos);
				
				//escreve no ficheiro a pontução
				play_song(&final);
				printf(" Introduza o seu nome (limite maximo de 15 caracteres): ");
				scanf("%s", j.nome);
				getchar();
				f = fopen("pontuacoes.txt", "w");
				if(f == NULL)
					return 1;
				writeFile(f, joga);
				fclose(f);
				
				//inicia a musica do menu
				play_song(&intro);
				
				//inicializacao do handler RTC				
				mask_pic(RTC_IRQ);
				install_c_irq_handler(RTC_IRQ,&rtc_intc,&old_irq_rtc,&new_irq_rtc);
				c = read_rtcv(RTC_STAT_B);
				c = c | RTC_PIE | RTC_AIE | RTC_UIE;
				write_rtcv(RTC_STAT_B , c);
				unmask_pic(RTC_IRQ);
				
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
						case 7:
							printf(" Parabens! Achou a tecla secreta!");
							getchar();
							break;
						case 0:
							printf(" OK\n");
							break;
						default:
							printf(" Opcao invalida. Por favor escolha uma nova opcao.\n");
							clear();
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
