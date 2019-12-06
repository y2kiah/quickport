/* ----==== WORLD.CPP ====---- */

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include <math.h>
#include "world.h"
#include "main.h"
#include "object.h"
#include "particle.h"
#include "light.h"
#include "texture.h"
#include "frustrum.h"

/*-----------------
---- VARIABLES ----
-----------------*/

extern GameOptions	Options;
extern Level		*GameLevel;
extern Player		*P;
extern bool			Key[];
extern float		TimeFix;
extern float		*Sin;
extern float		*Cos;
extern float		*Tan;

/*-----------------
---- FUNCTIONS ----
-----------------*/

////////// class Point3D //////////

/*const Point3D &Point3D::operator<<(const Point2D &p)
{
	x = p.x;
	z = p.z;

	return *this;
}*/

const Point3D &Point3D::operator+=(const Point3D &p)
{
	x += p.x;
	y += p.y;
	z += p.z;

	return *this;
}

/*const Point3D &Point3D::operator+=(const Point2D &p)
{
	x += p.x;
	z += p.z;

	return *this;
}*/

Point3D Point3D::operator*(const float s) const
{
	// Scales the point by a multiple of s
	Point3D n;
	n.x = x * s;
	n.y = y * s;
	n.z = z * s;
	
	return n;	
}

////////// class Point2D //////////

inline bool Point2D::operator!=(const Point2D &p) const
{
	if (p.x != x || p.z != z) return true;
	else return false;
}

Point2D Point2D::operator-(const Point2D &p) const
{
	Point2D n;
	n.x = x - p.x;
	n.z = z - p.z;
	return n;
}

const Point2D &Point2D::operator-=(const Point2D &p)
{
	x -= p.x;
	z -= p.z;

	return *this;
}

const Point2D &Point2D::operator+=(const Point2D &p)
{
	x += p.x;
	z += p.z;

	return *this;
}

float Point2D::operator*(const Point2D &p) const
{
	// Calculates dot product
	return (x * p.x) + (z * p.z);
}

Point2D Point2D::operator*(const float s) const
{
	// Scales the point by a multiple of s
	Point2D n;
	n.x = x * s;
	n.z = z * s;
	
	return n;	
}

const Point2D &Point2D::operator*=(const float s)
{
	x *= s;
	z *= s;

	return *this;
}

Point2D Point2D::operator%(const Point2D &p) const
{
	// Calculates normal vector using cross product
	Point2D n;
	n.x = p.z - z;
	n.z = x - p.x;

	// Normalize the vector to unit length
	float Length = sqrtf(n.x * n.x + n.z * n.z);
	float Inv_Length = Length == 0 ? 0 : 1.0f / Length;
	n.x = n.x * Inv_Length;
	n.z = n.z * Inv_Length;

	return n;
}

Point2D Point2D::UnitSlope(const Point2D &p) const
{
	// Calculates slope vector
	Point2D m;
	m.x = p.x - x;
	m.z = p.z - z;

	// Scale the vector to unit length
	float Length = sqrtf(m.x * m.x + m.z * m.z);
	float Inv_Length = Length == 0 ? 0 : 1.0f / Length;
	m.x = m.x * Inv_Length;
	m.z = m.z * Inv_Length;

	return m;
}

////////// class Line2D //////////

Point2D Line2D::FindInt(const Line2D &l) const
{
	Point2D p;
//	if (m == l.m) p.x = p.z = INFINITY; make sure to incorporate this into the calling function
	if (m == INFINITY) {
		p.x = b;
		p.z = l.m * p.x + l.b;
	} else if (l.m == INFINITY) {
		p.x = l.b;
		p.z = m * p.x + b;
	} else {
		p.x = (l.b - b) / (m - l.m);
		p.z = m * p.x + b;
	}
	return p;
}

float Line2D::FindDist(const Point2D &p) const
{
	return n * p - c;
}

