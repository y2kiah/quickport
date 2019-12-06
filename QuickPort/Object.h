/* ----==== OBJECT.H ====---- */

#ifndef OBJECT_H
#define OBJECT_H

#include "world.h"
#include "light.h"

/*---------------
---- DEFINES ----
---------------*/

///// Object sound events /////

enum {
	OBJECT_AMBIENT_LOOP = 0,
	OBJECT_WALKING_SHOES,
	OBJECT_RUNNING_SHOES,
	OBJECT_WALKING_FEET,
	OBJECT_RUNNING_FEET,
	OBJECT_JUMPING,
	OBJECT_LANDING,
	OBJECT_ATTACKING,
	OBJECT_DEFENDING,
	OBJECT_TALKING,
	OBJECT_MUMBLING,
	OBJECT_DAMAGED,
	OBJECT_DIEING,
	OBJECT_ACQUIRE_TARGET,
	OBJECT_AFRAID,
	OBJECT_ENRAGED,
	OBJECT_CELEBRATE,
	OBJECT_BURNING,
	OBJECT_WATER_FLOWING,
	OBJECT_SCRIPT1,
	OBJECT_SCRIPT2,
	OBJECT_SCRIPT3,
	OBJECT_SCRIPT4,
	OBJECT_SCRIPT5
};

///// Item sound events /////

enum {
	ITEM_PICKUP = 0,
	ITEM_USE,
	ITEM_DROP
};

///// Weapon sound events /////

enum {
	WEAPON_AMBIENT_LOOP = 0,
	WEAPON_ATTACK,
	WEAPON_DEFEND,
	WEAPON_BRANDISH,
	WEAPON_STORE,
	WEAPON_DROP,
	WEAPON_RELOAD
};

///// Missile sound events /////

enum {
	MISSILE_AMBIENT_LOOP = 0,
	MISSILE_HIT,
	MISSILE_DUD
};

/*------------------
---- STRUCTURES ----
------------------*/

class Sector;
class FrustrumList;

class Player {
	public:
		Point2D			p, viewp, New, VelSave;
		float			y, viewy, targety;
		int				YAngle, XAngle, ZAngle;
		float			RunSpeed, StrafeSpeed, VertSpeed;
		float			MaxSpeed;
		bool			Jump;
		char			*Name;
		Light			*light;
		Sector			*InSector;
		FrustrumList	*flist;

		void Input(void);
		void Update(void);
		void HandleCollide(Wall *Current);
		bool HandleCollide(Portal *Current);
		Sector *CheckCollisions(Sector *sect);
		Player(const char *_n,int _s,float _x,float _y,float _z,int _xa,int _ya,int _za,float maxspeed);
		~Player();
};

class Object {
	public:
		Object	*next;
		Point2D	p;
		float	y;
		int		XAngle, YAngle, ZAngle;
		int		MeshId;
		Particle_System	*PSystem;

		void Render(void) const;
		Object(float _x,float _y,float _z,float _xa,float _ya,float _za) {}
		~Object() {}
};

class Actor {
	public:
		Actor	*next;
		Point2D	p, New;
		float	y, targety;
		int		XAngle, YAngle, ZAngle;
		int		MeshId;

		void Render(void) const;
		Actor(float _x,float _y,float _z,float _xa,float _ya,float _za) {}
		~Actor() {}
};

//////////////////

/*
class Box3D {
	public:
		Box3D		*next;
		Vector3D	point[8];
		Face3D		face[6];
};

class Sphere3D {
	public:
		Sphere3D	*next;
		Vector3D	*p;	// position
		float		r;	// radius
};

class Face3D {
	public:
		Face3D		*next;
		int			index[3];
		int			texture_id;
		float		uv[3][2];
};

class Mesh3D {
	public:
		Vector3D	**point;						// list of points for each frame
		Vector3D	**normal;						// normals
		Face3D		*face;							// faces
		int			num_animations, num_frames;		// total number of animations in the model
		float		fps;
};

class Object3D {
	public:
		///// Position
		Vector3D	*p;
		Sector		*in_sector;
		int			y_angle, x_angle, z_angle;

		///// Properties
		Object3D			*next;
		Particle_System		*p_system;
		Sound				*sound;
		Box3D				*b_box;
		Sphere				*b_sphere;
		int					mesh_id;
};

class Static : public Object3D {

};

class Dynamic : public Object3D {

};

class Actor : public Object3D {

};

class Player : public Object3D {

};
*/

#endif