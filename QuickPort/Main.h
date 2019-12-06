/* ----==== MAIN.H ====---- */

#ifndef MAIN_H
#define MAIN_H

/*-----------------
---- CONSTANTS ----
-----------------*/

#define NUMTEXTURES			12
#define PI					3.1415926535897932384626433f
#define DEGTORAD			0.01745329252f
#define ANGLEINC			0.05f
#define ANGLEMULT			20
#define INFINITY			99999999.9999f
#define WALL_LIMIT			2

#define ANGLE360			7200
#define ANGLE270			5400
#define ANGLE180			3600
#define ANGLE90				1800
#define ANGLE60				1200
#define ANGLE45				900
#define ANGLE40				800
#define ANGLE30				600
#define ANGLE20				400
#define ANGLE15				300

#define KeyDown(vk_code)	((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

/*------------------
---- STRUCTURES ----
------------------*/

class GameOptions {	
	public:
		unsigned int RESX;
		unsigned int RESY;
		unsigned int COLORDEPTH;
		unsigned int ZDEPTH;
		unsigned int TEXSIZE;
		bool NICEGRAPHICS;
		bool MIPMAP;
		bool FILTERING;
		bool FULLSCREEN;

		GameOptions(char *_filename);
};

#endif