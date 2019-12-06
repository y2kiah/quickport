/* ----==== QUICKPORT LEVEL EDITOR ====----

	By: Jeff Kiah
	Started:	3/02
*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
//#include <stdlib.h>
#include <stdio.h>
//#include <stdarg.h>
//#include <iostream.h>
//#include <fstream.h>
//#include <ctype.h>
//#include <io.h>
//#include <direct.h>
#include "resource.h"
#include "level.h"

/*-----------------
---- CONSTANTS ----
-----------------*/

#define RESX	1600
#define RESY	1200

/*-----------------
---- VARIABLES ----
-----------------*/

MSG				Msg;
HINSTANCE		hInst;
WNDCLASSEX		Wc;
HWND			hWnd = NULL;
HDC				hDc = NULL;
HGLRC			hRc = NULL;
unsigned int	Font1;
bool			Key[256];
char			*TempString1 = new char[256];

/*-----------------
---- FUNCTIONS ----
-----------------*/

void LoadFont(void)
{
	HFONT NewFont;
	Font1 = glGenLists(96);

	NewFont = CreateFont(	-24,							// Height Of Font
							0,								// Width Of Font
							0,								// Angle Of Escapement
							0,								// Orientation Angle
							FW_BOLD,						// Font Weight
							FALSE,							// Italic
							FALSE,							// Underline
							FALSE,							// Strikeout
							ANSI_CHARSET,					// Character Set Identifier
							OUT_TT_PRECIS,					// Output Precision
							CLIP_DEFAULT_PRECIS,			// Clipping Precision
							ANTIALIASED_QUALITY,			// Output Quality
							FF_DONTCARE | DEFAULT_PITCH,	// Family And Pitch
							"Courier New");					// Font Name

	SelectObject(hDc, NewFont);
	wglUseFontBitmaps(hDc, 32, 96, Font1);
}

void DeleteFont(void)
{
	glDeleteLists(Font1, 96);
}

void Print(const char *Text, ...)
{
	TempString1[0] = NULL;
	va_list	Arg;								// Pointer to list of arguments

	if (Text == NULL) return;

	va_start(Arg, Text);						// Parses the string for variables
	vsprintf(TempString1, Text, Arg);			// And converts symbols to actual numbers
	va_end(Arg);								// Results are stored in text line

	glPushAttrib(GL_LIST_BIT);
	
	glListBase(Font1 - 32);
	glCallLists(strlen(TempString1), GL_UNSIGNED_BYTE, TempString1);

	glPopAttrib();
}

/*void LoadMesh(char *Filename, int Sub)
{
	MeshInfo *Temp = new MeshInfo(Filename);
	if (!Temp) {
		MessageBox(0,".3d File Not Found","Error",MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
		return;
	}

	Mesh[Sub] = glGenLists(1);
	glNewList(Mesh[Sub],GL_COMPILE);
	Temp->Display();
	glEndList();

	delete Temp;
}

void LoadMeshes(void)
{
	_finddata_t FileData;
	unsigned int Count = 0;
	
	_chdir("data/3d/");
	
	long FilePos = _findfirst("*.3d",&FileData);
	do {
		NumMeshes++;
	} while (!_findnext(FilePos,&FileData));

	Mesh = new unsigned int[NumMeshes];
	MeshName = new FileString[NumMeshes];

	FilePos = _findfirst("*.3d",&FileData);
	do {
		strcpy(MeshName[Count].Name,CharUpper(FileData.name));
		LoadMesh(MeshName[Count].Name,Count);
		Count++;
	} while (!_findnext(FilePos,&FileData));

	_chdir("../../");
}*/

void InitOpenGL(void)
{
	glViewport(0,0,RESX,RESY);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,RESX,RESY,0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glShadeModel(GL_SMOOTH);
	glCullFace(GL_BACK);
	glClearColor(1,1,1,0);
	glClearDepth(1.0);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
	glHint(GL_FOG_HINT, GL_NICEST);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glDisable(GL_FOG);
	glDisable(GL_NORMALIZE);
	glDisable(GL_DITHER);
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_LOGIC_OP);

	LoadFont();
}

