/* ----==== 3D.H ====---- */

#ifndef _3D_H
#define _3D_H

#include "world.h"

#define INDEX(a,b,s)	((b) * (s) + (a))

/*------------------
---- STRUCTURES ----
------------------*/

struct FileString {
	char Name[20]; 
};

class MeshFace {
	public:
		MeshFace *next;

		Point3D	v1, v2, v3;
//		Vector	n1, n2, n3;
		Color	c1, c2, c3;
		float	uv[3][2];

		int texid;
		MeshFace(Point3D *_vt1,Point3D *_vt2,Point3D *_vt3,float _u1,float _v1,float _u2,float _v2,float _u3,float _v3,Color *_c1,Color *_c2,Color *_c3,int _texid);
};

class Mesh3d {
	public:
		MeshFace *FaceList;
		int	NumFaces;

		void Render(void) const;
		Mesh3d(char *Filename);
		~Mesh3d();
};

class Face {
	public:
		Face	*next;
		int		v1, v2, v3;		// index numbers for the vertices and normals
		int		texid;
		float	uv[3][2];
		Color	c;

		Face(int i1,int i2,int i3,int texid,float u1,float v1,float u2,float v2,float u3,float v3,float r,float g,float b) {} // Do this
};

class Mesh {
	public:
		float	*vertices[3];
		float	*normals[3];
		Face	*faces;

		void Render(void);
		Mesh(unsigned char *Filename);
		~Mesh() {}
};

#endif