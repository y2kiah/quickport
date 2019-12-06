/* ----==== QUADTREE.H ====---- */

#ifndef QUADTREE_H
#define QUADTREE_H

/*---------------
---- DEFINES ----
---------------*/

#define LEAFSIZE	16

// Terrain Type, in order of layering
enum {
	TTYPE_WATER = 0,
	TTYPE_ROCK,
	TTYPE_MUD,
	TTYPE_DRYMUD,
	TTYPE_DIRT,
	TTYPE_SAND,
	TTYPE_LUSHGRASS,
	TTYPE_DRYGRASS,
	TTYPE_FOLIAGE,
	TTYPE_GRAVEL
};

/*------------------
---- STRUCTURES ----
------------------*/

class Strip {
	public:
		Strip	*next;
		int		*vertexref;
		int		ttype;

		void Render(void) {}
		Strip() { next = NULL; vertexref = NULL; ttype = 0; }
		~Strip() { delete [] vertexref; }
};

class Node {
	public:
		int		xpos[4];
		int		zpos[4];		// x, z coordinates
		int		ypos[2];		// elevation of bottom, top

		virtual void Traverse(void) = 0;
		virtual ~Node() {}
};

class Branch : public Node {
	public:
		Node	*subnode[4];	// nw, ne, sw, se

		void Traverse(void);
		Branch(int cx, int cz, int size);
		~Branch();
};

class Leaf : public Node {
	public:
		Strip	*strips;

		int		*hmapref;
		int		*tmapref;

		void Traverse(void);
		Leaf(int cx, int cz, int size);
		~Leaf();
};

class Quadtree {
	public:
		int		size;			// in pixels (2^n + 1)
		int		spacing;		// in feet
		int		*heightmap;
		int		*ttypemap;		// terrain type

		Node	*tree;

		void Buildtree(void);
		Quadtree();
		~Quadtree();
};

#endif