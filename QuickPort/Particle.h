/*
New particles are generated and placed into the current particle system
Each new particle is assigned it's own unique attributes
Any particles that have outlasted their life span are declared 'dead'
The current particles are moved according to their scripts (or in our case, according to their pre-assigned velocity vectors)
The current particles are rendered
*/

#ifndef PARTICLE_H
#define PARTICLE_H

#include "world.h"

class Particle {
	public:
		Particle	*next;

		Point3D		pos;			// Position vector, changed by velocity vector
		Point3D		vel;			// Velocity vector, changed by acceleration vector
		Point3D		accel;			// Acceleration vector, constant, set at initialization

		Color		c, deltac;		// Current color, color interval

		float		life;			// Total lifetime of the particle
		float		lifecounter;	// Lifetime left
		float		radius;			// diameter of the particle in world space

		bool		alive;			// Is the particle alive or dead?

		void Update(void);
		void Render(void);
		void Reset(Point3D *_pos,Point3D *_vel,Point3D *_accel,Color *_ci,Color *_cf,float _life,float rad);
		Particle(Point3D *_pos,Point3D *_vel,Point3D *_accel,Color *_ci,Color *_cf,float _life,float rad);
};

class Particle_Effect {
	public:
		Point3D		orand;				// A value for x y z of how far from origin particles will start randomly
		Point3D		direction;			// Direction particles will shoot, used to initialize particle vel vector, speed of particles built into this
		Point3D		drand;				// Randomness factor for direction vector
		Point3D		acceleration;		// Acceleration on particles
		
		Color		ci, cf;				// Initial and final color

		float		radius;
		float		lifespan;			// Lifespan of the particles created in seconds
		float		liferand;			// Random factor for lifespan + or -

		int			maxparticles;		// Total number of particles allowed in the system
		int			blendmode;			// Type of blending to use

		bool		lighting;			// Is it effected by lighting or not

		void Load(char *Filename);
		Particle_Effect();
		~Particle_Effect();
};

/*class Particle_System {
	public:
		Particle_System	*next;
		Particle_Effect	*peffect;
		Particle		*plist;		// List of particles
		Point3D			origin;		// Origin where the particles will start
		int				pcount;		// Number of particles currently in the system

		void Update(void);
		Particle_System(Point3D *_origin,Point3D *_orand,Point3D *_direction,Point3D *_drand,Point3D *_accel,Color *_ci,Color *_cf,float _radius,float _lifespan,float _liferand,int _maxparticles,bool _lighting);
		Particle_System(const char *Filename,Point3D *_origin);
		~Particle_System();
};*/

class Particle_System {
	public:
		Particle_System	*next;
		Particle		*plist;		// List of particles

		Point3D	origin;				// Origin where the particles will start
		Point3D	orand;				// A value for x y z of how far from origin particles will start randomly
		Point3D	direction;			// Direction particles will shoot, used to initialize particle vel vector, speed of particles built into this
		Point3D	drand;				// Randomness factor for direction vector
		Point3D	acceleration;		// Acceleration on particles

		Color	ci, cf;				// Initial and final color

		float	radius;
		float	lifespan;			// Lifespan of the particles created in seconds
		float	liferand;			// Random factor for lifespan + or -

		int		maxparticles;		// Total number of particles allowed in the system
		int		pcount;				// Number of particles currently in the system

		bool	lighting;			// Is it effected by lighting or not

		void Update(void);
		Particle_System(Point3D *_origin,Point3D *_orand,Point3D *_direction,Point3D *_drand,Point3D *_accel,Color *_ci,Color *_cf,float _radius,float _lifespan,float _liferand,int _maxparticles,bool _lighting);
		Particle_System(const char *Filename,Point3D *_origin);
		~Particle_System();
};


#endif