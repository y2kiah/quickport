/* ----==== QUICKPORT ENGINE v 0.1 ====----

	By:			Jeff Kiah
	Started:	2/02
*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <fmod.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <mmsystem.h>
#include <fstream.h>
#include "main.h"
#include "world.h"
#include "3d.h"

#include "texture.h"
#include "object.h"

/*-----------------
---- VARIABLES ----
-----------------*/

GameOptions		Options("game.cfg");

HINSTANCE		hInst;
MSG				Msg;
HWND			hWnd = NULL;
HDC				hDC = NULL;
HGLRC			hRC = NULL;

_int64			TimerFrequency, CurrentTime, LastTime;
float			TimeFix, TimerFrequencyFix, fps;
DWORD			fpsTime;
unsigned int	Font1;
char			*TempString;
bool			Active = true;
bool			ButtonDown[256];
int				NumFrames = 0;

FSOUND_SAMPLE	*Sound[2];
FSOUND_STREAM	*Stream;
FMUSIC_MODULE	*Music[4];
int				CurrentSong;

extern Level	*GameLevel;

// temp
Mesh3d *mesh;
extern Player *P;

/*-----------------
---- FUNCTIONS ----
-----------------*/

////////// class Options //////////

GameOptions::GameOptions(char *_filename)
{
	int boolset;
	char *Buffer = new char[80];

	ifstream inFile(_filename, ios::nocreate);

	if (!inFile) {
		MessageBox(0,_filename,"File not found",MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
		return;		
	}

	while (!inFile.eof()) {
		Buffer[0] = NULL;
		
		inFile.getline(Buffer,80);

		if (!strcmp(Buffer,"[RESX]")) {
			inFile >> RESX;
		} else if (!strcmp(Buffer,"[RESY]")) {
			inFile >> RESY;
		} else if (!strcmp(Buffer,"[COLORDEPTH]")) {
			inFile >> COLORDEPTH;
		} else if (!strcmp(Buffer,"[ZBUFFERDEPTH]")) {
			inFile >> ZDEPTH;
		} else if (!strcmp(Buffer,"[TEXTURESIZE]")) {
			inFile >> TEXSIZE;
		} else if (!strcmp(Buffer,"[NICEGRAPHICS]")) {
			inFile >> boolset;
			NICEGRAPHICS = boolset == 1 ? true : false;
		} else if (!strcmp(Buffer,"[MIPMAP]")) {
			inFile >> boolset;
			MIPMAP = boolset == 1 ? true : false;
		} else if (!strcmp(Buffer,"[FILTERING]")) {
			inFile >> boolset;
			FILTERING = boolset == 1 ? true : false;
		} else if (!strcmp(Buffer,"[FULLSCREEN]")) {
			inFile >> boolset;
			FULLSCREEN = boolset == 1 ? true : false;
		}
	}

	inFile.close();

	delete [] Buffer;
}

//////////

extern void FreeMemory(void);
extern void SetupGame(void);
extern void PlayGame(void);

float GetTimePassed(void) {
	QueryPerformanceCounter((LARGE_INTEGER *)&CurrentTime);
	float Offset = (float)(CurrentTime - LastTime) * TimerFrequencyFix;
	LastTime = CurrentTime;
	
	return Offset;
}


void LoadFont(void)
{
	HFONT NewFont;
	Font1 = glGenLists(96);

	NewFont = CreateFont(	-20,							// Height Of Font
							0,								// Width Of Font
							0,								// Angle Of Escapement
							0,								// Orientation Angle
							FW_NORMAL,						// Font Weight
							FALSE,							// Italic
							FALSE,							// Underline
							FALSE,							// Strikeout
							ANSI_CHARSET,					// Character Set Identifier
							OUT_TT_PRECIS,					// Output Precision
							CLIP_DEFAULT_PRECIS,			// Clipping Precision
							ANTIALIASED_QUALITY,			// Output Quality
							FF_DONTCARE | DEFAULT_PITCH,	// Family And Pitch
							"Courier");						// Font Name

	SelectObject(hDC, NewFont);
	wglUseFontBitmaps(hDC, 32, 96, Font1);

	TempString = new char[256];
}

void DeleteFont(void)
{
	delete [] TempString;
	glDeleteLists(Font1, 96);
}

void Print(const char *Text, ...)
{
	TempString[0] = NULL;
	va_list	Arg;								// Pointer to list of arguments

	if (Text == NULL) return;

	va_start(Arg, Text);						// Parses the string for variables
	vsprintf(TempString, Text, Arg);			// And converts symbols to actual numbers
	va_end(Arg);								// Results are stored in text line

	glPushAttrib(GL_LIST_BIT);
	
	glListBase(Font1 - 32);
	glCallLists(strlen(TempString), GL_UNSIGNED_BYTE, TempString);

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

void InitFMOD(void)
{
	FSOUND_SetMaxHardwareChannels(32);
	FSOUND_Init(44100,64,0);
	Sound[0] = FSOUND_Sample_Load(FSOUND_FREE,"data/sound/jump.wav",FSOUND_8BITS | FSOUND_MONO | FSOUND_LOOP_OFF,0);
	Sound[1] = FSOUND_Sample_Load(FSOUND_FREE,"data/sound/chaingun_once.wav",FSOUND_8BITS | FSOUND_MONO | FSOUND_LOOP_OFF,0);
//	Music = FSOUND_Stream_OpenFile("data/sound/m001.mp3",FSOUND_LOOP_NORMAL | FSOUND_HW3D,0);
	CurrentSong = 0;
	Music[0] = FMUSIC_LoadSong("data/music/track_01.it");
	Music[1] = FMUSIC_LoadSong("data/music/track_02.it");
	Music[2] = FMUSIC_LoadSong("data/music/track_03.it");
	Music[3] = FMUSIC_LoadSong("data/music/track_04.it");
	FMUSIC_SetMasterVolume(Music[0],128);
	FMUSIC_SetMasterVolume(Music[1],128);
	FMUSIC_SetMasterVolume(Music[2],128);
	FMUSIC_SetMasterVolume(Music[3],128);
}

void DisableFMOD(void)
{
	FSOUND_Close();
}

void HandleMusic(void)
{
	if (GetAsyncKeyState('M') & 0x8000) {
		if (!ButtonDown['M']) {
			ButtonDown['M'] = true;
//			FSOUND_Stream_Play(FSOUND_FREE,Music);
			FMUSIC_StopSong(Music[CurrentSong]);
			CurrentSong++;
			if (CurrentSong > 3) CurrentSong = 0;
			FMUSIC_PlaySong(Music[CurrentSong]);
		}
	} else ButtonDown['M'] = false;

	if (GetAsyncKeyState('N') & 0x8000) {
		if (!ButtonDown['N']) {
			ButtonDown['N'] = true;
//			FSOUND_Stream_Stop(Music);
			FMUSIC_StopSong(Music[CurrentSong]);
			CurrentSong--;
			if (CurrentSong < 0) CurrentSong = 3;
			FMUSIC_PlaySong(Music[CurrentSong]);
		}
	} else ButtonDown['N'] = false;

	if (GetAsyncKeyState('B') & 0x8000) {
		if (!ButtonDown['B']) {
			ButtonDown['B'] = true;
			if (FMUSIC_IsPlaying(Music[CurrentSong])) FMUSIC_StopSong(Music[CurrentSong]);
			else FMUSIC_PlaySong(Music[CurrentSong]);
		}
	} else ButtonDown['B'] = false;

	if (FMUSIC_IsFinished(Music[CurrentSong])) {
			FMUSIC_StopSong(Music[CurrentSong]);
			CurrentSong++;
			if (CurrentSong > 3) CurrentSong = 0;
			FMUSIC_PlaySong(Music[CurrentSong]);
	}
}

void InitOpenGL(void)
{
	glViewport(0,0,Options.RESX,Options.RESY);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (float)Options.RESX / (float)Options.RESY, 0.1, 2000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glShadeModel(GL_SMOOTH);
	glCullFace(GL_BACK);
	glClearColor(0,0,0,0);
	glClearDepth(1.0);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (Options.NICEGRAPHICS) { 
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
		glHint(GL_FOG_HINT, GL_NICEST);
	} else {
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
		glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);
		glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
		glHint(GL_POLYGON_SMOOTH_HINT,GL_FASTEST);
		glHint(GL_FOG_HINT, GL_FASTEST);
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glDisable(GL_NORMALIZE);
	glDisable(GL_DITHER);
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_LOGIC_OP);

	LoadFont();
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_CREATE:
			return 0;
			break;

		case WM_DESTROY:
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
			break;

		case WM_ACTIVATE:
			if (LOWORD(wParam) == WA_INACTIVE || HIWORD(wParam)) {
				Active = false;
				ShowCursor(true);
				FSOUND_SetPaused(FSOUND_ALL,TRUE);
				FMUSIC_SetPaused(Music[CurrentSong],TRUE);
			} else if (!Active && !HIWORD(wParam)) {
				Active = true;
				ShowCursor(false);
				SetCursorPos(Options.RESX / 2, Options.RESY / 2);
				FSOUND_SetPaused(FSOUND_ALL,FALSE);
				FMUSIC_SetPaused(Music[CurrentSong],FALSE);
			}
			return 0;
			break;

		case WM_SYSCOMMAND:
			switch (wParam) {
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
				return 0;
			}
			return 0;
			break;
	
		case WM_SIZE:
			return 0;
			break;

		case WM_LBUTTONDOWN:
			return 0;
			break;

		case WM_LBUTTONUP:
			return 0;
			break;

		case WM_RBUTTONUP:
			return 0;
			break;

		default:
			return DefWindowProc(hwnd,msg,wParam,lParam);
	}

	return 0;
}