void Line2D::Reset(float x1,float z1,float x2,float z2)
{
	p1.x = x1;	p1.z = z1;	p2.x = x2;	p2.z = z2;
	
	length = sqrtf((z2-z1)*(z2-z1) + (x2-x1)*(x2-x1));
	if (length == 0) length = 0.000001f;
	n.x = (z2 - z1) / length;
	n.z = (x1 - x2) / length;
	sl.x = (x2 - x1) / length;
	sl.z = (z2 - z1) / length;

	if (x2 == x1) m = INFINITY;
	else m = (z2 - z1) / (x2 - x1);

	if (m == INFINITY) b = x1;
	else b = -x1 * m + z1;

	c = n * p1;
}

void Line2D::Reset(const Point2D *_p1,const Point2D *_p2)
{
	p1.x = _p1->x;	p1.z = _p1->z;	p2.x = _p2->x;	p2.z = _p2->z;
	
	length = sqrtf((p2.z-p1.z)*(p2.z-p1.z) + (p2.x-p1.x)*(p2.x-p1.x));
	if (length == 0) length = 0.000001f;
	n.x = (p2.z - p1.z) / length;
	n.z = (p1.x - p2.x) / length;

	if (p2.x == p1.x) m = INFINITY;
	else m = (p2.z - p1.z) / (p2.x - p1.x);

	if (m == INFINITY) b = p1.x;
	else b = -p1.x * m + p1.z;

	c = n * p1;
}

void Line2D::Reset(const Point3D *_p1,const Point3D *_p2)
{
	p1.x = _p1->x;	p1.z = _p1->z;	p2.x = _p2->x;	p2.z = _p2->z;
	
	length = sqrtf((p2.z-p1.z)*(p2.z-p1.z) + (p2.x-p1.x)*(p2.x-p1.x));
	if (length == 0) length = 0.000001f;
	n.x = (p2.z - p1.z) / length;
	n.z = (p1.x - p2.x) / length;

	if (p2.x == p1.x) m = INFINITY;
	else m = (p2.z - p1.z) / (p2.x - p1.x);

	if (m == INFINITY) b = p1.x;
	else b = -p1.x * m + p1.z;

	c = n * p1;
}

Line2D::Line2D(float x1,float z1,float x2,float z2)
{
	p1.x = x1;	p1.z = z1;	p2.x = x2;	p2.z = z2;
	
	length = sqrtf((z2-z1)*(z2-z1) + (x2-x1)*(x2-x1));
	if (length == 0) length = 0.000001f;
	n.x = (z2 - z1) / length;
	n.z = (x1 - x2) / length;
	sl.x = (x2 - x1) / length;
	sl.z = (z2 - z1) / length;

	if (x2 == x1) m = INFINITY;
	else m = (z2 - z1) / (x2 - x1);

	if (m == INFINITY) b = x1;
	else b = -x1 * m + z1;

	c = n * p1;
}

Line2D::Line2D(const Point2D *_p1,const Point2D *_p2)
{
	p1.x = _p1->x;	p1.z = _p1->z;	p2.x = _p2->x;	p2.z = _p2->z;
	
	length = sqrtf((p2.z-p1.z)*(p2.z-p1.z) + (p2.x-p1.x)*(p2.x-p1.x));
	if (length == 0) length = 0.000001f;
	n.x = (p2.z - p1.z) / length;
	n.z = (p1.x - p2.x) / length;

	if (p2.x == p1.x) m = INFINITY;
	else m = (p2.z - p1.z) / (p2.x - p1.x);

	if (m == INFINITY) b = p1.x;
	else b = -p1.x * m + p1.z;

	c = n * p1;
}

////////// class Wall //////////

void Wall::Render(const float yf,const float yc,bool texture) const
{
	if (texture) {
		float ul = length / GameLevel->TextureList[texid].w_tex_dist;
		float vf = yf / GameLevel->TextureList[texid].h_tex_dist;
		float vc = yc / GameLevel->TextureList[texid].h_tex_dist;

		glBindTexture(GL_TEXTURE_2D, GameLevel->TextureList[texid].texture[0]);
		glBegin(GL_QUADS);
			glTexCoord2f(ul, vf);
			glVertex3f(p1.x, yf, p1.z);

			glTexCoord2f(ul, vc);
			glVertex3f(p1.x, yc, p1.z);

			glTexCoord2f(0, vc);
			glVertex3f(p2.x, yc, p2.z);

			glTexCoord2f(0, vf);
			glVertex3f(p2.x, yf, p2.z);
		glEnd();
	} else {
		glBegin(GL_QUADS);
			glVertex3f(p1.x, yf, p1.z);
			glVertex3f(p1.x, yc, p1.z);
			glVertex3f(p2.x, yc, p2.z);
			glVertex3f(p2.x, yf, p2.z);
		glEnd();
	}
}

