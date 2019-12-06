/* ----==== TEXTURE PACK UTILITY ====---- */

#include <iostream.h>
#include <fstream.h>
#include <string.h>

///// CONSTANTS /////

///// STRUCTURES /////

class Filename {
	public:
		int Num;
		char *Name;
		Filename *Next;

		Filename(const int _Num,const char *_Name);
		~Filename() { delete [] Name; }
};

///// VARIABLES /////

Filename *NameList, *LastPtr;

///// FUNCTIONS /////

Filename::Filename(const int _Num,const char *_Name)
{
	Num = _Num;
	Name = new char[strlen(_Name)];
	strcpy(Name,_Name);
	Next = NULL;
}

void Push(const int _Num,const char *_Name)
{
	if (NameList == NULL) {
		NameList = new Filename(_Num,_Name);
		LastPtr = NameList;
	} else {
		LastPtr->Next = new Filename(_Num,_Name);
		LastPtr = LastPtr->Next;
	}
}

void Pop(const int _Num)
{
	Filename *Current = NameList, *Prev = NULL, *Temp = NULL;
	
}

void Do_Program(void)
{
	while (1) {
	}

}

void Free_Memory(void)
{
	Filename *Current = NameList, *Temp = NULL;
	while (Current != NULL) {
		Temp = Current;
		Current = Current->Next;
		delete Temp;
	}
}

int main(void)
{
	Free_Memory();
	return 0;
}