void KillWindow(void) {
	if (Options.FULLSCREEN) {
		ChangeDisplaySettings(NULL,0);
		ShowCursor(true);
	}

	if (hRC) {
		if (!wglMakeCurrent(NULL,NULL))
			MessageBox(0,"Release Of DC And RC Failed","Error",MB_OK | MB_ICONERROR);

		if (!wglDeleteContext(hRC))
			MessageBox(0,"Release Rendering Context Failed","Error",MB_OK | MB_ICONERROR);

		hRC = NULL;
	}

	if (hDC && !ReleaseDC(hWnd,hDC)) {
		MessageBox(0,"Release Device Context Failed","Error",MB_OK | MB_ICONERROR);
		hDC = NULL;
	}

	if (hWnd && !DestroyWindow(hWnd))
	{
		MessageBox(NULL,"Could Not Release hWnd","Error",MB_OK | MB_ICONERROR);
		hWnd = NULL;
	}

	if (!UnregisterClass("QuickPort Engine",hInst)) {
		MessageBox(NULL,"Could Not Unregister Class","Error",MB_OK | MB_ICONERROR);
		hInst = NULL;
	}
}

bool InitWindow(void)
{
	WNDCLASS	wc;
	DWORD		dwExStyle;
	DWORD		dwStyle;
	UINT		PixelFormat;
	
	hInst = GetModuleHandle(NULL);

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "QuickPort Engine";

	if (!RegisterClass(&wc)) {
		MessageBox(0,"Failed To Register The Window Class","Error",MB_OK | MB_ICONERROR);
		return false;
	}

	if (Options.FULLSCREEN) {
		
		DEVMODE dmScreenSettings;

		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));		
		dmScreenSettings.dmSize	= sizeof(DEVMODE);
		dmScreenSettings.dmPelsWidth = Options.RESX;
		dmScreenSettings.dmPelsHeight = Options.RESY;
		dmScreenSettings.dmBitsPerPel = Options.COLORDEPTH;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		int Check = ChangeDisplaySettings(&dmScreenSettings,CDS_TEST);
		
		switch (Check) {
			case DISP_CHANGE_SUCCESSFUL:
				ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN);
				break;

			case DISP_CHANGE_FAILED:
				KillWindow();
				MessageBox(0,"Failed To Change To Desired Settings","Error",MB_OK | MB_ICONERROR);
				return false;

			case DISP_CHANGE_BADMODE:
				KillWindow();
				MessageBox(0,"Fullscreen Video Mode Not Supported","Error",MB_OK | MB_ICONERROR);
				return false;

			case DISP_CHANGE_RESTART:
				KillWindow();
				MessageBox(0,"Must Restart To Get Fullscreen Video Mode","Error",MB_OK | MB_ICONERROR);
				return false;
		}
	}

	if (Options.FULLSCREEN) {
		dwExStyle = WS_EX_APPWINDOW | WS_EX_TOPMOST;
		dwStyle = WS_POPUP;
	} else {
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPED;
	}

	hWnd = CreateWindowEx(dwExStyle,"QuickPort Engine","QuickPort Engine",dwStyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,0,0,Options.RESX,Options.RESY,NULL,NULL,hInst,NULL);
	if (!hWnd) {
		KillWindow();
		MessageBox(0,"Window Creation Error","Error",MB_OK | MB_ICONERROR);
		return false;
	}

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		Options.COLORDEPTH,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		Options.ZDEPTH,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	hDC = GetDC(hWnd);
	if (!hDC) {
		KillWindow();
		MessageBox(NULL,"Can't Create A Device Context","Error",MB_OK | MB_ICONERROR);
		return false;
	}

	PixelFormat = ChoosePixelFormat(hDC, &pfd);
	if (!PixelFormat) {
		KillWindow();
		MessageBox(0,"Cannot Find Suitable Pixel Format","Error",MB_OK | MB_ICONERROR);
		return false;
	}

	if (!SetPixelFormat(hDC, PixelFormat, &pfd)) {
		KillWindow();
		MessageBox(0,"Cannot Set The Pixel Format","Error",MB_OK | MB_ICONERROR);
		return false;
	}

	hRC = wglCreateContext(hDC);
	if (!hRC) {
		KillWindow();
		MessageBox(0,"Cannot Create An OpenGL Rendering Context","Error",MB_OK | MB_ICONERROR);
		return false;
	}

	if (!wglMakeCurrent(hDC, hRC)) {
		KillWindow();
		MessageBox(0,"Cannot Activate GLRC","Error",MB_OK | MB_ICONERROR);
		return false;
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	SetFocus(hWnd);
	wglMakeCurrent(hDC, hRC);	

//	ReSizeGLScene(width, height);

	return true;
}

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int iCmdShow)
{
	for (int c = 0; c < 256; c++) ButtonDown[c] = false;

	if (!InitWindow()) return 0;
	ShowCursor(false);
	SetCursorPos(Options.RESX / 2,Options.RESY / 2);

	InitOpenGL();
	InitFMOD();
	SetupGame();

	QueryPerformanceFrequency((LARGE_INTEGER *)&TimerFrequency);
	TimerFrequencyFix = 1.0f / (float)TimerFrequency;

// temp
	FMUSIC_PlaySong(Music[0]);
	mesh = new Mesh3d("data/MODELS/table_01.3d");

	srand(time(0));
	fpsTime = timeGetTime();
	TimeFix = GetTimePassed();
	while (1) {
		if (PeekMessage(&Msg,NULL,0,0,PM_REMOVE)) {
			if (Msg.message == WM_QUIT) break;
			else {
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
			}
		}

		TimeFix = GetTimePassed();

		if (Active) {
			if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) SendMessage(hWnd,WM_CLOSE,0,0);

			HandleMusic();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			PlayGame();

			///// Write some text /////

			glDisable(GL_DEPTH_TEST);

			glLoadIdentity();
			glTranslatef(0,0,-52);

			glDisable(GL_CULL_FACE);

/*			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_COLOR,GL_ONE);
			glColor4f(1,1,1,1);
			
			glBindTexture(GL_TEXTURE_2D,GameLevel->TextureList[12].texture[0]);
			glBegin(GL_QUADS);
				glTexCoord2i(0,0);
				glVertex3f(-1,1,0);

				glTexCoord2i(0,1);
				glVertex3f(-1,-1,0);

				glTexCoord2i(1,1);
				glVertex3f(1,-1,0);

				glTexCoord2i(1,0);
				glVertex3f(1,1,0);
			glEnd();*/

//			glDisable(GL_BLEND);
			glDisable(GL_TEXTURE_2D);
//			glBlendFunc(GL_SRC_ALPHA,GL_ONE);

			glColor3f(1,1,1);
			glRasterPos2i(-39,28);
			Print("QuickPort Engine");

			if (NumFrames == 100) {
				fps = 100.0f / ((float)(timeGetTime() - fpsTime) * 0.001f);
				fpsTime = timeGetTime();
				NumFrames = 0;
			}

			glColor3f(0,1,0);
			glRasterPos2i(-39,26);
			Print("fps: %3.2f",fps);

			glColor3f(0,0,1);
			glRasterPos2i(-39,24);
			Print("channels: %i",FSOUND_GetChannelsPlaying());

			glColor3f(0,1,0);
			glRasterPos2i(-39,22);
			Print("sector: %i",P->InSector->id);

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glEnable(GL_TEXTURE_2D);

			/////

			SwapBuffers(hDC);

			NumFrames++;
		}
	}

	DeleteFont();
	DisableFMOD();
	KillWindow();
	FreeMemory();

	// temp
	delete mesh;
	//

	return Msg.wParam;
}