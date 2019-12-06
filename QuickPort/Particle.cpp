/* ----==== PARTICLE.CPP ====---- */

#include <windows.h>
#include <gl\gl.h>
//#include <gl\glu.h>
//#include <gl\glaux.h>
#include <math.h>
#include <fstream.h>
#include <string.h>
#include "particle.h"
//#include "engine.h"
#include "main.h"
#include "object.h"
#include "world.h"

/*-----------------
---- VARIABLES ----
-----------------*/

extern Player	*P;
extern float	*Sin;
extern float	*Cos;
extern float	*Tan;
extern float	TimeFix;

/*-----------------
---- FUNCTIONS ----
-----------------*/

////////// class Particle //////////

void Particle::Update(void)
{
	if (lifecounter <= 0) {
		alive = false;
		return;
	}

	pos += vel * TimeFix;
	vel += accel * TimeFix;

	c.r += deltac.r * TimeFix;
	c.g += deltac.g * TimeFix;
	c.b += deltac.b * TimeFix;

	lifecounter -= life * TimeFix;
}

void Particle::Render(void)
{
	glPushMatrix();

	glTranslatef(pos.x,pos.y,pos.z);
	glRotatef(-P->YAngle * ANGLEINC, 0,1,0);
	glRotatef(-P->XAngle * ANGLEINC, 1,0,0);

	glColor4f(c.r, c.g, c.b, lifecounter);
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2d(0,0);
		glVertex3f(-radius, radius, 0);

		glTexCoord2d(1,0);
		glVertex3f(-radius, -radius, 0);

		glTexCoord2d(0,1);
		glVertex3f(radius, radius, 0);

		glTexCoord2d(1,1);
		glVertex3f(radius, -radius, 0);
	glEnd();

	glPopMatrix();
}

void Particle::Reset(Point3D *_pos,Point3D *_vel,Point3D *_accel,Color *_ci,Color *_cf,float _life,float rad)
{
	pos = *_pos;
	vel = *_vel;
	accel = *_accel;
	c = *_ci;
	life = 1 / _life;
	lifecounter = 1;

	deltac.r = (_cf->r - c.r) * life;
	deltac.g = (_cf->g - c.g) * life;
	deltac.b = (_cf->b - c.b) * life;
	
	radius = rad;
	alive = true;
}

Particle::Particle(Point3D *_pos,Point3D *_vel,Point3D *_accel,Color *_ci,Color *_cf,float _life,float rad)
{
	next = NULL;
	pos = *_pos;
	vel = *_vel;
	accel = *_accel;
	c = *_ci;
	life = 1 / _life;
	lifecounter = 1;

	deltac.r = (_cf->r - c.r) * life;
	deltac.g = (_cf->g - c.g) * life;
	deltac.b = (_cf->b - c.b) * life;

	radius = rad;
	alive = true;
}

////////// class Particle_System //////////

void Particle_System::Update(void)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
//	glBlendFunc(GL_SRC_COLOR,GL_SRC_ALPHA);//_MINUS_SRC_COLOR); // smoke?
	glEnable(GL_BLEND);
//	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	glBindTexture(GL_TEXTURE_2D, 12);

	Particle *current = plist;
	while (current->next != NULL) {

		if (current->alive) {
			current->Render();
			current->Update();
		} else {
			float ls = lifespan + (rand()%20-10) * 0.1f * liferand;
			Point3D p, v;

			p.x = origin.x + orand.x * (float)(rand()%2000 - 1000) * 0.0001f;
			p.y = origin.y + orand.y * (float)(rand()%2000 - 1000) * 0.0001f;
			p.z = origin.z + orand.z * (float)(rand()%2000 - 1000) * 0.0001f;
			v.x = direction.x + drand.x * (float)(rand()%200 - 100) * 0.001f;
			v.y = direction.y + drand.y * (float)(rand()%200 - 100) * 0.001f;
			v.z = direction.z + drand.z * (float)(rand()%200 - 100) * 0.001f;

			current->Reset(&p,&v,&acceleration,&ci,&cf,ls,radius);
		}
		current = current->next;
	}

	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
