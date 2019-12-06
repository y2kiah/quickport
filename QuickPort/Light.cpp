/* ----==== LIGHT.CPP ====---- */

#include <windows.h>
#include <gl\gl.h>
//#include <gl\glu.h>
//#include <gl\glaux.h>
#include <math.h>
#include "light.h"
#include "engine.h"
#include "main.h"
#include "object.h"

/*-----------------
---- VARIABLES ----
-----------------*/

extern GameOptions Options;
extern Level	*GameLevel;
extern Player	*P;
extern float	*Sin;
extern float	*Cos;
extern float	*Tan;

/*-----------------
---- FUNCTIONS ----
-----------------*/

void Light::LinkSector(void)
{
	Sector *current = GameLevel->SectorList;
	while (current != NULL) {
		if (current->id == targetid) {
			InSector = current;
			break;
		}
		current = current->next;
	}
}

void Light::Render(Sector *sect)
{
	if (!on) return;
	if (c.r <= sect->c.r && c.g <= sect->c.g && c.b <= sect->c.b) return; // if the light is darker than the sector ambient light, leave
//	if (sect != InSector) {
//		Line

		// Loop sectors
//		Portal *currentp = InSector->portal;
//		while (currentp != NULL) {
			



//			currentp = currentp->next;
//		}

//	} else {	///// Start the sect = InSect section
	
		Portal *current1 = sect->portal;
		while (current1 != NULL) {
			float Dist = current1->FindDist(p);

			if (Dist < radius && Dist > 0 && current1->visible && (current1->s->floory > sect->floory || current1->s->ceily < sect->ceily)) {
				// Find wall lighting
				float cosval = Cos[(int)(Dist * distcosfade)];
				float useradius = radius * cosval;
				Color usecolor;
				usecolor.r = c.r * cosval;
				usecolor.g = c.g * cosval;
				usecolor.b = c.b * cosval;

				Point2D pint, pmin, pmax;
				pint.x = p.x - current1->n.x * Dist;
				pint.z = p.z + current1->n.z * -Dist;

				pmin.x = pint.x - useradius * current1->sl.x;
				pmin.z = pint.z - useradius * current1->sl.z;
				pmax.x = pint.x + useradius * current1->sl.x;
				pmax.z = pint.z + useradius * current1->sl.z;

				// Clip lighting to wall limits
				bool render = false;
				float wlim1, wlim2, minp, maxp, useyf, useyc, useminu = 0, usemaxu = 1, useminv = 0, usemaxv = 1, distbetween;
				float deltauv = 1.0f / (useradius * 2);

				if (current1->p1.x != current1->p2.x) {
					wlim1 = current1->p1.x;
					wlim2 = current1->p2.x;
					minp = pmin.x;
					maxp = pmax.x;
				} else {
					wlim1 = current1->p1.z;
					wlim2 = current1->p2.z;
					minp = pmin.z;
					maxp = pmax.z;
				}

				if (wlim1 > wlim2) {
					if (minp > wlim2 && maxp < wlim1) {
						render = true;
						if (minp > wlim1) {
							distbetween = sqrtf((current1->p1.x - pmin.x) * (current1->p1.x - pmin.x) + (current1->p1.z - pmin.z) * (current1->p1.z - pmin.z));
							pmin = current1->p1;
							useminu += distbetween * deltauv;
						}
						if (maxp < wlim2) {
							distbetween = sqrtf((current1->p2.x - pmax.x) * (current1->p2.x - pmax.x) + (current1->p2.z - pmax.z) * (current1->p2.z - pmax.z));
							pmax = current1->p2;
							usemaxu -= distbetween * deltauv;
						}
					}
				} else {
					if (minp < wlim2 && maxp > wlim1) {
						render = true;
						if (minp < wlim1) {
							distbetween = sqrtf((current1->p1.x - pmin.x) * (current1->p1.x - pmin.x) + (current1->p1.z - pmin.z) * (current1->p1.z - pmin.z));
							pmin = current1->p1;
							useminu += distbetween * deltauv;
						}
						if (maxp > wlim2) {
							distbetween = sqrtf((current1->p2.x - pmax.x) * (current1->p2.x - pmax.x) + (current1->p2.z - pmax.z) * (current1->p2.z - pmax.z));
							pmax = current1->p2;
							usemaxu -= distbetween * deltauv;
						}
					}
				}

				// Render the lighting
				if (render) {
					glColor3f(usecolor.r,usecolor.g,usecolor.b);
					if (current1->s->floory > sect->floory) {
						useyc = y+useradius;
						if (useyc > current1->s->floory) {
							usemaxv = 1 - deltauv * (useyc - current1->s->floory);
							useyc = current1->s->floory;
						}
						useyf = y-useradius;
						if (useyf < sect->floory) {
							useminv = deltauv * (sect->floory - useyf);
							useyf = sect->floory;
						}

						if (useyc > useyf) {
							glBegin(GL_QUADS);
								glTexCoord2f(useminu,usemaxv);
								glVertex3f(pmin.x,useyc,pmin.z);

								glTexCoord2f(usemaxu,usemaxv);
								glVertex3f(pmax.x,useyc,pmax.z);

								glTexCoord2f(usemaxu,useminv);
								glVertex3f(pmax.x,useyf,pmax.z);
	
								glTexCoord2f(useminu,useminv);
								glVertex3f(pmin.x,useyf,pmin.z);
							glEnd();
						}
					}		

					useminv = 0; usemaxv = 1;
					if (current1->s->ceily < sect->ceily) {
						useyc = y+useradius;
						if (useyc > sect->ceily) {
							usemaxv = 1 - deltauv * (useyc - sect->ceily);
							useyc = sect->ceily;
						}
						useyf = y-useradius;
						if (useyf < current1->s->ceily) {
							useminv = deltauv * (current1->s->ceily - useyf);
							useyf = current1->s->ceily;
						}

						if (useyc > useyf) {
							glBegin(GL_QUADS);
								glTexCoord2f(useminu,usemaxv);
								glVertex3f(pmin.x,useyc,pmin.z);

								glTexCoord2f(usemaxu,usemaxv);
								glVertex3f(pmax.x,useyc,pmax.z);

								glTexCoord2f(usemaxu,useminv);
								glVertex3f(pmax.x,useyf,pmax.z);
	
								glTexCoord2f(useminu,useminv);
								glVertex3f(pmin.x,useyf,pmin.z);
							glEnd();
						}
					}
				}
			}
			current1 = current1->next;
		}

		Wall *current2 = sect->wall;
		while (current2 != NULL) {
			float Dist = current2->FindDist(p);

			if (Dist < radius && Dist > 0 && current2->visible) {
				// Find wall lighting
				float cosval = Cos[(int)(Dist * distcosfade)];
				float useradius = radius * cosval;
				Color usecolor;
				usecolor.r = c.r * cosval;
				usecolor.g = c.g * cosval;
				usecolor.b = c.b * cosval;

				Point2D pint, pmin, pmax;
				pint.x = p.x - current2->n.x * Dist;
				pint.z = p.z + current2->n.z * -Dist;

				pmin.x = pint.x - useradius * current2->sl.x;
				pmin.z = pint.z - useradius * current2->sl.z;
				pmax.x = pint.x + useradius * current2->sl.x;
				pmax.z = pint.z + useradius * current2->sl.z;

				// Clip lighting to wall limits
				bool render = false;
				float wlim1, wlim2, minp, maxp, useyf, useyc, useminu = 0, usemaxu = 1, useminv = 0, usemaxv = 1, distbetween;
				float deltauv = 1.0f / (useradius * 2);

				if (current2->p1.x != current2->p2.x) {
					wlim1 = current2->p1.x;
					wlim2 = current2->p2.x;
					minp = pmin.x;
					maxp = pmax.x;
				} else {
					wlim1 = current2->p1.z;
					wlim2 = current2->p2.z;
					minp = pmin.z;
					maxp = pmax.z;
				}

				if (wlim1 > wlim2) {
					if (minp > wlim2 && maxp < wlim1) {
						render = true;
						if (minp > wlim1) {
							distbetween = sqrtf((current2->p1.x - pmin.x) * (current2->p1.x - pmin.x) + (current2->p1.z - pmin.z) * (current2->p1.z - pmin.z));
							pmin = current2->p1;
							useminu += distbetween * deltauv;
						}
						if (maxp < wlim2) {
							distbetween = sqrtf((current2->p2.x - pmax.x) * (current2->p2.x - pmax.x) + (current2->p2.z - pmax.z) * (current2->p2.z - pmax.z));
							pmax = current2->p2;
							usemaxu -= distbetween * deltauv;
						}
					}
				} else {
					if (minp < wlim2 && maxp > wlim1) {
						render = true;
						if (minp < wlim1) {
							distbetween = sqrtf((current2->p1.x - pmin.x) * (current2->p1.x - pmin.x) + (current2->p1.z - pmin.z) * (current2->p1.z - pmin.z));
							pmin = current2->p1;
							useminu += distbetween * deltauv;
						}
						if (maxp > wlim2) {
							distbetween = sqrtf((current2->p2.x - pmax.x) * (current2->p2.x - pmax.x) + (current2->p2.z - pmax.z) * (current2->p2.z - pmax.z));
							pmax = current2->p2;
							usemaxu -= distbetween * deltauv;
						}
					}
				}

				useyc = y+useradius;
				if (useyc > sect->ceily) {
					usemaxv = 1 - deltauv * (useyc - sect->ceily);
					useyc = sect->ceily;
				}
				useyf = y-useradius;
				if (useyf < sect->floory) {
					useminv = deltauv * (sect->floory - useyf);
					useyf = sect->floory;
				}

				// Render the lighting
				if (render) {
					glColor3f(usecolor.r,usecolor.g,usecolor.b);
					glBegin(GL_QUADS);
						glTexCoord2f(useminu,usemaxv);
						glVertex3f(pmin.x,useyc,pmin.z);

						glTexCoord2f(usemaxu,usemaxv);
						glVertex3f(pmax.x,useyc,pmax.z);

						glTexCoord2f(usemaxu,useminv);
						glVertex3f(pmax.x,useyf,pmax.z);
	
						glTexCoord2f(useminu,useminv);
						glVertex3f(pmin.x,useyf,pmin.z);
					glEnd();
				}
			}
			current2 = current2->next;
		}

		// Find floor and ceiling lighting
		Line2D l1, l2, l3;
		float fDist = y - sect->floory;
		float cDist = sect->ceily - y;

		if (fDist < radius && fDist > 0) {
			float cosvalf = Cos[(int)(fDist * distcosfade)];
			float useradiusf = radius * cosvalf;
			float uvscalef = 0.5f / useradiusf;
			Color usecolorf;
			usecolorf.r = c.r * cosvalf;
			usecolorf.g = c.g * cosvalf;
			usecolorf.b = c.b * cosvalf;

			glColor3f(usecolorf.r,usecolorf.g,usecolorf.b);

			Point2D *currentp = sect->point->next;
			while (currentp != NULL && currentp->next != NULL) {
				l1.Reset(sect->point,currentp);
				l2.Reset(currentp,currentp->next);
				l3.Reset(currentp->next,sect->point);

				if (l1.FindDist(p) > -useradiusf && l2.FindDist(p) > -useradiusf && l3.FindDist(p) > -useradiusf) {				
					glBegin(GL_TRIANGLES);
						glTexCoord2f(0.5f-(p.x-sect->point->x)*uvscalef,0.5f-(p.z-sect->point->z)*uvscalef);
						glVertex3f(sect->point->x,sect->floory,sect->point->z);					

						glTexCoord2f(0.5f-(p.x-currentp->x)*uvscalef,0.5f-(p.z-currentp->z)*uvscalef);
						glVertex3f(currentp->x,sect->floory,currentp->z);

						glTexCoord2f(0.5f-(p.x-currentp->next->x)*uvscalef,0.5f-(p.z-currentp->next->z)*uvscalef);
						glVertex3f(currentp->next->x,sect->floory,currentp->next->z);
					glEnd();
				}

				currentp = currentp->next;
			}

			/*Point2D *currentp = sect->point;
			glBegin(GL_TRIANGLE_FAN);
				while (currentp != NULL) {
					glTexCoord2f(0.5f-(p.x-currentp->x)*uvscalef,0.5f-(p.z-currentp->z)*uvscalef);
					glVertex3f(currentp->x,sect->floory,currentp->z);
					currentp = currentp->next;
				}
			glEnd();*/

		}

		if (cDist < radius && cDist > 0 && sect->ceiltexid != -1) {
			float cosvalc = Cos[(int)(cDist * distcosfade)];
			float useradiusc = radius * cosvalc;
			float uvscalec = 0.5f / useradiusc;
			Color usecolorc;
			usecolorc.r = c.r * cosvalc;
			usecolorc.g = c.g * cosvalc;
			usecolorc.b = c.b * cosvalc;

			glColor3f(usecolorc.r,usecolorc.g,usecolorc.b);

			Point2D *currentp = sect->point->next;
			while (currentp != NULL && currentp->next != NULL) {
				l1.Reset(sect->point,currentp);
				l2.Reset(currentp,currentp->next);
				l3.Reset(currentp->next,sect->point);

				if (l1.FindDist(p) > -useradiusc && l2.FindDist(p) > -useradiusc && l3.FindDist(p) > -useradiusc) {				
					glBegin(GL_TRIANGLES);
						glTexCoord2f(0.5f-(p.x-sect->point->x)*uvscalec,0.5f-(p.z-sect->point->z)*uvscalec);
						glVertex3f(sect->point->x,sect->ceily,sect->point->z);					

						glTexCoord2f(0.5f-(p.x-currentp->x)*uvscalec,0.5f-(p.z-currentp->z)*uvscalec);
						glVertex3f(currentp->x,sect->ceily,currentp->z);

						glTexCoord2f(0.5f-(p.x-currentp->next->x)*uvscalec,0.5f-(p.z-currentp->next->z)*uvscalec);
						glVertex3f(currentp->next->x,sect->ceily,currentp->next->z);
					glEnd();
				}

				currentp = currentp->next;
			}
			
			/*Point2D *currentp = sect->point;
			glBegin(GL_TRIANGLE_FAN);
				while (currentp != NULL) {
					glTexCoord2f(0.5f-(p.x-currentp->x)*uvscalec,0.5f-(p.z-currentp->z)*uvscalec);
					glVertex3f(currentp->x,sect->ceily,currentp->z);
					currentp = currentp->next;
				}
			glEnd();*/
		}
//	}
}

