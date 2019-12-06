/* ----==== ENGINE.CPP ====---- */

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include <math.h>
#include "engine.h"
#include "main.h"
#include "object.h"
#include "world.h"
#include "particle.h"
#include "light.h"
#include "texture.h"
#include "frustrum.h"
#include "3d.h"

/*-----------------
---- VARIABLES ----
-----------------*/

Level	*GameLevel;
Player	*P;
float	*Sin;
float	*Cos;
float	*Tan;

extern GameOptions Options;

extern Mesh3d *mesh;
// temp
//extern float	TimeFix;
//Point3D			cam;

/*-----------------
---- FUNCTIONS ----
-----------------*/

void BuildLevel(void);

////////// Global Functions //////////

void LoadTextures(void)
{
	GameLevel->TextureList[0].LoadTexture("data/textures/wall_01.bmp",1,false);
	GameLevel->TextureList[1].LoadTexture("data/textures/005.bmp",1,false);
	GameLevel->TextureList[2].LoadTexture("data/textures/floor_03.bmp",1,false);
	GameLevel->TextureList[3].LoadTexture("data/textures/floor_06.bmp",1,false);
	GameLevel->TextureList[4].LoadTexture("data/textures/wall_06.bmp",1,false);
	GameLevel->TextureList[5].LoadTexture("data/textures/020.bmp",1,false);
	GameLevel->TextureList[6].LoadTexture("data/textures/013.bmp",1,false);
	GameLevel->TextureList[7].LoadTexture("data/textures/grass1_mossy.bmp",1,false);
	GameLevel->TextureList[8].LoadTexture("data/textures/006.bmp",1,false);
	GameLevel->TextureList[9].LoadTexture("data/textures/dirt_01.bmp",1,false);
	GameLevel->TextureList[10].LoadTexture("data/textures/Light.bmp",1,true);
	GameLevel->TextureList[11].LoadTexture("data/textures/Particle.bmp",1,false);

/*	_finddata_t FileData;
	unsigned int Count = 0;

	_chdir("data/bmp/");
	
	long FilePos = _findfirst("*.bmp",&FileData);
	do {
		NumTextures++;				
	} while (!_findnext(FilePos,&FileData));
	
	Texture = new unsigned int[NumTextures];
	TextureName = new FileString[NumTextures];

	FilePos = _findfirst("*.bmp",&FileData);
	do {
		strcpy(TextureName[Count].Name,CharUpper(FileData.name));
		LoadTexture(TextureName[Count].Name,Count);
		Count++;
	} while (!_findnext(FilePos,&FileData));

	_chdir("../../");*/
}