Wall::Wall(float x1,float z1,float x2,float z2,int _texid) : Line2D(x1,z1,x2,z2)
{
	next = NULL;
	texid = _texid;
	visible = false;
}

////////// class Portal //////////

void Portal::LinkSector(void)
{
	Sector *current = GameLevel->SectorList;
	while (current != NULL) {
		if (current->id == targetid) {
			s = current;
			break;
		}
		current = current->next;
	}
}

void Portal::RenderSector(void)
{
	visible = true;

	if (FindDist(P->p) < 0) visible = false; // Is the player behind the portal?  If so, is the portal within view frustrum
	else if ((P->flist->list->ln[0].FindDist(p1) < 0 && P->flist->list->ln[0].FindDist(p2) < 0) || (P->flist->list->ln[1].FindDist(p1) < 0 && P->flist->list->ln[1].FindDist(p2) < 0)) visible = false;
//	else if ((P->flist->list->ln[0].FindDist(p1) > 0 && P->flist->list->ln[1].FindDist(p1) > 0) || (P->flist->list->ln[0].FindDist(p2) > 0 && P->flist->list->ln[1].FindDist(p2) > 0)) visible = true;
//	else if ((P->flist->list->ln[0].FindDist(p1) < 0 &&
// add above that must be > ln[2] also	

	if (visible) {
//		P->flist->PushCheck(&P->p,&p2,&p1);
		s->Render();
//		P->flist->Pop();
	}
}

void Portal::Render(const float yf,const float yc,bool texture) const
{
	float ul, vf, vc;
	float yfuse, ycuse, vfuse, vcuse;

	if (texture) {
		if (textured) {
			ul = length / GameLevel->TextureList[texid].w_tex_dist;

			yfuse = (yf < s->floory ? s->floory : yf);
			ycuse = (yc > s->ceily ? s->ceily : yc);
			vfuse = yfuse / GameLevel->TextureList[texid].h_tex_dist;
			vcuse = ycuse / GameLevel->TextureList[texid].h_tex_dist;

			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			glBindTexture(GL_TEXTURE_2D, GameLevel->TextureList[texid].texture[0]);

			glBegin(GL_QUADS);
				glTexCoord2f(ul, vfuse);
				glVertex3f(p1.x, yfuse, p1.z);

				glTexCoord2f(ul, vcuse);
				glVertex3f(p1.x, ycuse, p1.z);

				glTexCoord2f(0, vcuse);
				glVertex3f(p2.x, ycuse, p2.z);

				glTexCoord2f(0, vfuse);
				glVertex3f(p2.x, yfuse, p2.z);
			glEnd();
		}

		glBlendFunc(GL_DST_COLOR,GL_ZERO);
		if (s->floory > yf) {
			ul = length / GameLevel->TextureList[ltexid].w_tex_dist;
			vf = yf / GameLevel->TextureList[ltexid].h_tex_dist;

			glBindTexture(GL_TEXTURE_2D, GameLevel->TextureList[ltexid].texture[0]);

			glBegin(GL_QUADS);
				glTexCoord2f(ul, vf);
				glVertex3f(p1.x, yf, p1.z);

				glTexCoord2f(ul, vf + (s->floory-yf) / GameLevel->TextureList[ltexid].h_tex_dist);
				glVertex3f(p1.x, s->floory, p1.z);
					
				glTexCoord2f(0, vf + (s->floory-yf) / GameLevel->TextureList[ltexid].h_tex_dist);
				glVertex3f(p2.x, s->floory, p2.z);
	
				glTexCoord2f(0, vf);
				glVertex3f(p2.x, yf, p2.z);
			glEnd();
		}		
		if (s->ceily < yc) {
			ul = length / GameLevel->TextureList[utexid].w_tex_dist;
			vc = yc / GameLevel->TextureList[utexid].h_tex_dist;

			glBindTexture(GL_TEXTURE_2D, GameLevel->TextureList[utexid].texture[0]);

			glBegin(GL_QUADS);
				glTexCoord2f(ul, vc);	
				glVertex3f(p1.x, yc, p1.z);

				glTexCoord2f(0, vc);
				glVertex3f(p2.x, yc, p2.z);

				glTexCoord2f(0, vc - (yc-s->ceily) / GameLevel->TextureList[utexid].h_tex_dist);
				glVertex3f(p2.x, s->ceily, p2.z);

				glTexCoord2f(ul, vc - (yc-s->ceily) / GameLevel->TextureList[utexid].h_tex_dist);
				glVertex3f(p1.x, s->ceily, p1.z);
			glEnd();
		}
	} else {
		if (s->floory > yf) {
			glBegin(GL_QUADS);
				glVertex3f(p1.x, yf, p1.z);
				glVertex3f(p1.x, s->floory, p1.z);
				glVertex3f(p2.x, s->floory, p2.z);
				glVertex3f(p2.x, yf, p2.z);
			glEnd();
		}		
		if (s->ceily < yc) {
			glBegin(GL_QUADS);
				glVertex3f(p1.x, yc, p1.z);
				glVertex3f(p2.x, yc, p2.z);
				glVertex3f(p2.x, s->ceily, p2.z);
				glVertex3f(p1.x, s->ceily, p1.z);
			glEnd();
		}
	}
}

