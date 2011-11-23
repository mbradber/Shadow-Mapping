#include"Plane.h"

void Plane::deallocate()
{
	pDevice = NULL;
}

void Plane::init(ID3D10Device* device, float scale)
{
	pDevice = device;
 
	numVerts = 4;
	numFaces = 2;

	float specularPower = 150;

	PosNormVertex vertices [] = 
	{
		PosNormVertex(D3DXVECTOR3(-1.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), GREEN, D3DXCOLOR(1, 1, 1, specularPower)),
		PosNormVertex(D3DXVECTOR3(1.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), GREEN, D3DXCOLOR(1, 1, 1, specularPower)),
		PosNormVertex(D3DXVECTOR3(-1.0f, 0.0f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), GREEN, D3DXCOLOR(1, 1, 1, specularPower)),
		PosNormVertex(D3DXVECTOR3(1.0f, 0.0f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), GREEN, D3DXCOLOR(1, 1, 1, specularPower)),
	};

	//scale the geometry
	for(int i = 0; i < numVerts; ++i)
		vertices[i].pos *= scale;

	DWORD indices[] = 
    {
		//top face
		3, 1, 0,
		0, 2, 3,
    };

	D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(PosNormVertex) * numVerts;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    pDevice->CreateBuffer(&vbd, &vinitData, &vb);

	D3D10_BUFFER_DESC ibd;
    ibd.Usage = D3D10_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(DWORD) * numFaces * 3;
    ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    pDevice->CreateBuffer(&ibd, &iinitData, &ib);
}

void Plane::draw()
{
	UINT stride = sizeof(PosNormVertex);
    UINT offset = 0;
    pDevice->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	pDevice->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
	pDevice->DrawIndexed(numFaces*3, 0, 0);
}