void SetupGame(void)
{
	// Build trig lookup tables
	Sin = new float[ANGLE360];
	Cos = new float[ANGLE360];
	Tan = new float[ANGLE360];

	for (int Angle = 0; Angle < ANGLE360; Angle++) {
		Sin[Angle] = (float)sin((float)Angle * ANGLEINC * DEGTORAD);
		Cos[Angle] = (float)cos((float)Angle * ANGLEINC * DEGTORAD);
		if (Angle == ANGLE90 || Angle == ANGLE270) Tan[Angle] = INFINITY;
		else Tan[Angle] = (float)tan((float)Angle * ANGLEINC * DEGTORAD);
	}

/////////////////// BUILD A TEMPORARY LEVEL /////////////////////////////
	
	Wall	*TWallList;
	Portal	*TPortalList;
	Point2D	*TPointList;
	Sector	*TSectorList;
	Color	c = {0.15f,0.15f,0.15f};
	Point3D	o;
	
///// Sector 1 Stuff /////
	
	TWallList = new Wall(16,0,24,-8,0);
	TWallList->next = new Wall(24,-8,24,-40,0);
	TWallList->next->next = new Wall(-24,-24,-24,-8,0);
	TWallList->next->next->next = new Wall(-24,-8,-16,0,0);
	TWallList->next->next->next->next = new Wall(-16,0,16,0,0);

	TPortalList = new Portal(24,-40,6,-40,2,false,0,0,0);
	TPortalList->next = new Portal(6,-40,-6,-40,4,false,0,0,0);
	TPortalList->next->next = new Portal(-6,-40,-24,-40,3,false,0,0,0);
	TPortalList->next->next->next = new Portal(-24,-40,-24,-24,9,false,0,0,0);

	TPointList = new Point2D(16,0);
	TPointList->next = new Point2D(24,-8);
	TPointList->next->next = new Point2D(24,-40);
	TPointList->next->next->next = new Point2D(-24,-40);
	TPointList->next->next->next->next = new Point2D(-24,-8);
	TPointList->next->next->next->next->next = new Point2D(-16,0);

	TSectorList = new Sector(TWallList,TPortalList,TPointList,&c,0,20,1,2,1);

	o.x = 0;
	o.y = 6.65f;
	o.z = -1.25f;
	TSectorList->PSystem = new Particle_System("data/level/torch_03.pe",&o);
	o.x += 6;
	TSectorList->PSystem->next = new Particle_System("data/level/torch_01.pe",&o);
//	o.x += 6;
//	TSectorList->PSystem->next->next = new Particle_System("data/level/torch.pe",&o);
//	o.x += 6;
//	TSectorList->PSystem->next->next->next = new Particle_System("data/level/torch.pe",&o);

///// Sector 2 Stuff /////

	TWallList = new Wall(24,-40,6,-52,4);
	
	TPortalList = new Portal(6,-52,6,-46,5,false,0,0,0);
	TPortalList->next = new Portal(6,-46,6,-40,4,false,0,0,0);
	TPortalList->next->next = new Portal(6,-40,24,-40,1,false,0,0,0);

	TPointList = new Point2D(24,-40);
	TPointList->next = new Point2D(6,-52);
	TPointList->next->next = new Point2D(6,-40);

	TSectorList->next = new Sector(TWallList,TPortalList,TPointList,&c,0,10,2,2,1);

///// Sector 3 Stuff /////

	TWallList = new Wall(-6,-52,-24,-40,4);

	TPortalList = new Portal(-6,-40,-6,-46,4,false,0,0,0);
	TPortalList->next = new Portal(-6,-46,-6,-52,5,false,0,0,0);
	TPortalList->next->next = new Portal(-24,-40,-6,-40,1,false,0,0,0);

	TPointList = new Point2D(-6,-40);
	TPointList->next = new Point2D(-6,-52);
	TPointList->next->next = new Point2D(-24,-40);

	TSectorList->next->next = new Sector(TWallList,TPortalList,TPointList,&c,0,10,3,2,1);

///// Sector 4 Stuff /////

	TWallList = NULL;

	TPortalList = new Portal(6,-40,6,-46,2,false,0,5,0);
	TPortalList->next = new Portal(6,-46,-6,-46,5,false,0,5,0);
	TPortalList->next->next = new Portal(-6,-46,-6,-40,3,false,0,5,0);
	TPortalList->next->next->next = new Portal(-6,-40,6,-40,1,false,0,5,0);

	TPointList = new Point2D(6,-40);
	TPointList->next = new Point2D(6,-46);
	TPointList->next->next = new Point2D(-6,-46);
	TPointList->next->next->next = new Point2D(-6,-40);

	TSectorList->next->next->next = new Sector(TWallList,TPortalList,TPointList,&c,-1,10,4,2,1);

///// Sector 5 Stuff /////

	TWallList = NULL;

	TPortalList = new Portal(6,-46,6,-52,2,false,0,5,0);
	TPortalList->next = new Portal(6,-52,-6,-52,6,false,0,5,0);
	TPortalList->next->next = new Portal(-6,-52,-6,-46,3,false,0,5,0);
	TPortalList->next->next->next = new Portal(-6,-46,6,-46,4,false,0,5,0);

	TPointList = new Point2D(6,-46);
	TPointList->next = new Point2D(6,-52);
	TPointList->next->next = new Point2D(-6,-52);
	TPointList->next->next->next = new Point2D(-6,-46);

	TSectorList->next->next->next->next = new Sector(TWallList,TPortalList,TPointList,&c,-2,10,5,2,1);

///// Sector 6 Stuff /////

	TWallList = new Wall(6,-52,6,-58,4);
	TWallList->next = new Wall(-6,-58,-6,-52,4);

	TPortalList = new Portal(6,-58,-6,-58,7,false,0,0,0);
	TPortalList->next = new Portal(-6,-52,6,-52,5,false,0,5,0);

	TPointList = new Point2D(6,-52);
	TPointList->next = new Point2D(6,-58);
	TPointList->next->next = new Point2D(-6,-58);
	TPointList->next->next->next = new Point2D(-6,-52);

	TSectorList->next->next->next->next->next = new Sector(TWallList,TPortalList,TPointList,&c,-3,10,6,2,1);

///// Sector 7 Stuff /////

	TWallList = new Wall(6,-58,6,-64,4);
	TWallList->next = new Wall(-6,-64,-6,-58,4);

	TPortalList = new Portal(6,-64,-6,-64,8,false,0,0,0);
	TPortalList->next = new Portal(-6,-58,6,-58,6,false,0,5,0);

	TPointList = new Point2D(6,-58);
	TPointList->next = new Point2D(6,-64);
	TPointList->next->next = new Point2D(-6,-64);
	TPointList->next->next->next = new Point2D(-6,-58);

	TSectorList->next->next->next->next->next->next = new Sector(TWallList,TPortalList,TPointList,&c,-4,10,7,2,1);

	o.x = -5;
	o.y = 3;
	o.z = -61;
	TSectorList->next->next->next->next->next->next->PSystem = new Particle_System("data/level/torch_02.pe",&o);
	o.x = 5;
	TSectorList->next->next->next->next->next->next->PSystem->next = new Particle_System("data/level/torch_02.pe",&o);

///// Sector 8 Stuff /////

	TWallList = new Wall(6,-64,6,-72,4);
	TWallList->next = new Wall(-6,-72,-6,-64,4);

	TPortalList = new Portal(6,-72,-6,-72,13,false,0,0,0);
	TPortalList->next = new Portal(-6,-64,6,-64,7,false,0,5,0);

	TPointList = new Point2D(6,-64);
	TPointList->next = new Point2D(6,-72);
	TPointList->next->next = new Point2D(-6,-72);
	TPointList->next->next->next = new Point2D(-6,-64);

	TSectorList->next->next->next->next->next->next->next = new Sector(TWallList,TPortalList,TPointList,&c,-5,10,8,2,1);

///// Sector 9 Stuff /////

	TWallList = new Wall(-40,-40,-40,-33,4);
	TWallList->next = new Wall(-40,-33,-39,-28,4);
	TWallList->next->next = new Wall(-39,-28,-36,-25,4);
	TWallList->next->next->next = new Wall(-36,-25,-31,-24,4);
	TWallList->next->next->next->next = new Wall(-31,-24,-24,-24,4);

	TPortalList = new Portal(-24,-24,-24,-40,1,false,0,0,0);
	TPortalList->next = new Portal(-24,-40,-40,-40,10,false,0,5,0);

	TPointList = new Point2D(-24,-40);
	TPointList->next = new Point2D(-40,-40);
	TPointList->next->next = new Point2D(-40,-33);
	TPointList->next->next->next = new Point2D(-39,-28);
	TPointList->next->next->next->next = new Point2D(-36,-25);
	TPointList->next->next->next->next->next = new Point2D(-31,-24);
	TPointList->next->next->next->next->next->next = new Point2D(-24,-24);

	TSectorList->next->next->next->next->next->next->next->next = new Sector(TWallList,TPortalList,TPointList,&c,0,10,9,2,1);

///// Sector 10 Stuff /////

	TWallList = new Wall(-24,-40,-24,-53,4);
	TWallList->next = new Wall(-40,-53,-40,-40,4);

	TPortalList = new Portal(-24,-53,-40,-53,11,false,0,5,5);
	TPortalList->next = new Portal(-40,-40,-24,-40,9,false,0,5,5);

	TPointList = new Point2D(-24,-40);
	TPointList->next = new Point2D(-24,-53);
	TPointList->next->next = new Point2D(-40,-53);
	TPointList->next->next->next = new Point2D(-40,-40);

	TSectorList->next->next->next->next->next->next->next->next->next = new Sector(TWallList,TPortalList,TPointList,&c,1,10,10,2,1);

///// Sector 11 Stuff /////

	TWallList = new Wall(-24,-53,-24,-66,4);
	TWallList->next = new Wall(-40,-66,-40,-53,4);

	TPortalList = new Portal(-24,-66,-40,-66,12,false,0,5,5);
	TPortalList->next = new Portal(-40,-53,-24,-53,10,false,0,5,5);

	TPointList = new Point2D(-24,-53);
	TPointList->next = new Point2D(-24,-66);
	TPointList->next->next = new Point2D(-40,-66);
	TPointList->next->next->next = new Point2D(-40,-53);

	TSectorList->next->next->next->next->next->next->next->next->next->next = new Sector(TWallList,TPortalList,TPointList,&c,2,10,11,2,1);

	o.x = -39;
	o.y = 8;
	o.z = -59;
	TSectorList->next->next->next->next->next->next->next->next->next->next->PSystem = new Particle_System("data/level/torch_02.pe",&o);
//	o.y = 7.8;
//	TSectorList->next->next->next->next->next->next->next->next->next->next->PSystem->next = new Particle_System("data/level/smoke.pe",&o);

///// Sector 12 Stuff /////

	TWallList = new Wall(-24,-66,-24,-80,4);
	TWallList->next = new Wall(-40,-80,-40,-66,4);

	TPortalList = new Portal(-24,-80,-40,-80,14,false,0,5,5);
	TPortalList->next = new Portal(-40,-66,-24,-66,11,false,0,5,5);

	TPointList = new Point2D(-24,-66);
	TPointList->next = new Point2D(-24,-80);
	TPointList->next->next = new Point2D(-40,-80);
	TPointList->next->next->next = new Point2D(-40,-66);

	TSectorList->next->next->next->next->next->next->next->next->next->next->next = new Sector(TWallList,TPortalList,TPointList,&c,3,11,12,2,1);

///// Sector 13 Stuff /////

	TWallList = new Wall(6,-72,6,-80,4);
	TWallList->next = new Wall(-6,-80,-6,-72,4);
	
	TPortalList = new Portal(6,-80,-6,-80,16,false,0,5,5);
	TPortalList->next = new Portal(-6,-72,6,-72,8,false,0,5,5);

	TPointList = new Point2D(6,-72);
	TPointList->next = new Point2D(6,-80);
	TPointList->next->next = new Point2D(-6,-80);
	TPointList->next->next->next = new Point2D(-6,-72);

	TSectorList->next->next->next->next->next->next->next->next->next->next->next->next = new Sector(TWallList,TPortalList,TPointList,&c,-6,10,13,2,1);

///// Sector 14 Stuff /////

	TWallList = new Wall(-24,-80,-24,-94,4);
	TWallList->next = new Wall(-40,-94,-40,-80,4);

	TPortalList = new Portal(-24,-94,-40,-94,15,false,0,5,5);
	TPortalList->next = new Portal(-40,-80,-24,-80,12,false,0,5,5);

	TPointList = new Point2D(-24,-80);
	TPointList->next = new Point2D(-24,-94);
	TPointList->next->next = new Point2D(-40,-94);
	TPointList->next->next->next = new Point2D(-40,-80);

	TSectorList->next->next->next->next->next->next->next->next->next->next->next->next->next = new Sector(TWallList,TPortalList,TPointList,&c,4,12,14,2,1);

///// Sector 15 Stuff /////

	TWallList = new Wall(-24,-94,-24,-115,4);
	TWallList->next = new Wall(-24,-119,-24,-120,4);
	TWallList->next->next = new Wall(-24,-120,-40,-120,4);
	TWallList->next->next->next = new Wall(-40,-120,-40,-94,4);

	TPortalList = new Portal(-24,-115,-24,-119,19,false,0,5,4);
	TPortalList->next = new Portal(-40,-94,-24,-94,14,false,0,5,5);

	TPointList = new Point2D(-24,-94);
	TPointList->next = new Point2D(-24,-120);
	TPointList->next->next = new Point2D(-40,-120);
	TPointList->next->next->next = new Point2D(-40,-94);

	TSectorList->next->next->next->next->next->next->next->next->next->next->next->next->next->next = new Sector(TWallList,TPortalList,TPointList,&c,5,13,15,2,1);

///// Sector 16 Stuff /////

	TWallList = new Wall(6,-120,-6,-120,4);
	TWallList->next = new Wall(-6,-114,-6,-113,8);

	TPortalList = new Portal(6,-80,6,-120,17,false,0,5,4);
	TPortalList->next = new Portal(-6,-120,-6,-114,21,true,6,4,4);
	TPortalList->next->next = new Portal(-6,-113,-6,-80,18,false,0,5,4);
	TPortalList->next->next->next = new Portal(-6,-80,6,-80,13,false,0,5,4);

	TPointList = new Point2D(6,-80);
	TPointList->next = new Point2D(6,-120);
	TPointList->next->next = new Point2D(-6,-120);
	TPointList->next->next->next = new Point2D(-6,-80);

	TSectorList->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next = new Sector(TWallList,TPortalList,TPointList,&c,-7,20,16,9,1);

	o.x = 0;
	o.y = -6.5;
	o.z = -100;
	TSectorList->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->PSystem = new Particle_System("data/level/fountaininner_01.pe",&o);
	TSectorList->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->PSystem->next = new Particle_System("data/level/fountainouter_01.pe",&o);

///// Sector 17 Stuff /////

	TWallList = new Wall(24,-80,24,-120,4);
	TWallList->next = new Wall(24,-120,6,-120,4);
	TWallList->next->next = new Wall(6,-80,24,-80,4);

	TPortalList = new Portal(6,-120,6,-80,16,false,0,5,5);

	TPointList = new Point2D(24,-80);
	TPointList->next = new Point2D(24,-120);
	TPointList->next->next = new Point2D(6,-120);
	TPointList->next->next->next = new Point2D(6,-80);

	TSectorList->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next = new Sector(TWallList,TPortalList,TPointList,&c,-6.8f,20,17,7,1);

///// Sector 18 Stuff /////

	TWallList = new Wall(-6,-113,-7,-113,8);
	TWallList->next = new Wall(-22,-113,-23,-113,8);
	TWallList->next->next = new Wall(-23,-113,-23,-80,4);
	TWallList->next->next->next = new Wall(-23,-80,-6,-80,4);

	TPortalList = new Portal(-6,-80,-6,-113,16,false,0,5,5);
	TPortalList->next = new Portal(-7,-113,-22,-113,22,true,6,4,4);

	TPointList = new Point2D(-6,-80);
	TPointList->next = new Point2D(-6,-113);
	TPointList->next->next = new Point2D(-23,-113);
	TPointList->next->next->next = new Point2D(-23,-80);

	TSectorList->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next = new Sector(TWallList,TPortalList,TPointList,&c,-6.8f,20,18,7,1);

///// Sector 19 Stuff /////

	TWallList = new Wall(-23,-119,-24,-119,5);
	TWallList->next = new Wall(-24,-115,-23,-115,5);

	TPortalList = new Portal(-23,-115,-23,-119,20,false,0,0,4);
	TPortalList->next = new Portal(-24,-119,-24,-115,15,false,0,0,4);

	TPointList = new Point2D(-23,-115);
	TPointList->next = new Point2D(-23,-119);
	TPointList->next->next = new Point2D(-24,-119);
	TPointList->next->next->next = new Point2D(-24,-115);

	TSectorList->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next = new Sector(TWallList,TPortalList,TPointList,&c,5,12,19,2,5);

///// Sector 20 Stuff /////

	TWallList = new Wall(-7,-120,-23,-120,4);
	TWallList->next = new Wall(-23,-120,-23,-119,4);
	TWallList->next->next = new Wall(-23,-115,-23,-114,4);
	TWallList->next->next->next = new Wall(-23,-114,-22,-114,4);

	TPortalList = new Portal(-7,-114,-7,-120,21,false,0,4,4);
	TPortalList->next = new Portal(-23,-119,-23,-115,19,false,0,0,4);
	TPortalList->next->next = new Portal(-22,-114,-7,-114,22,false,0,4,4);

	TPointList = new Point2D(-7,-114);
	TPointList->next = new Point2D(-7,-120);
	TPointList->next->next = new Point2D(-23,-120);
	TPointList->next->next->next = new Point2D(-23,-114);

	TSectorList->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next = new Sector(TWallList,TPortalList,TPointList,&c,5,14,20,2,1);

///// Sector 21 Stuff /////

	TWallList = new Wall(-6,-120,-7,-120,5);
	TWallList->next = new Wall(-7,-114,-6,-114,5);

	TPortalList = new Portal(-6,-114,-6,-120,16,true,6,5,5);
	TPortalList->next = new Portal(-7,-120,-7,-114,20,false,0,5,5);

	TPointList = new Point2D(-6,-114);
	TPointList->next = new Point2D(-6,-120);
	TPointList->next->next = new Point2D(-7,-120);
	TPointList->next->next->next = new Point2D(-7,-114);

	TSectorList->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next = new Sector(TWallList,TPortalList,TPointList,&c,8,13,21,5,5);

///// Sector 22 Stuff /////

	TWallList = new Wall(-7,-113,-7,-114,5);
	TWallList->next = new Wall(-22,-114,-22,-113,5);

	TPortalList = new Portal(-7,-114,-22,-114,20,false,0,5,5);
	TPortalList->next = new Portal(-22,-113,-7,-113,18,true,6,5,5);

	TPointList = new Point2D(-7,-113);
	TPointList->next = new Point2D(-7,-114);
	TPointList->next->next = new Point2D(-22,-114);
	TPointList->next->next->next = new Point2D(-22,-113);

	TSectorList->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next->next = new Sector(TWallList,TPortalList,TPointList,&c,8,13,22,5,5);

//////////////////////////

	GameLevel = new Level("Demo Level");
	GameLevel->SectorList = TSectorList;
	GameLevel->LightList = new Light(0,-1,-100,30,1,1,1,16);
	GameLevel->LightList->next = new Light(-39,7,-59,15,0.8f,0.76f,0.72f,11);
	GameLevel->LightList->next->next = new Light(-8,10,-8,25,0.8f,0.8f,0.8f,1);
//	GameLevel->LightList->next->next->next = new Light(23,8,-119,15,0.25f,0.7f,0.4f,17);
	GameLevel->LightList->next->next->next = new Light(23,8,-119,15,0.8f,0.8f,0.9f,17);
	GameLevel->LightList->next->next->next->next = new Light(-15,10,-119,10,0.7f,0.2f,0.2f,20);
	GameLevel->LightList->next->next->next->next->next = new Light(5,3,-61,15,0.8f,0.76f,0.72f,7);
	GameLevel->LightList->next->next->next->next->next->next = new Light(-5,3,-61,15,0.8f,0.76f,0.72f,7);


	GameLevel->BuildLevel();

	P = new Player("Kiah",1,0,0,-6,0,0,0,20);
//	cam.x = 0;
//	cam.y = 6;
//	cam.z = -6;
////////////////////////////////////////////////////////////////////////////

	LoadTextures();
}

