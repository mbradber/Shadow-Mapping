#pragma once
#include"Utility.h"

struct Light
{
	D3DXVECTOR4 position;
	D3DXVECTOR4 direction;
	D3DXCOLOR ambient;
	D3DXCOLOR diffuse;
	D3DXCOLOR specular;
	float range;
};