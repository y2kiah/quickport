/* ----==== FRUSTRUM.H ====---- */

#ifndef FRUSTRUM_H
#define FRUSTRUM_H

#include "world.h"

/*------------------
---- STRUCTURES ----
------------------*/

class Frustrum {
	public:
		Frustrum	*next;
		Point2D		op[2];		// Original frustrum points
		Point2D		wp[2];		// World points updated with GetWorldPoints
		Line2D		ln[3];		// Planes that make up the edges of the frustrum. 5th plane set up manually

//		void GetWorldPoints(Point2D *p,int ZAngle,int XAngle,int YAngle);
		void GetWorldPoints(Point2D *p,int Angle);
//		Frustrum(float Angle, float SeeDist);
		Frustrum(float Angle);
		Frustrum(Point2D *origin, Point2D *left, Point2D *right);
};

class FrustrumList {
	public:
		Frustrum		*list;

		void Push(Point2D *origin, Point2D *left, Point2D *right);
		void PushCheck(Point2D *origin, Point2D *left, Point2D *right);
		void Pop(void);
		FrustrumList() { list = NULL; }
		~FrustrumList();
};

#endif