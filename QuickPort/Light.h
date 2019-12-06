/* ----==== LIGHT.H ====---- */

#ifndef LIGHT_H
#define LIGHT_H

#include "world.h"

class Light {
	public:
		Light		*next;
		SectorNode	*Sectors;		// Sectors that the light will effect
		Sector		*InSector;		// Sector the light is in
		Point2D		p, oldp;		// Position of the light and stored position for checking if light moved
		float		y, oldy;		// Position in the y axis
		float		radius;			// The max spread of the light, intensity varies directly with cosine of distance
		float		distcosfade;	// ANGLE90 / radius - used to express total distance in terms of an angle from 0 to 90
		int			targetid;
		bool		on;				// on = true	off = false
		Color		c;				// Color (at intensity level 1.0)

		void Render(Sector *sect);
		void Update(Sector *s, int count);
		void LinkSector(void);
		Light(float _x,float _y,float _z,float _radius,float _r,float _g,float _b,int _sectorid);
		~Light();
};

class LightNode {
	public:
		LightNode	*next;
		Light		*light;

		LightNode() { next = NULL; light = NULL; }
		LightNode(Light *l) { light = l; next = NULL; }
};

// Color of light at point of intersection with a plane is Cos[WallDist * distwallfade] * color
// Width / 2 of light spot on plane is Cos[WallDist * distwallfade] * radius
// WallDist is the distance from the light to the closest point on a plane (it's along that planes normal)
// This doesn't guarantee that the light spot will fall on a wall because the plane is infinite but the wall has defined points
// so we must check with the boundaries of the wall to see if the spot falls on it at all, and if so, if it is interrupted by the edge of a wall
// must perform a simple kind of clipping to readjust the light polygon size, and find new u,v values for those points so the correct points on the
// lighting texture will show up
// Option: use progressively larger and larger textures for the light spot if the light box size gets bigger, so accuracy isn't reduced

#endif
