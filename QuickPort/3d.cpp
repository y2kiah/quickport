/* ----==== 3D.CPP ====---- */

#include <windows.h>
#include <gl\gl.h>
#include <iostream.h>
#include <fstream.h>
#include "3d.h"
#include "main.h"
#include "world.h"
#include "texture.h"

/*-----------------
---- VARIABLES ----
-----------------*/

extern Level	*GameLevel;
//extern GameOptions Options;

/*-----------------
---- FUNCTIONS ----
-----------------*/

////////// class MeshFace //////////

MeshFace::MeshFace(Point3D *_vt1,Point3D *_vt2,Point3D *_vt3,float _u1,float _v1,float _u2,float _v2,float _u3,float _v3,Color *_c1,Color *_c2,Color *_c3,int _texid)
{
	v1 = *_vt1;
	v2 = *_vt2;
	v3 = *_vt3;
//	n1 = *_n1;
//	n2 = *_n2;
//	n3 = *_n3;
	uv[0][0] = _u1;	uv[0][1] = _v1;
	uv[1][0] = _u2;	uv[1][1] = _v2;
	uv[2][0] = _u3;	uv[2][1] = _v3;
	c1 = *_c1;
	c2 = *_c2;
	c3 = *_c3;
	texid = _texid;
	next = NULL;
}

////////// class Mesh3d //////////

void Mesh3d::Render(void) const
{
	glEnable(GL_CULL_FACE);
	glColor3f(1,1,1);

	MeshFace *current = FaceList;
	while (current != NULL) {	
		glBindTexture(GL_TEXTURE_2D,GameLevel->TextureList[current->texid].texture[0]);
		glBegin(GL_TRIANGLES);
			glTexCoord2f(current->uv[2][0],current->uv[2][1]);
			glVertex3f(current->v3.x,current->v3.y,-current->v3.z-15);

			glTexCoord2f(current->uv[1][0],current->uv[1][1]);
			glVertex3f(current->v2.x,current->v2.y,-current->v2.z-15);

			glTexCoord2f(current->uv[0][0],current->uv[0][1]);
			glVertex3f(current->v1.x,current->v1.y,-current->v1.z-15);
		glEnd();

		current = current->next;
	}
}

Mesh3d::Mesh3d(char *Filename)
{
	ifstream inFile(Filename,ios::nocreate);
	if (!inFile) {
		MessageBox(0,".3d File Not Found","Error",MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
		return;
	}

	FaceList = NULL;
	MeshFace *LastPtr = NULL;
	Point3D sv1, sv2, sv3;
	float u1,v1,u2,v2,u3,v3;
	Color c1,c2,c3;
//	Vector sn1, sn2, sn3;
	int	TexCount;
	int InputTexId;
	char Buffer[30];

	inFile >> NumFaces;
	inFile >> TexCount;

	int *SetTexId = new int[TexCount];

	for (int c = 0; c < TexCount; c++) {
		Buffer[0] = NULL;		
		inFile >> Buffer;

		for (int t = 0; t < NUMTEXTURES; t++) {
//			if (!strcmp(TextureName[t].Name,CharUpper(Buffer))) {
//				SetTexId[c] = t;
//				break;
//			}
		}
	}

	for (c = 0; c < NumFaces; c++) {
		inFile >> sv1.x   >> sv1.y   >> sv1.z
			   >> u1	  >> v1
			   >> c1.r	  >> c1.g	 >> c1.b
			   >> sv2.x   >> sv2.y   >> sv2.z
			   >> u2	  >> v2
			   >> c2.r	  >> c2.g	 >> c2.b
			   >> sv3.x   >> sv3.y   >> sv3.z
			   >> u3	  >> v3
			   >> c3.r	  >> c3.g	 >> c3.b
//			   >> sn1.x   >> sn1.y   >> sn1.z
//			   >> sn2.x   >> sn2.y   >> sn2.z
//			   >> sn3.x   >> sn3.y   >> sn3.z
			   >> InputTexId;

		if (FaceList == NULL)
			FaceList = LastPtr = new MeshFace(&sv1,&sv2,&sv3,u1,v1,u2,v2,u3,v3,&c1,&c2,&c3,5);//SetTexId[InputTexId]);
		else {
			LastPtr->next = new MeshFace(&sv1,&sv2,&sv3,u1,v1,u2,v2,u3,v3,&c1,&c2,&c3,5);//SetTexId[InputTexId]);
			LastPtr = LastPtr->next;
		}
	}

	delete [] SetTexId;

	inFile.close();
}

Mesh3d::~Mesh3d()
{
	MeshFace *current = FaceList, *temp;

	while (current != NULL) {
		temp = current;
		current = current->next;
		delete temp;
	}
}