Portal::Portal(float x1,float z1,float x2,float z2,int _targetid,bool _textured,int _texid,int ltid,int utid) : Line2D(x1,z1,x2,z2)
{
	next = NULL;	
	s = NULL;
	texid = _texid;
	ltexid = ltid;
	utexid = utid;
	targetid = _targetid;
	textured = _textured;
	visible = false;
}

////////// class Sector //////////

void Sector::LinkPortals(void)
{
	Portal *current = portal;
	while (current != NULL) {
		current->LinkSector();
		current = current->next;
	}
}

void Sector::Render(void)
{
	if (rendered) return;

	Portal *current1 = portal;
	while (current1 != NULL) {
		current1->RenderSector();
		current1 = current1->next;
	}

	if (this == P->InSector) glDisable(GL_DEPTH_TEST);

	if (Lights == NULL) {
		glColor4f(c.r,c.g,c.b,0.15f);

		current1 = portal;
		while (current1 != NULL) {
			if (current1->visible) current1->Render(floory,ceily,true);
			current1 = current1->next;
		}

		Wall *current2 = wall;
		while (current2 != NULL) {
			current2->Render(floory,ceily,true);
			current2 = current2->next;
		}

		Point2D *current3 = point;
		glBindTexture(GL_TEXTURE_2D, GameLevel->TextureList[ceiltexid].texture[0]);
	
		if (ceiltexid != -1) {
			glCullFace(GL_FRONT);
			glBegin(GL_TRIANGLE_FAN);
				while (current3 != NULL) {
					glTexCoord2f(current3->x / GameLevel->TextureList[ceiltexid].w_tex_dist,current3->z / GameLevel->TextureList[ceiltexid].h_tex_dist);
					glVertex3f(current3->x,ceily,current3->z);
					current3 = current3->next;
				}
			glEnd();
			glCullFace(GL_BACK);
		}

		glBindTexture(GL_TEXTURE_2D, GameLevel->TextureList[floortexid].texture[0]);
		current3 = point;
		glBegin(GL_TRIANGLE_FAN);
			while (current3 != NULL) {
				glTexCoord2f(current3->x / GameLevel->TextureList[floortexid].w_tex_dist,current3->z / GameLevel->TextureList[floortexid].h_tex_dist);
				glVertex3f(current3->x,floory,current3->z);
				current3 = current3->next;
			}
		glEnd();
	} else {
		glDepthMask(GL_FALSE);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_CULL_FACE);
		glColor3f(c.r,c.g,c.b);

		current1 = portal;
		while (current1 != NULL) {
			if (current1->visible) current1->Render(floory,ceily,false);
			current1 = current1->next;
		}

		Wall *current2 = wall;
		while (current2 != NULL) {
			if (current2->FindDist(P->p) > 0) {
				current2->visible = true;
				current2->Render(floory,ceily,false);
			}
			current2 = current2->next;
		}

		Point2D *current3;
		if (ceiltexid != -1) {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			current3 = point;
			glBegin(GL_TRIANGLE_FAN);
				while (current3 != NULL) {
					glVertex3f(current3->x,ceily,current3->z);
					current3 = current3->next;
				}
			glEnd();
			glCullFace(GL_BACK);
		}

		current3 = point;
		glBegin(GL_TRIANGLE_FAN);
			while (current3 != NULL) {
				glVertex3f(current3->x,floory,current3->z);
				current3 = current3->next;
			}
		glEnd();

///// Render lighting
		glDisable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_COLOR,GL_ONE);
		glEnable(GL_BLEND);

		glBindTexture(GL_TEXTURE_2D, GameLevel->TextureList[10].texture[0]);
		LightNode *currentln = Lights;
		while (currentln != NULL) {
			currentln->light->Render(this);
			currentln = currentln->next;
		}

