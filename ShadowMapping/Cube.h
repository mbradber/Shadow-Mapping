#pragma once
#include"Utility.h"
#include"Vertices.h"

class Cube
{
public:
	Cube(void);
	~Cube(void);

	void init(ID3D10Device* device, float scale);
	void draw();
	void deallocate();

private:
	DWORD numVerts;
	DWORD numFaces;
	PosTexVertex v[8];

	ID3D10Device * pDevice;
	ID3D10Buffer* vb;
	ID3D10Buffer* ib;
};

