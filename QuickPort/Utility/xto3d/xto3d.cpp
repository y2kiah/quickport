/*
	xto3d utility by Jeff Kiah

	This utility was created to convert .X files to .3D files for usage in the Guerilla project.
*/

#include <iostream.h>
#include <fstream.h>
#include <string.h>

////////// Structures //////////

struct Color {
	float r, g, b;
};

struct Vertex {
	float x, y, z;
	float u, v;
	Color c;
};

struct FaceId {
	unsigned int v1, v2, v3, MaterialId;
};

struct Material {
	Color c;
	char TextureName[30];
};

struct Face {
	Vertex v1, v2, v3;
	unsigned int MaterialId;
};

////////// Variables //////////

Vertex *VertexList;
FaceId *FaceIdList;
Material *MaterialList;
Face *FaceList;
int NumFaces = 0;
int NumMaterialFaces = 0;
int NumVertices = 0;
int NumMaterials = 0;
int MaterialCount = 0;
int TextureCount = 0;
int Count;

////////// Functions //////////

int ConvertFile(char *Dest, char *Source)
{
	char Buffer[80];
	cout << "\nConverting File..." << endl;

	ifstream inFile(Source,ios::in);
	if (!inFile) {
		cout << "  Error: " << Source << " Not Found\n";
		return 1;
	}

	ofstream outFile(Dest,ios::out);
	if (!outFile) {
		cout << "  Error: " << "Could Not Create " << Dest << endl;
		inFile.close();
		return 1;
	}

///// Read Everything Into Buffers /////
	while (!inFile.eof()) {
		Buffer[0] = NULL;
		inFile.getline(Buffer, 80);

		if (!strcmp(Buffer,"Mesh {")) {
			cout << "  Found: Mesh" << endl;
			
			inFile >> NumVertices;
			inFile.ignore();
			
			VertexList = new Vertex[NumVertices];

			for (Count = 0; Count < NumVertices; Count++) {
				inFile >> VertexList[Count].x;
				inFile.ignore();
				inFile >> VertexList[Count].y;
				inFile.ignore();
				inFile >> VertexList[Count].z;
				inFile.ignore(2);
			}

			inFile >> NumFaces;
			inFile.ignore();

			FaceIdList = new FaceId[NumFaces];
			int Dummy;

			for (Count = 0; Count < NumFaces; Count++) {
				inFile >> Dummy;
				inFile.ignore();
				inFile >> FaceIdList[Count].v1;
				inFile.ignore();
				inFile >> FaceIdList[Count].v2;
				inFile.ignore();
				inFile >> FaceIdList[Count].v3;
				inFile.ignore(2);
			}

		} else if (!strcmp(Buffer," MeshMaterialList {")) {
			cout << "  Found: MeshMaterialList" << endl;
			
			inFile >> NumMaterials;
			inFile.ignore();
			inFile >> NumMaterialFaces;
			inFile.ignore();

			MaterialList = new Material[NumMaterials];

			for (Count = 0; Count < NumMaterialFaces; Count++) {
				inFile >> FaceIdList[Count].MaterialId;
				inFile.ignore();
			}

			if (NumMaterialFaces < NumFaces) {
				for (Count = NumMaterialFaces; Count < NumFaces; Count++)
					FaceIdList[Count].MaterialId = FaceIdList[NumMaterialFaces-1].MaterialId;
			}

		} else if (!strcmp(Buffer,"  Material {")) {
			cout << "  Found: Material #" << MaterialCount + 1 << endl;
			inFile >> MaterialList[MaterialCount].c.r;
			inFile.ignore();
			inFile >> MaterialList[MaterialCount].c.g;
			inFile.ignore();
			inFile >> MaterialList[MaterialCount].c.b;
			inFile.ignore();

			MaterialCount++;

		} else if (!strcmp(Buffer,"   TextureFilename {")) {
			cout << "  Found: TextureFilename #" << TextureCount + 1 << endl;
			inFile.ignore(5);
			inFile.get(MaterialList[TextureCount].TextureName,29,'"');

			TextureCount++;

		} else if (!strcmp(Buffer," MeshTextureCoords {")) {
			cout << "  Found: MeshTextureCoords" << endl;
			inFile >> NumVertices;
			inFile.ignore();

			for (Count = 0; Count < NumVertices; Count++) {
				inFile >> VertexList[Count].u;
				inFile.ignore();
				inFile >> VertexList[Count].v;
				inFile.ignore(2);
			}
		}
	}

///// Sort The Buffers /////

	FaceList = new Face[NumFaces];

	for (Count = 0; Count < NumFaces; Count++) {
		FaceList[Count].MaterialId = FaceIdList[Count].MaterialId;
		VertexList[FaceIdList[Count].v1].c = MaterialList[FaceList[Count].MaterialId].c;
		VertexList[FaceIdList[Count].v2].c = MaterialList[FaceList[Count].MaterialId].c;
		VertexList[FaceIdList[Count].v3].c = MaterialList[FaceList[Count].MaterialId].c;
		FaceList[Count].v1 = VertexList[FaceIdList[Count].v1];
		FaceList[Count].v2 = VertexList[FaceIdList[Count].v2];
		FaceList[Count].v3 = VertexList[FaceIdList[Count].v3];
	}

///// Create .3D File /////

	outFile.setf(ios::fixed,ios::floatfield);
	outFile.setf(ios::showpoint);
	outFile.precision(6);
	outFile << NumFaces << endl;
	outFile << NumMaterials << endl << endl;

	for (Count = 0; Count < NumMaterials; Count++)
		outFile << MaterialList[Count].TextureName << endl;
	outFile << endl;

///// Print Stuff /////

	for (Count = 0; Count < NumFaces; Count++) {
		outFile << FaceList[Count].v1.x << ' ' << FaceList[Count].v1.y << ' ' << FaceList[Count].v1.z << endl;
		outFile << FaceList[Count].v1.u << ' ' << FaceList[Count].v1.v << endl;
		outFile << FaceList[Count].v1.c.r << ' ' << FaceList[Count].v1.c.g << ' ' << FaceList[Count].v1.c.b << endl;

		outFile << FaceList[Count].v2.x << ' ' << FaceList[Count].v2.y << ' ' << FaceList[Count].v2.z << endl;
		outFile << FaceList[Count].v2.u << ' ' << FaceList[Count].v2.v << endl;
		outFile << FaceList[Count].v2.c.r << ' ' << FaceList[Count].v2.c.g << ' ' << FaceList[Count].v2.c.b << endl;

		outFile << FaceList[Count].v3.x << ' ' << FaceList[Count].v3.y << ' ' << FaceList[Count].v3.z << endl;
		outFile << FaceList[Count].v3.u << ' ' << FaceList[Count].v3.v << endl;
		outFile << FaceList[Count].v3.c.r << ' ' << FaceList[Count].v3.c.g << ' ' << FaceList[Count].v3.c.b << endl;
		
		outFile << FaceList[Count].MaterialId << endl << endl;
	}

///// Free Memory /////

	delete [] VertexList;
	delete [] FaceIdList;
	delete [] MaterialList;
	delete [] FaceList;

	inFile.close();
	outFile.close();

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc == 3) {
		if (!ConvertFile(argv[1],argv[2])) cout << "Conversion Successful!\n";
		else cout << "Conversion Not Complete\n";
	} else {
		cout << "\n---=== .X to .3D File Converter ===---\n";
		cout << "             By Jeff Kiah\n\n";
		cout << "Usage:  xto3d <destination.3d> <source.x>\n\n";
	}

	return 0;
}