///// Draw textures
		glDepthMask(GL_TRUE);

		glColor4f(1,1,1,0.15f);

		current1 = portal;
		while (current1 != NULL) {
			if (current1->visible) {
				current1->Render(floory,ceily,true);
				current1->visible = false;
			}
			current1 = current1->next;
		}

		glBlendFunc(GL_DST_COLOR,GL_ZERO);

		current2 = wall;
		while (current2 != NULL) {
			if (current2->visible) {
				current2->Render(floory,ceily,true);
				current2->visible = false;
			}
			current2 = current2->next;
		}

		if (ceiltexid != -1) {
			glBindTexture(GL_TEXTURE_2D, GameLevel->TextureList[ceiltexid].texture[0]);
			current3 = point;	
			glCullFace(GL_FRONT);
			glBegin(GL_TRIANGLE_FAN);
				while (current3 != NULL) {
					glTexCoord2f(current3->x / GameLevel->TextureList[ceiltexid].w_tex_dist,current3->z / GameLevel->TextureList[ceiltexid].h_tex_dist);
					glVertex3f(current3->x,ceily,current3->z);
					current3 = current3->next;
				}
			glEnd();
			glCullFace(GL_BACK);
		}

		glBindTexture(GL_TEXTURE_2D, GameLevel->TextureList[floortexid].texture[0]);
		current3 = point;
		glBegin(GL_TRIANGLE_FAN);
			while (current3 != NULL) {
				glTexCoord2f(current3->x / GameLevel->TextureList[floortexid].w_tex_dist,current3->z / GameLevel->TextureList[floortexid].h_tex_dist);
				glVertex3f(current3->x,floory,current3->z);
				current3 = current3->next;
			}
		glEnd();
		glDisable(GL_BLEND);
	}

	Particle_System *current = PSystem;
	while (current != NULL) {
		current->Update();
		current = current->next;
	}

	rendered = true;

	if (this == P->InSector) glEnable(GL_DEPTH_TEST);
}

Sector::Sector(Wall *_wall,Portal *_portal,Point2D *_point,Color *_c,float yf,float yc,int _id,int ftid,int ctid)
{
	wall = _wall;
	portal = _portal;
	point = _point;
	c = *_c;
	floory = yf;
	ceily = yc;
	id = _id;
	floortexid = ftid;
	ceiltexid = ctid;
	rendered = false;
	Lights = NULL;
	PSystem = NULL;
	next = NULL;
}

Sector::~Sector()
{
	Wall *current1 = wall, *temp1;
	while (current1 != NULL) {
		temp1 = current1;
		current1 = current1->next;
		delete temp1;
	}

	Portal *current2 = portal, *temp2;
	while (current2 != NULL) {
		temp2 = current2;
		current2 = current2->next;
		delete temp2;
	}

	Particle_System *current3 = PSystem, *temp3;
	while (current3 != NULL) {
		temp3 = current3;
		current3 = current3->next;
		delete temp3;
	}

	LightNode *current4 = Lights, *temp4;
	while (current4 != NULL) {
		temp4 = current4;
		current4 = current4->next;
		delete temp4;
	}

}

