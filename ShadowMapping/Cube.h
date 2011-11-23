#pragma once
#include"Geometry.h"

class Cube: public Geometry
{
public:

	void init(ID3D10Device* device, float scale);
	void draw();
	void deallocate();

};

