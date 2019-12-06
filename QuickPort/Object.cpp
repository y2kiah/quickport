/* ----==== OBJECT.CPP ====---- */

#include <windows.h>
#include <string.h>
#include <math.h>
#include "object.h"
#include "main.h"
#include "frustrum.h"

#include "fmod.h"

/*-----------------
---- VARIABLES ----
-----------------*/

extern GameOptions	Options;
extern Level		*GameLevel;
extern float		*Sin;
extern float		*Cos;
extern float		*Tan;
extern float		TimeFix;
extern bool			ButtonDown[256];
POINT				mPos;

extern FSOUND_SAMPLE *Sound[2];

/*-----------------
---- FUNCTIONS ----
-----------------*/

////////// class Player //////////

void Player::Input(void)
{
	// Mouse-Look
	GetCursorPos(&mPos);
	SetCursorPos(Options.RESX / 2,Options.RESY / 2);

	int mDiffx = Options.RESX / 2 - mPos.x;
	int mDiffy = Options.RESY / 2 - mPos.y;

	YAngle -= mDiffx;
	XAngle -= mDiffy;

	if (YAngle < 0) YAngle += ANGLE360;
	else if (YAngle >= ANGLE360) YAngle -= ANGLE360;
	if (XAngle < -ANGLE60) XAngle = -ANGLE60;
	else if (XAngle > ANGLE60) XAngle = ANGLE60;

	// Movement
	if (!Jump) {
		float TopSpeed = MaxSpeed;
		if (KeyDown(VK_LSHIFT)) TopSpeed *= 0.4f;
		if (KeyDown('W') && !KeyDown('S')) {
			RunSpeed += 100 * TimeFix;
			if (RunSpeed > TopSpeed) RunSpeed = TopSpeed;
		} else if (KeyDown('S') && !KeyDown('W')) {
			RunSpeed -= 100 * TimeFix;
			if (RunSpeed < -TopSpeed) RunSpeed = -TopSpeed;
		}
		else if (fabs(RunSpeed) > 0.5f) RunSpeed *= 0.1f * TimeFix;
		else RunSpeed = 0;

		if (KeyDown('D') && !KeyDown('A')) {
			StrafeSpeed += 100 * TimeFix;
			if (StrafeSpeed > TopSpeed) StrafeSpeed = TopSpeed;
		} else if (KeyDown('A') && !KeyDown('D')) {
			StrafeSpeed -= 100 * TimeFix;
			if (StrafeSpeed < -TopSpeed) StrafeSpeed = -TopSpeed;
		}
		else if (fabs(StrafeSpeed) > 0.5f) StrafeSpeed *= 0.1f * TimeFix;
		else StrafeSpeed = 0;

		if (KeyDown(VK_SPACE) && !Jump && y == targety) {
			VertSpeed = 14;
			Jump = true;
			FSOUND_PlaySound(FSOUND_FREE,Sound[0]);
		}

		if (KeyDown('L')) {
			if (!ButtonDown['L']) {
				ButtonDown['L'] = true;
				light->on = !light->on;
			}
		} else ButtonDown['L'] = false;
	}

/*	if (light->radius <= 100 && Key['Z']) {
		float r = light->radius += 1;
		delete light;
		light = new Light(viewp.x,viewy,viewp.z,r,1,1,1,0);
		light->InSector = InSector;
	}
	if (light->radius != 1 && Key['X']) {
		float r = light->radius -= 1;
		if (r < 1) r = 1;
		delete light;
		light = new Light(viewp.x,viewy,viewp.z,r,1,1,1,0);
		light->InSector = InSector;
	}*/

/*	if (Key['R']) {
		ZAngle = 0;
	}
	if (Key['Q']) {
		ZAngle -= 2;
		if (ZAngle < -ANGLE30) ZAngle = -ANGLE30;
	}
	if (Key['E']) {
		ZAngle += 2;
		if (ZAngle > ANGLE30) ZAngle = ANGLE30;
	}
	if (Key['Z']) y += 0.5f;
	if (Key['X']) y -= 0.5f;*/
}

void Player::HandleCollide(Wall *Current)
{
	float Dist = Current->FindDist(New);
	if (Dist < WALL_LIMIT) {
//		Line2D moveline(p.x,p.z,Test->x,Test->z);
//		Point2D pint = Current->FindInt(moveline);
//		pint.x = p.x - Current->n.x * -Dist;
//		pint.z = p.z - Current->n.z * -Dist;

		Point2D A = Current->p1 - Current->p2;
		Point2D B = Current->p1 - p;
		Point2D C = Current->p2 - p;

		float p1 = A * B;
		float p2 = A * C;

		if (p1 > 0 && p2 < 0) {
			float dx = Current->n.x * (-Dist+WALL_LIMIT);
			float dz = Current->n.z * (-Dist+WALL_LIMIT);

			New.x += dx;
			New.z += dz;
		}
	}
}

