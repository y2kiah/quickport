/* ----==== LEVEL.H ====---- */

#ifndef LEVEL_H
#define LEVEL_H

struct List {
	List		*next;
	char		*filename;
	int			index;
};

struct Point2D {
	float x, z;
};

struct Portal {
	Point2D	p1, p2;


};

struct Sector {
	Point2D	*fcpoints;
	
	float	yf, yc;
	float	r, g, b;

	int		texid;
};

struct Level {
	List	*texturelist;
	List	*objectlist;
	List	*soundlist;
	List	*particlelist;

};


#endif