/* ----==== TEXTURE.CPP ====---- */

#include <windows.h>
#include <gl\gl.h>
#include <gl\glaux.h>
//#include <iostream.h>
#include <string.h>
#include "texture.h"
#include "main.h"

/*-----------------
---- VARIABLES ----
-----------------*/

extern GameOptions Options;

/*-----------------
---- FUNCTIONS ----
-----------------*/

////////// class Texture //////////

void Texture::LoadTexture(const char *_filename,const int numframes,const bool clamp)
{
	num_frames = numframes;
	texture = new unsigned int[numframes];
	glGenTextures(numframes,texture);

	AUX_RGBImageRec *Temp = auxDIBImageLoad(_filename);
	if (!Temp) {
		MessageBox(0,".bmp File Not Found","Error",MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
		return;		
	}

	w = Temp->sizeX;
	h = Temp->sizeY;
	w_tex_dist = (float)w / (float)Options.TEXSIZE * 10.0f;
	h_tex_dist = (float)h / (float)Options.TEXSIZE * 10.0f;

	glBindTexture(GL_TEXTURE_2D,texture[0]); // change 0 to current frame that is being loaded

	if (Options.FILTERING) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if (!Options.MIPMAP) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			if (clamp) {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			} else {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}
			glTexImage2D(GL_TEXTURE_2D,0,3,Temp->sizeX,Temp->sizeY,0,GL_RGB,GL_UNSIGNED_BYTE,Temp->data);
		} else {
			if (Options.NICEGRAPHICS) glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			else glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
			if (clamp) {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			} else {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}
			gluBuild2DMipmaps(GL_TEXTURE_2D,3,Temp->sizeX,Temp->sizeY,GL_RGB,GL_UNSIGNED_BYTE,Temp->data);
		}
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		if (!Options.MIPMAP) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			if (clamp) {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			} else {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}
			glTexImage2D(GL_TEXTURE_2D,0,3,Temp->sizeX,Temp->sizeY,0,GL_RGB,GL_UNSIGNED_BYTE,Temp->data);
		} else {
			if (Options.NICEGRAPHICS) glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_NEAREST);
			else glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_LINEAR);
			if (clamp) {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			} else {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}
			gluBuild2DMipmaps(GL_TEXTURE_2D,3,Temp->sizeX,Temp->sizeY,GL_RGB,GL_UNSIGNED_BYTE,Temp->data);
		}
	}

	filename = new char[strlen(_filename)];
	strcpy(filename,_filename);
}

Texture::Texture()
{
	texture = NULL;
	num_frames = w = h = 0;
	w_tex_dist = h_tex_dist = 0;
	filename = NULL;
}

Texture::~Texture()
{
	glDeleteTextures(num_frames,texture);
	delete [] texture;
	delete [] filename;
}