//	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	if (pcount < maxparticles) {
		float ls = lifespan + (rand()%20-10) * 0.1f * liferand;
		Point3D p, v;

		p.x = origin.x + orand.x * (float)(rand()%2000 - 1000) * 0.0001f;
		p.y = origin.y + orand.y * (float)(rand()%2000 - 1000) * 0.0001f;
		p.z = origin.z + orand.z * (float)(rand()%2000 - 1000) * 0.0001f;
		v.x = direction.x + drand.x * (float)(rand()%200 - 100) * 0.001f;
		v.y = direction.y + drand.y * (float)(rand()%200 - 100) * 0.001f;
		v.z = direction.z + drand.z * (float)(rand()%200 - 100) * 0.001f;

		current->next = new Particle(&p,&v,&acceleration,&ci,&cf,ls,radius);
		pcount++;
	}
}

Particle_System::Particle_System(Point3D *_origin,Point3D *_orand,Point3D *_direction,Point3D *_drand,Point3D *_accel,Color *_ci,Color *_cf,float _radius,float _lifespan,float _liferand,int _maxparticles,bool _lighting)
{
	next = NULL;
	origin = *_origin;
	orand = *_orand;
	direction = *_direction;
	drand = *_drand;
	acceleration = *_accel;
	ci = *_ci;
	cf = *_cf;
	radius = _radius;
	lifespan = _lifespan;
	liferand = _liferand;
	maxparticles = _maxparticles;

	plist = new Particle(&origin,&direction,&acceleration,&ci,&cf,lifespan,radius);
	pcount = 1;
	lighting = _lighting;
}

Particle_System::Particle_System(const char *Filename,Point3D *_origin)
{
	char *Buffer = new char[80];

	ifstream inFile(Filename, ios::nocreate);

	if (!inFile) {
		MessageBox(0,Filename,"File not found",MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
		return;		
	}


	while (!inFile.eof()) {
		Buffer[0] = NULL;
		
		inFile.getline(Buffer,80);

		if (!strcmp(Buffer,"[POSRAND]")) {
			inFile >> orand.x;
			inFile.ignore();
			inFile >> orand.y;
			inFile.ignore();
			inFile >> orand.z;
			inFile.ignore();
		} else if (!strcmp(Buffer,"[DIRECTION]")) {
			inFile >> direction.x;
			inFile.ignore();
			inFile >> direction.y;
			inFile.ignore();
			inFile >> direction.z;
			inFile.ignore();
		} else if (!strcmp(Buffer,"[DIRRAND]")) {
			inFile >> drand.x;
			inFile.ignore();
			inFile >> drand.y;
			inFile.ignore();
			inFile >> drand.z;
			inFile.ignore();
		} else if (!strcmp(Buffer,"[ACCELERATION]")) {
			inFile >> acceleration.x;
			inFile.ignore();
			inFile >> acceleration.y;
			inFile.ignore();
			inFile >> acceleration.z;
			inFile.ignore();
		} else if (!strcmp(Buffer,"[COLORI]")) {
			inFile >> ci.r;
			inFile.ignore();
			inFile >> ci.g;
			inFile.ignore();
			inFile >> ci.b;
			inFile.ignore();
		} else if (!strcmp(Buffer,"[COLORF]")) {
			inFile >> cf.r;
			inFile.ignore();
			inFile >> cf.g;
			inFile.ignore();
			inFile >> cf.b;
			inFile.ignore();
		} else if (!strcmp(Buffer,"[LIGHT]")) {
			int lightingint;
			inFile >> lightingint;
			lighting = lightingint == 1 ? true : false;
		} else if (!strcmp(Buffer,"[RADIUS]")) {
			inFile >> radius;
		} else if (!strcmp(Buffer,"[LIFESPAN]")) {
			inFile >> lifespan;
		} else if (!strcmp(Buffer,"[LIFERAND]")) {
			inFile >> liferand;
		} else if (!strcmp(Buffer,"[NUMPARTICLES]")) {
			inFile >> maxparticles;
		}
	}

	inFile.close();

	delete [] Buffer;

	next = NULL;
	origin = *_origin;

	plist = new Particle(&origin,&direction,&acceleration,&ci,&cf,lifespan,radius);
	pcount = 1;
}

Particle_System::~Particle_System()
{
	Particle *current = plist, *temp;
	while (current != NULL) {
		temp = current;
		current = current->next;
		delete temp;
	}
}