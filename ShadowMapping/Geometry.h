#pragma once
#include"Utility.h"
#include"Vertices.h"

class Geometry
{
public:
	Geometry(void);

	//pure virtual functions for geometry objects
	virtual void init(ID3D10Device* device, float scale) = 0;
	virtual void draw() = 0;
	virtual void deallocate() = 0;

protected:
	DWORD numVerts;
	DWORD numFaces;

	ID3D10Device * pDevice;
	ID3D10Buffer* vb;
	ID3D10Buffer* ib;
};