////////// class Skybox //////////

void Skybox::Render(void)
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);

	glColor3f(1,1,1);
	glBindTexture(GL_TEXTURE_2D,GameLevel->TextureList[texid[0]].texture[0]);
	glBegin(GL_QUADS);
		glTexCoord2i(1,1);
		glVertex3f(-100,100,-100);

		glTexCoord2i(1,0);
		glVertex3f(-100,-100,-100);

		glTexCoord2i(0,0);
		glVertex3f(100,-100,-100);

		glTexCoord2i(0,1);
		glVertex3f(100,100,-100);
	glEnd();

	glBindTexture(GL_TEXTURE_2D,GameLevel->TextureList[texid[1]].texture[0]);
	glBegin(GL_QUADS);
		glTexCoord2i(0,1);
		glVertex3f(-100,100,100);

		glTexCoord2i(0,0);
		glVertex3f(-100,-100,100);

		glTexCoord2i(1,0);
		glVertex3f(100,-100,100);

		glTexCoord2i(1,1);
		glVertex3f(100,100,100);
	glEnd();

	glBindTexture(GL_TEXTURE_2D,GameLevel->TextureList[texid[2]].texture[0]);
	glBegin(GL_QUADS);
		glTexCoord2i(1,1);
		glVertex3i(-100,100,100);

		glTexCoord2i(1,0);
		glVertex3i(-100,-100,100);

		glTexCoord2i(0,0);
		glVertex3i(-100,-100,-100);

		glTexCoord2i(0,1);
		glVertex3i(-100,100,-100);
	glEnd();

	glBindTexture(GL_TEXTURE_2D,GameLevel->TextureList[texid[3]].texture[0]);
	glBegin(GL_QUADS);
		glTexCoord2i(0,1);
		glVertex3i(100,100,100);

		glTexCoord2i(0,0);
		glVertex3i(100,-100,100);

		glTexCoord2i(1,0);
		glVertex3i(100,-100,-100);

		glTexCoord2i(1,1);
		glVertex3i(100,100,-100);
	glEnd();

	glBindTexture(GL_TEXTURE_2D,GameLevel->TextureList[texid[4]].texture[0]);
	glBegin(GL_QUADS);
		glTexCoord2i(1,1);
		glVertex3i(-100,100,-100);

		glTexCoord2i(0,1);
		glVertex3i(-100,100,100);

		glTexCoord2i(0,0);
		glVertex3i(100,100,100);

		glTexCoord2i(1,0);
		glVertex3i(100,100,-100);
	glEnd();

	glBindTexture(GL_TEXTURE_2D,GameLevel->TextureList[texid[5]].texture[0]);
	glBegin(GL_QUADS);
		glTexCoord2i(1,0);
		glVertex3i(-100,-100,-100);

		glTexCoord2i(0,0);

		glVertex3i(-100,-100,100);


		glTexCoord2i(0,1);
		glVertex3i(100,-100,100);

		glTexCoord2i(1,1);

		glVertex3i(100,-100,-100);
	glEnd();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
}

Skybox::Skybox(int id)
{
	for (int c = 0; c < 6; c++) texid[c] = id+c;
}

////////// class Level //////////

void Level::BuildLevel(void)
{
	Sector *current = SectorList;
	while (current != NULL) {
		current->LinkPortals();
		current = current->next;
	}

	Light *current2 = LightList;
	while (current2 != NULL) {
		current2->LinkSector();
		current2 = current2->next;
	}
}

Level::Level(const char *Filename)
{
	Name = NULL;
	SectorList = NULL;
	LightList = NULL;
	SkyBox = new Skybox(NUMTEXTURES-6);
	TextureList = new Texture[NUMTEXTURES];
}

Level::~Level()
{
	Sector *current = SectorList, *temp;
	while (current != NULL) {
		temp = current;
		current = current->next;
		delete temp;
	}

	Light *current2 = LightList, *temp2;
	while (current2 != NULL) {
		temp2 = current2;
		current2 = current2->next;
		delete temp2;
	}

	delete SkyBox;
	delete [] Name;
	delete [] TextureList;
}
