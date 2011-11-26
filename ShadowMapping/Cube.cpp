#include "Cube.h"

void Cube::deallocate()
{
	pDevice = NULL;
}

void Cube::init(ID3D10Device* device, float scale)
{
	pDevice = device;
 
	numVerts = 24;
	numFaces    = 36;

	float specularPower = 150.0f;

	PosNormVertex vertices [] = 
	{
		//face 1
		PosNormVertex(D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), RED, D3DXCOLOR(1, 1, 1, specularPower)),
        PosNormVertex(D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), RED, D3DXCOLOR(1, 1, 1, specularPower)),
        PosNormVertex(D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), RED, D3DXCOLOR(1, 1, 1, specularPower)),
        PosNormVertex(D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), RED, D3DXCOLOR(1, 1, 1, specularPower)),
		//face 2
        PosNormVertex(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), RED, D3DXCOLOR(1, 1, 1, specularPower)),
        PosNormVertex(D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), RED, D3DXCOLOR(1, 1, 1, specularPower)),
        PosNormVertex(D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), RED, D3DXCOLOR(1, 1, 1, specularPower)),
        PosNormVertex(D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), RED, D3DXCOLOR(1, 1, 1, specularPower)),
		//face 3
        PosNormVertex(D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), RED, D3DXCOLOR(1, 1, 1, specularPower)),
        PosNormVertex(D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), RED, D3DXCOLOR(1, 1, 1, specularPower)),
        PosNormVertex(D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), RED, D3DXCOLOR(1, 1, 1, specularPower)),
        PosNormVertex(D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), RED, D3DXCOLOR(1, 1, 1, specularPower)),
		//face 4
        PosNormVertex(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), RED, D3DXCOLOR(1, 1, 1, specularPower)),
        PosNormVertex(D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), RED, D3DXCOLOR(1, 1, 1, specularPower)),
        PosNormVertex(D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), RED, D3DXCOLOR(1, 1, 1, specularPower)),
        PosNormVertex(D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), RED, D3DXCOLOR(1, 1, 1, specularPower)),
		//face 5
        PosNormVertex(D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), RED, D3DXCOLOR(1, 1, 1, specularPower)),
        PosNormVertex(D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), RED, D3DXCOLOR(1, 1, 1, specularPower)),
        PosNormVertex(D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), RED, D3DXCOLOR(1, 1, 1, specularPower)),
        PosNormVertex(D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), RED, D3DXCOLOR(1, 1, 1, specularPower)),
		//face 6
        PosNormVertex(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), RED, D3DXCOLOR(1, 1, 1, specularPower)),
        PosNormVertex(D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), RED, D3DXCOLOR(1, 1, 1, specularPower)),
        PosNormVertex(D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), RED, D3DXCOLOR(1, 1, 1, specularPower)),
        PosNormVertex(D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), RED, D3DXCOLOR(1, 1, 1, specularPower)),
	};

	//scale the geometry
	for(int i = 0; i < numVerts; ++i)
		vertices[i].pos *= scale;

	DWORD indices[] = 
    {
		//face 1
        0, 1, 2,
        2, 1, 3,
		//face 2
        4, 5, 6,
        6, 5, 7,
		//face 3
        8, 9, 10,
        10, 9, 11,
		//face 4
        12, 13, 14,
        14, 13, 15,
		//face 5
        16, 17, 18,
        18, 17, 19,
		//face 6
        20, 21, 22,
        22, 21, 23,
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

void Cube::draw()
{
	UINT stride = sizeof(PosNormVertex);
    UINT offset = 0;
    pDevice->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	pDevice->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
	pDevice->DrawIndexed(numFaces*3, 0, 0);
}