bool Player::HandleCollide(Portal *Current)
{
	if ((Current->s->floory - y > 3) || (Current->s->ceily - viewy < 0.5f) || (Current->s->ceily - Current->s->floory <= 6.5)) {
		float Dist = Current->FindDist(New);
		if (Dist < WALL_LIMIT) {
//			Point2D pint;
//			pint.x = p.x - Current->n.x * -Dist;
//			pint.z = p.z - Current->n.z * -Dist;

			Point2D A = Current->p1 - Current->p2;
			Point2D B = Current->p1 - p;
			Point2D C = Current->p2 - p;

			float p1 = A * B;
			float p2 = A * C;

			if (p1 > 0 && p2 < 0) {
				float dx = Current->n.x * (-Dist+WALL_LIMIT);
				float dz = Current->n.z * (-Dist+WALL_LIMIT);

				New.x += dx;
				New.z += dz;
			}
		}
	} else {
		float Dist = Current->FindDist(New);
		if (Dist < 0) {
//			Point2D pint;
//			pint.x = p.x - Current->n.x * -Dist;
//			pint.z = p.z - Current->n.z * -Dist;

			Point2D A = Current->p1 - Current->p2;
			Point2D B = Current->p1 - p;
			Point2D C = Current->p2 - p;

			float p1 = A * B;
			float p2 = A * C;

			if (p1 > 0 && p2 < 0) return true;
		}
	}
	return false;
}

Sector *Player::CheckCollisions(Sector *sect)
{
	Wall *current1 = sect->wall;
	while (current1 != NULL) {
		HandleCollide(current1);
		current1 = current1->next;
	}

	Portal *current2 = sect->portal;
	while (current2 != NULL) {
		if (HandleCollide(current2)) {
			sect = CheckCollisions(current2->s);
			break;
		}
		current2 = current2->next;
	}

	return sect;
}

void Player::Update(void)
{
	New = p;

	targety = InSector->floory;
	if (y - targety > 3) Jump = true;
	if (Jump) {
		y += VertSpeed * TimeFix;
		VertSpeed -= 32.2f * TimeFix;
		if (y <= targety) {
			Jump = false;
			VertSpeed = 0;
			FSOUND_PlaySound(FSOUND_FREE,Sound[1]);
		}
	} else if (targety > y) {
		y += 10 * TimeFix;
		if (y >= targety) y = targety;
	} else if (y > targety) {
		y -= 10 * TimeFix;
		if (y <= targety) y = targety;
	}

	viewy = y + 6;
	if (viewy > InSector->ceily - 0.5f) {
		viewy = InSector->ceily - 0.5f;
		y = viewy - 6;
		VertSpeed = 0;
	}

	if (RunSpeed != 0 || StrafeSpeed != 0) {
		// Move Player
		if (!Jump) {
			VelSave.x = (Sin[YAngle] * RunSpeed + Cos[YAngle] * StrafeSpeed);
			VelSave.z = (Cos[YAngle] * RunSpeed - Sin[YAngle] * StrafeSpeed);
		}
		New.x += VelSave.x * TimeFix;
		New.z -= VelSave.z * TimeFix;

		// Check Collisions
		InSector = CheckCollisions(InSector);
	}
	p = viewp = New;

	if (light != NULL) {
		light->p = p;
		light->y = viewy;
		light->InSector = InSector;
	}

//	flist->list->GetWorldPoints(&p,ZAngle,XAngle,YAngle);
	flist->list->GetWorldPoints(&p,YAngle);
}

Player::Player(const char *_n,int _s,float _x,float _y,float _z,int _xa,int _ya,int _za,float maxspeed)
{
	p.x = _x;
	y = _y;
	p.z = _z;
	viewp = p;
	viewy = y + 6;
	targety = y;
	XAngle = _xa;
	YAngle = _ya;
	ZAngle = _za;
	RunSpeed = StrafeSpeed = VertSpeed = 0;
	MaxSpeed = maxspeed;
	Jump = false;
	Name = new char[strlen(_n)];
	strcpy(Name,_n);

	InSector = NULL;
	Sector *Current = GameLevel->SectorList;
	while (Current != NULL) {
		if (Current->id == _s) {
			InSector = Current;
			break;
		}
		Current = Current->next;
	}

	light = new Light(viewp.x,viewy,viewp.z,25,1,0.9f,0.8f,_s);
	light->InSector = InSector;

	flist = new FrustrumList;
	flist->list = new Frustrum(80);//,1000);
}

Player::~Player()
{
	if (light != NULL) delete light;
	delete flist;
	delete [] Name;
}