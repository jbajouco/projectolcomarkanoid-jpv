#ifndef _ARKANOID_H_
#define _ARKANOID_H_

#include <stdio.h>
#include <stdlib.h>		
#include <string.h>
#include <dos.h>
#include <time.h>

#include "utypes.h"
#include "io.h"
#include "tabuleiro.h"
#include "video-text.h"
#include "video-graphics.h"
#include "ints.h"
#include "timer.h"
#include "music.h"
#include "kbc.h"
#include "rtc.h"
#include "sprite.h"

 /** 
  *   Enumeracao das entradas utilizadas na 
  *   maquina de Estados.
  */
enum Estados{INICIO, PLAYING, PAUSE, FIM, REPEAT};

#define ESC			0x01
#define ESQUERDA 	0x1e
#define DIREITA 	0x20
#define ESPACO	 	0x39

#endif
