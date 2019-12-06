/* ----==== QUADTREE.CPP ====---- */

#include <windows.h>
#include <gl\gl.h>
#include "quadtree.h"

/*-----------------
---- VARIABLES ----
-----------------*/

Quadtree	*terrain;

/*-----------------
---- FUNCTIONS ----
-----------------*/

////////// class Branch //////////

void Branch::Traverse(void)
{
	// check visible
	subnode[0]->Traverse();
	subnode[1]->Traverse();
	subnode[2]->Traverse();
	subnode[3]->Traverse();

	glDisable(GL_TEXTURE_2D);
	glColor3f(0,0,1);
	glBegin(GL_LINE_STRIP);
		glVertex3i(xpos[0],0,zpos[0]);
		glVertex3i(xpos[1],0,zpos[1]);
		glVertex3i(xpos[3],0,zpos[3]);
		glVertex3i(xpos[2],0,zpos[2]);
		glVertex3i(xpos[0],0,zpos[0]);
	glEnd();
	glEnable(GL_TEXTURE_2D);
}

Branch::Branch(int cx, int cz, int size)
{
	xpos[0] = (cx - size / 2) * terrain->spacing;
	zpos[0] = (cz - size / 2) * terrain->spacing;

	xpos[1] = (cx + size / 2) * terrain->spacing;
	zpos[1] = (cz - size / 2) * terrain->spacing;

	xpos[2] = (cx - size / 2) * terrain->spacing;
	zpos[2] = (cz + size / 2) * terrain->spacing;

	xpos[3] = (cx + size / 2) * terrain->spacing;
	zpos[3] = (cz + size / 2) * terrain->spacing;

	ypos[0] = 0; // min
	ypos[1] = 0; // max

	int newsize = size / 2;
	int newdeltacx = size / 4;
	int newdeltacz = size / 4;

	if (newsize <= LEAFSIZE) {
		subnode[0] = new Leaf(cx - newdeltacx,cz - newdeltacz,newsize);
		subnode[1] = new Leaf(cx + newdeltacx,cz - newdeltacz,newsize);
		subnode[2] = new Leaf(cx - newdeltacx,cz + newdeltacz,newsize);
		subnode[3] = new Leaf(cx + newdeltacx,cz + newdeltacz,newsize);
	} else {
		subnode[0] = new Branch(cx - newdeltacx,cz - newdeltacz,newsize);
		subnode[1] = new Branch(cx + newdeltacx,cz - newdeltacz,newsize);
		subnode[2] = new Branch(cx - newdeltacx,cz + newdeltacz,newsize);
		subnode[3] = new Branch(cx + newdeltacx,cz + newdeltacz,newsize);
	}
}

Branch::~Branch()
{
	delete subnode[0];
	delete subnode[1];
	delete subnode[2];
	delete subnode[3];
}

////////// class Leaf //////////

void Leaf::Traverse(void)
{
	// check visible

	int posx, posz1 = zpos[0], posz2 = zpos[0] + terrain->spacing;
	int *linestart1 = hmapref;
	int *linestart2 = hmapref + terrain->size;

	for (int ztrans = 0; ztrans < LEAFSIZE; ztrans++) {
		posx = xpos[0];

		glDisable(GL_TEXTURE_2D);
		glBegin(GL_TRIANGLE_STRIP);

			for (int xtrans = 0; xtrans < LEAFSIZE+1; xtrans++) {
				glColor3f(0.2f, 0.7f, 0.2f);
//				glColor3f(1,1,1);
				glVertex3i(posx,*(linestart1+xtrans),posz1);
				glVertex3i(posx,*(linestart2+xtrans),posz2);

				posx += terrain->spacing;
			}

		glEnd();
		glColor3f(1,0,0);
		glBegin(GL_LINE_STRIP);
			glVertex3i(xpos[0],0,zpos[0]);
			glVertex3i(xpos[1],0,zpos[1]);
			glVertex3i(xpos[3],0,zpos[3]);
			glVertex3i(xpos[2],0,zpos[2]);
			glVertex3i(xpos[0],0,zpos[0]);
		glEnd();
		glEnable(GL_TEXTURE_2D);

		posz1 += terrain->spacing;
		posz2 += terrain->spacing;

		linestart1 = linestart2;
		linestart2 += terrain->size;
	}
}

Leaf::Leaf(int cx, int cz, int size)
{
	xpos[0] = (cx - size / 2) * terrain->spacing;
	zpos[0] = (cz - size / 2) * terrain->spacing;

	xpos[1] = (cx + size / 2) * terrain->spacing;
	zpos[1] = (cz - size / 2) * terrain->spacing;

	xpos[2] = (cx - size / 2) * terrain->spacing;
	zpos[2] = (cz + size / 2) * terrain->spacing;

	xpos[3] = (cx + size / 2) * terrain->spacing;
	zpos[3] = (cz + size / 2) * terrain->spacing;

	ypos[0] = 0; // min
	ypos[1] = 0; // max

	int xref = cx - size / 2;
	int zref = cz - size / 2;
	int refnum = zref * terrain->size + xref;

	hmapref = &terrain->heightmap[refnum];
	tmapref = &terrain->ttypemap[refnum];
}

Leaf::~Leaf()
{
	Strip *current = strips, *temp;
	while (current != NULL) {
		temp = current;
		current = current->next;
		delete temp;
	}
}

////////// class Quadtree //////////

void Quadtree::Buildtree(void)
{
	tree = new Branch((size-1) / 2,(size-1) / 2,size-1);
}

Quadtree::Quadtree()
{
	size = 33;//1025;
	spacing = 5;

	heightmap = new int[size*size];
	ttypemap = new int[size*size];

	for (int c = 0; c < size*size; c++) heightmap[c] = ttypemap[c] = 0;

	tree = NULL;
}

Quadtree::~Quadtree()
{
	delete tree;

	delete [] heightmap;
	delete [] ttypemap;
}