void EnableOpenGL(void)
{	
	PIXELFORMATDESCRIPTOR pfd;
	UINT PixelFormat;

	hDc = GetDC(hWnd);
			
	ZeroMemory(&pfd,sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 16;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;					

	PixelFormat = ChoosePixelFormat(hDc, &pfd);
	if (!PixelFormat) {
		MessageBox(0,"Cannot Find Suitable Pixel Format","Error",MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
		return;
	}

	if (!SetPixelFormat(hDc, PixelFormat, &pfd)) {
		MessageBox(0,"Cannot Set The Pixel Format","Error",MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
		return;
	}

	hRc = wglCreateContext(hDc);
	if (!hRc) {
		MessageBox(0,"Cannot Create An OpenGL Rendering Context","Error",MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
		return;
	}

	if (!wglMakeCurrent(hDc, hRc)) {
		MessageBox(0,"Cannot Activate GLRC","Error",MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
		return;
	}
}

void DisableOpenGL(void)
{
	wglMakeCurrent(NULL,NULL);
	wglDeleteContext(hRc);
	ReleaseDC(hWnd,hDc);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_CREATE:
			return 0;

		case WM_DESTROY:
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case ID_FILE_EXIT:
					PostQuitMessage(0);
					break;
			}
			return 0;

		case WM_KEYDOWN:
			Key[wParam] = TRUE;
			switch (wParam) {
				case VK_ESCAPE: 
					SendMessage(hWnd,WM_CLOSE,0,0);
					break;
			}
			return 0;

		case WM_KEYUP:
			Key[wParam] = FALSE;
//			switch (wParam) {
//			}
			return 0;

		default:
			return DefWindowProc(hwnd,msg,wParam,lParam);
	}

	return 0;
}

int InitWindow(HINSTANCE hinst, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int iCmdShow)
{
	Wc.cbSize			= sizeof(Wc);
	Wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	Wc.lpfnWndProc		= WndProc;
	Wc.cbClsExtra		= 0;
	Wc.cbWndExtra		= 0;
	Wc.hInstance		= hInst;
	Wc.hIcon			= LoadIcon(NULL,IDI_APPLICATION);
	Wc.hCursor			= LoadCursor(NULL,IDC_ARROW);
	Wc.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	Wc.lpszMenuName		= "Menu";
	Wc.lpszClassName	= "Level Editor";
	Wc.hIconSm			= LoadIcon(NULL,IDI_APPLICATION);

	if (!RegisterClassEx(&Wc)) {
		MessageBox(0,"Failed To Register The Window Class","Error",MB_OK | MB_ICONERROR);
		return FALSE;
	}

	hWnd = CreateWindowEx(WS_EX_TOPMOST,"Level Editor","Level Editor",WS_OVERLAPPEDWINDOW,0,0,RESX,RESY,NULL,NULL,hInst,NULL);
	if (!hWnd) {
		MessageBox(0,"Window Creation Error","Error",MB_OK | MB_ICONERROR);
		return FALSE;
	}
	
	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);
	SetFocus(hWnd);
	wglMakeCurrent(hDc,hRc);	

	return TRUE;
}

void DrawBackground(void)
{
	glColor3f(0.8f,0.8f,0.8f);
	glBegin(GL_QUADS);
		glVertex2i(1600,0);
		glVertex2i(1200,0);
		glVertex2i(1200,1200);
		glVertex2i(1600,1200);
	glEnd();
}

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int iCmdShow)
{		
	for (int c = 0; c < 256; c++) Key[c] = FALSE;

	if (!InitWindow(hInst,NULL,lpszCmdLine,iCmdShow)) PostQuitMessage(0);
	EnableOpenGL();
	InitOpenGL();

	while (1) {
		if (PeekMessage(&Msg,NULL,0,0,PM_NOREMOVE)) {
			if (!GetMessage(&Msg,NULL,0,0)) break;
			TranslateMessage(&Msg); 
			DispatchMessage(&Msg);
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		DrawBackground();

		glFlush();
		SwapBuffers(hDc);
	}

	ChangeDisplaySettings(NULL,NULL);
	DisableOpenGL();

	DestroyWindow(hWnd);
	DeleteFont();

	return Msg.wParam;
}