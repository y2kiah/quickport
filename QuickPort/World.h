/* ----==== WORLD.H ====---- */

#ifndef WORLD_H
#define WORLD_H

#define SKYBOX	-1;

/*------------------
---- STRUCTURES ----
------------------*/

//class Vector;
class Point2D;
class Sector;
class Particle_System;
class Light;
class LightNode;
class Texture;
class Frustrum;

struct Color {
	float r, g, b;							// Used for lighting and blending
};

class Point3D {
	public:
		float x, y, z;

//		const Point3D &operator<<(const Point2D &p);
		const Point3D &operator+=(const Point3D &p);
//		const Point3D &operator+=(const Point2D &p);
		Point3D operator*(const float s) const;
		Point3D() {x = y = z = 0;}
		Point3D(float _x,float _y,float _z) { x = _x; y = _y; z = _z; }
};

class Point2D {
	public:
		Point2D *next;
		float	x, z;

		inline bool operator!=(const Point2D &p) const;
		Point2D operator-(const Point2D &p) const;
		const Point2D &operator-=(const Point2D &p);
		const Point2D &operator+=(const Point2D &p);
		float operator*(const Point2D &p) const;			// Dot product
		Point2D Point2D::operator*(const float s) const;
		const Point2D &operator*=(const float s);
		Point2D operator%(const Point2D &p) const;
		Point2D UnitSlope(const Point2D &p) const;
		Point2D(float _x,float _z) {x = _x; z = _z; next = NULL;}
		Point2D() {x = z = 0; next = NULL;}
};

class Line2D {
	public:
		Point2D p1, p2, n, sl;		// end points, normal and slope, define p1 to p2 clockwise for normal to face inward, pointers will reference point stored in sector
		float	m, b;				// y = mx + b
		float	c;					// ax + by + c = 0
		float	length;

		Point2D	FindInt(const Line2D &l) const;			// Find the intersection point with another line
		float FindDist(const Point2D &p) const;			// Find shortest distance from a point to the line
		void Reset(float x1,float z1,float x2,float z2);
		void Reset(const Point2D *_p1,const Point2D *_p2);
		void Reset(const Point3D *_p1,const Point3D *_p2);
		Line2D() {}
		Line2D(float x1,float z1,float x2,float z2);
		Line2D(const Point2D *_p1,const Point2D *_p2);
};

/*struct Line2DNode {
	Line2DNode	*next;
	Line2D		*lptr;
	float		dist;
};

class Line2DList {
	public:
		Line2DNode *lines;
		
		void Push(Line2D *_l) {} // Do this
		void Pop(void) {}
		Line2DList() { lines = NULL; }
		~Line2DList() {} // Do this
};*/

class Wall : public Line2D {
	public:
		Wall	*next;
		int		texid;
		bool	visible;					// flag set if the wall is visible

		void Render(const float yf,const float yc,bool texture) const;
		Wall(float x1,float z1,float x2,float z2,int _texid);
};

class Portal : public Line2D {
	public:
		Portal	*next;
		Sector	*s;							// Sector that is pointed to
		int		texid, ltexid, utexid;
		int		targetid;					// Used for linking portals to sectors at startup
		bool	textured;
		bool	visible;

		void LinkSector(void);
		void RenderSector(void);
		void Render(const float yf,const float yc,bool texture) const;
		Portal(float x1,float z1,float x2,float z2,int _targetid,bool _textured,int _texid,int ltid,int utid);
};

class Sector {
	public:
		Sector	*next;
		Wall	*wall;						// Walls in the sector
		Portal	*portal;					// Portals in the sector
		Point2D	*point;						// Floor and ceiling points
		float	floory, ceily;				// y values for floor and ceiling
		int		id;							// Used for linking portals to sectors at startup
		int		floortexid;
		int		ceiltexid;
		bool	rendered;					// Was it already rendered this frame?
		Color	c;
		LightNode		*Lights;
		Particle_System *PSystem;			

		void LinkPortals(void);
		void Render(void);
		Sector(Wall *_wall,Portal *_portal,Point2D *_point,Color *_c,float yf,float yc,int _id,int ftid,int ctid);
		~Sector();
};

class SectorNode {
	public:
		SectorNode	*next;
		Sector		*sector;

		SectorNode() { next = NULL; sector = NULL; }
		SectorNode(Sector *s) { sector = s; next = NULL; }
};

/*
class Room : public Sector
class Elevator : public Sector
class Terrain : public Sector
class Skybox : public Sector
*/

class Skybox {
	public:
		int	texid[6];
		
		void Render(void);
		Skybox(int id);
};

class Level {
	public:
		Sector			*SectorList;		// Actual memory storage of sectors in the level
		Skybox			*SkyBox;
		Light			*LightList;			// Lights in the level
		Texture			*TextureList;		// All of the textures used in the level
		char			*Name;				// Name of level

		void BuildLevel(void);
		Level(const char *Filename);
		~Level();
};

#endif