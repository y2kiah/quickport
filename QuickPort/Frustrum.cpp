/* ----==== FRUSTRUM.CPP ====---- */

#include <windows.h>
#include <math.h>
#include "frustrum.h"
//#include "object.h"
#include "main.h"

/*-----------------
---- VARIABLES ----
-----------------*/

extern float	*Sin;
extern float	*Cos;

/*-----------------
---- FUNCTIONS ----
-----------------*/

////////// class Frustrum //////////
/*Frustrum::Frustrum(float Angle, float SeeDist)
{
	// Upper Left
	op[0].x = -tanf(Angle * DEGTORAD * 0.5f) * SeeDist;
	op[0].y = tanf(Angle * DEGTORAD * 0.5f) * SeeDist * 0.75f;
	op[0].z = -SeeDist;
	// Lower Left
	op[1].x = op[0].x;
	op[1].y = -op[0].y;
	op[1].z = -SeeDist;
	// Lower Right
	op[2].x = -op[0].x;
	op[2].y = -op[0].y;
	op[2].z = -SeeDist;
	// Upper Right
	op[3].x = -op[0].x;
	op[3].y = op[0].y;
	op[3].z = -SeeDist;

	for (int p = 0; p < 4; p++) wp[p] = op[p];

	Point3D origin(0,0,0);
	ln[0].Reset(&op[0],&origin);
	ln[1].Reset(&op[1],&origin);
	ln[2].Reset(&origin,&op[2]);
	ln[3].Reset(&origin,&op[3]);

	next = NULL;
}

void Frustrum::GetWorldPoints(Point2D *p,int ZAngle,int XAngle,int YAngle)
{
	Point3D tp, p3d;
	p3d.x = p->x;
	p3d.y = 0;
	p3d.z = p->z;

	for (int c = 0; c < 4; c++) wp[c] = op[c];

	for (c = 0; c < 4; p++) {
		tp.x = wp[c].x * Cos[ZAngle] + wp[c].y * Sin[ZAngle];
		tp.y = wp[c].y * Cos[ZAngle] - wp[c].x * Sin[ZAngle];
		wp[c].x = tp.x;
		wp[c].y = tp.y;
		
		tp.y = wp[c].y * Cos[XAngle] + wp[c].z * Sin[XAngle];
		tp.z = wp[c].z * Cos[XAngle] - wp[c].y * Sin[XAngle];
		wp[c].y = tp.y;
		wp[c].z = tp.z;

		tp.z = wp[c].z * Cos[YAngle] + wp[c].x * Sin[YAngle];
		tp.x = wp[c].x * Cos[YAngle] - wp[c].z * Sin[YAngle];
		wp[c].x = tp.x;
		wp[c].z = tp.z;
		
		wp[c] += p3d;
	}

	ln[0].Reset(&wp[0],&p3d);
	ln[1].Reset(&wp[1],&p3d);
	ln[2].Reset(&p3d,&wp[2]);
	ln[3].Reset(&p3d,&wp[3]);
}*/

////////// class Frustrum //////////

void Frustrum::GetWorldPoints(Point2D *p, int Angle)
{
	Point2D tp;

	for (int c = 0; c < 2; c++) wp[c] = op[c];

	for (c = 0; c < 2; c++) {
		tp.z = wp[c].z * Cos[Angle] + wp[c].x * Sin[Angle];
		tp.x = wp[c].x * Cos[Angle] - wp[c].z * Sin[Angle];

		wp[c].x = tp.x;
		wp[c].z = tp.z;
		
		wp[c] += *p;
	}

	ln[0].Reset(&wp[0],p);
	ln[1].Reset(p,&wp[1]);
}

Frustrum::Frustrum(float Angle)
{
	// Left
	op[0].x = -tanf(Angle * DEGTORAD * 0.5f) * 1000;
	op[0].z = -1000;
	// Right
	op[1].x = -op[0].x;
	op[1].z = -1000;

	wp[0] = op[0];
	wp[1] = op[1];

	Point2D origin(0,0);
	ln[0].Reset(&op[0],&origin);
	ln[1].Reset(&origin,&op[1]);

	next = NULL;
}

Frustrum::Frustrum(Point2D *origin, Point2D *left, Point2D *right)
{
	ln[0].Reset(left,origin);
	ln[1].Reset(origin,right);
	next = NULL;
}

////////// class FrustrumList //////////

void FrustrumList::Push(Point2D *origin, Point2D *left, Point2D *right)
{
	Frustrum *NewFrust = new Frustrum(origin,left,right);
	NewFrust->ln[2].Reset(left,right);
	NewFrust->next = list;
	list = NewFrust;
}

void FrustrumList::PushCheck(Point2D *origin, Point2D *left, Point2D *right)
{
	Frustrum *NewFrust = new Frustrum(origin,left,right);
	
	if (list->ln[0].FindDist(*left) <= 0) NewFrust->ln[0] = list->ln[0];
	if (list->ln[1].FindDist(*right) <= 0) NewFrust->ln[1] = list->ln[1];

	NewFrust->ln[2].Reset(left,right);
	NewFrust->next = list;
	list = NewFrust;
}

void FrustrumList::Pop(void)
{
	if (list != NULL) {
		Frustrum *temp = list;
		list = list->next;
		delete temp;
	}
}

FrustrumList::~FrustrumList()
{
	Frustrum *current = list, *temp;
	while (current != NULL) {
		temp = current;
		current = current->next;
		delete temp;
	}
}