void Light::Update(Sector *sect, int count)
// This function checks to see which sectors the light effects, first time Update(light->InSector,0);
{
	if ((oldp.x != p.x || oldp.z != p.z || oldy != y) || count != 0) {	// if the light moved find which sectors it now effects
		SectorNode *currents, *temps;
		LightNode *currentl, *templ;

		if (count == 0) {
			currents = Sectors;				// delete the current sector list and the lights in the sectors' light lists
			while (currents != NULL) {
				LightNode *back = NULL;
				currentl = currents->sector->Lights;
				while (currentl != NULL) {
					if (currentl->light == this) {
						if (back != NULL) {
							back->next = currentl->next;
						} else {
							currents->sector->Lights = currentl->next;
						}
						delete currentl;
						break;
					}
					back = currentl;
					currentl = currentl->next;
				}

				temps = currents;
				currents = currents->next;
				delete temps;
			}

			Sectors = new SectorNode(InSector);
			templ = InSector->Lights;
			InSector->Lights = new LightNode(this);
			InSector->Lights->next = templ;

			oldp = p;
			oldy = y;
		}
		
		Portal *current = sect->portal;
		while (current != NULL) {
			float Dist = current->FindDist(p);

			if (Dist < (radius*0.9f) && Dist > 0) {
				bool intersect = false;
				float wlim1, wlim2, minp, maxp;
				float cosval = Cos[(int)(Dist * distcosfade)];
				float useradius = radius * cosval;

				Point2D pint, pmin, pmax;
				pint.x = p.x - current->n.x * Dist;
				pint.z = p.z + current->n.z * -Dist;

				pmin.x = pint.x - useradius * current->sl.x;
				pmin.z = pint.z - useradius * current->sl.z;
				pmax.x = pint.x + useradius * current->sl.x;
				pmax.z = pint.z + useradius * current->sl.z;

				if (current->p1.x != current->p2.x) {
					wlim1 = current->p1.x;
					wlim2 = current->p2.x;
					minp = pmin.x;
					maxp = pmax.x;
				} else {
					wlim1 = current->p1.z;
					wlim2 = current->p2.z;
					minp = pmin.z;
					maxp = pmax.z;
				}

				if (wlim1 > wlim2) {
					if (minp > wlim2 && maxp < wlim1) intersect = true;
				} else {
					if (minp < wlim2 && maxp > wlim1) intersect = true;
				}

				currents = Sectors;								// Make sure it doesn't count a sector twice in the list
				while (currents != NULL) {
					if (current->s == currents->sector) {
						intersect = false;
						break;
					}
					currents = currents->next;
				}

				if (intersect) {
					temps = Sectors;							// Add sector to the front of the list
					Sectors = new SectorNode(current->s);
					Sectors->next = temps;

					bool addtosectorlist = true;
					currentl = current->s->Lights;
					while (currentl != NULL) {
						if (currentl->light == this) {
							addtosectorlist = false;
							break;
						}
						currentl = currentl->next;
					}
					if (addtosectorlist) {
						templ = current->s->Lights;
						current->s->Lights = new LightNode(this);
						current->s->Lights->next = templ;
					}

					Update(current->s,count+1);
				}
			}
			current = current->next;
		}
	}
}

Light::Light(float _x,float _y,float _z,float _radius,float _r,float _g,float _b,int _sectorid)
{
	p.x = _x;
	p.z = _z;
	oldp.x = p.x + 1;	// so the first frame it'll compute the sectors
	oldp.z = p.z + 1;
	y = _y;
	oldy = y + 1;
	radius = _radius;
	distcosfade = ANGLE90 / radius;
	c.r = _r;
	c.g = _g;
	c.b = _b;
	on = true;
	targetid = _sectorid;
	InSector = NULL;
	Sectors = NULL;
	next = NULL;
}

Light::~Light()
{
	SectorNode *current = Sectors, *temp;
	while (current != NULL) {
		temp = current;
		current = current->next;
		delete temp;
	}
}