void FreeMemory(void)
{
	delete [] Sin;
	delete [] Cos;
	delete [] Tan;
	delete GameLevel;
	delete P;
}

void PlayGame(void)
{
	// Reset Sectors
	Sector *currents = GameLevel->SectorList;
	while (currents != NULL) {
		currents->rendered = false;
		currents = currents->next;
	}

	// Update player
	P->Input();
	P->Update();

	// Update lights
	if (P->light != NULL) if (P->light->on) P->light->Update(P->InSector,0);
	
	Light *currentl = GameLevel->LightList;
	while (currentl != NULL) {
		currentl->Update(currentl->InSector,0);
		currentl = currentl->next;
	}

	// Rotate and then translate to camera space
	glLoadIdentity();
	glRotatef(P->ZAngle * ANGLEINC, 0,0,1);
	glRotatef(P->XAngle * ANGLEINC, 1,0,0);
	glRotatef(P->YAngle * ANGLEINC, 0,1,0);

//	GameLevel->SkyBox->Render();

	glTranslatef(-P->viewp.x, -P->viewy, -P->viewp.z);
//	glTranslatef(-cam.x,-cam.y,-cam.z);

	// Start from the sector the player is in

	P->InSector->Render();
	mesh->Render();

//	frust.GetWorldPoints(&P->p,P->YAngle);
	// frustrum
/*	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glBegin(GL_LINES);
		glColor3f(1,1,1);
		glVertex3f(frust.ln[0].p1.x,P->y+1,frust.ln[0].p1.z);
		glVertex3f(frust.ln[0].p2.x,P->y+1,frust.ln[0].p2.z);

		glVertex3f(frust.ln[1].p1.x,P->y+1,frust.ln[1].p1.z);
		glVertex3f(frust.ln[1].p2.x,P->y+1,frust.ln[1].p2.z);
		
		glColor3f(1,0,0);
		glVertex3f(frust.ln[1].p2.x,P->y+1,frust.ln[1].p2.z);
		glVertex3f(frust.ln[0].p1.x,P->y+1,frust.ln[0].p1.z);
	glEnd();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);*/

	/// TEMP render lights
/*	glDisable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);
	currentl = GameLevel->LightList;
	while (currentl != NULL) {
///// Draw a little symbol for the light ///// temp
		glColor3f(currentl->c.r,currentl->c.g,currentl->c.b);
		glBegin(GL_QUADS);
			glVertex3f(currentl->p.x-0.2f,currentl->y,currentl->p.z);
			glVertex3f(currentl->p.x,currentl->y+0.2f,currentl->p.z);
			glVertex3f(currentl->p.x+0.2f,currentl->y,currentl->p.z);
			glVertex3f(currentl->p.x,currentl->y-0.2f,currentl->p.z);
			glVertex3f(currentl->p.x,currentl->y,currentl->p.z-0.2f);
			glVertex3f(currentl->p.x,currentl->y+0.2f,currentl->p.z);
			glVertex3f(currentl->p.x,currentl->y,currentl->p.z+0.2f);
			glVertex3f(currentl->p.x,currentl->y-0.2f,currentl->p.z);
		glEnd();
///////////////////////////////////////////////

/*		if (P->InSector == currentl->InSector) {
			if (Key[VK_UP]) currentl->p.z += 6*TimeFix;
			if (Key[VK_DOWN]) currentl->p.z -= 6*TimeFix;
			if (Key[VK_RIGHT]) currentl->p.x -= 6*TimeFix;
			if (Key[VK_LEFT]) currentl->p.x += 6*TimeFix;
			if (Key[VK_PRIOR]) currentl->y += 6*TimeFix;
			if (Key[VK_NEXT]) currentl->y -= 6*TimeFix;
			if (currentl->y > currentl->InSector->ceily) currentl->y = currentl->InSector->ceily;
			if (currentl->y < currentl->InSector->floory) currentl->y = currentl->InSector->floory;
		}*/
/*		currentl = currentl->next;
	}
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
*/
/*	if (GetAsyncKeyState(VK_UP)) cam.z -= 20 * TimeFix;
	if (GetAsyncKeyState(VK_DOWN)) cam.z += 20 * TimeFix;
	if (GetAsyncKeyState(VK_LEFT)) cam.x -= 20 * TimeFix;
	if (GetAsyncKeyState(VK_RIGHT)) cam.x += 20 * TimeFix;
	if (GetAsyncKeyState(VK_PRIOR)) cam.y -= 20 * TimeFix;
	if (GetAsyncKeyState(VK_NEXT)) cam.y += 20 * TimeFix;*/
}
