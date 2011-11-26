#include "Tetrahedron.h"

void Tetrahedron::deallocate()
{
}

void Tetrahedron::init(ID3D10Device* device, float scale)
{
	pDevice = device;

	numVerts = 12;
	numFaces = 4;

	float specularPower = 0;
	D3DXCOLOR commonSpec(0.25, 0.25, 0.25, specularPower);

	PosNormVertex vertices [] = 
	{

		//front face
		PosNormVertex(D3DXVECTOR3(-1, -1, -1), D3DXVECTOR3(0, 0, -1), BLUE, commonSpec),
		PosNormVertex(D3DXVECTOR3(1, -1, -1), D3DXVECTOR3(0, 0, -1), BLUE, commonSpec),
		PosNormVertex(D3DXVECTOR3(0, 1, 0), D3DXVECTOR3(0, 0, -1), BLUE, commonSpec),

		//second back face
		PosNormVertex(D3DXVECTOR3(1, -1, -1), D3DXVECTOR3(1, 0, 1), BLUE, commonSpec),
		PosNormVertex(D3DXVECTOR3(0, -1, 1), D3DXVECTOR3(1, 0, 1), BLUE, commonSpec),
		PosNormVertex(D3DXVECTOR3(0, 1, 0), D3DXVECTOR3(1, 0, 1), BLUE, commonSpec),

		//first back face
		PosNormVertex(D3DXVECTOR3(0, -1, 1), D3DXVECTOR3(-1, 0, 1), BLUE, commonSpec),
		PosNormVertex(D3DXVECTOR3(-1, -1, -1), D3DXVECTOR3(-1, 0, 1), BLUE, commonSpec),
		PosNormVertex(D3DXVECTOR3(0, 1, 0), D3DXVECTOR3(-1, 0, 1), BLUE, commonSpec),

		//bottom face
		PosNormVertex(D3DXVECTOR3(-1, -1, -1), D3DXVECTOR3(0, -1, 0), BLUE, commonSpec),
        PosNormVertex(D3DXVECTOR3(1, -1, -1), D3DXVECTOR3(0, -1, 0), BLUE, commonSpec),
        PosNormVertex(D3DXVECTOR3(0, -1, 1), D3DXVECTOR3(0, -1, 0), BLUE, commonSpec),
	};

	//scale the geometry
	for(int i = 0; i < numVerts; ++i)
		vertices[i].pos *= scale;

	DWORD indices[] = 
    {
		//face 1
        2, 1, 0,
        5, 4, 3,
		8, 7, 6,
		9, 10, 11
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

void Tetrahedron::draw()
{
	UINT stride = sizeof(PosNormVertex);
    UINT offset = 0;
    pDevice->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	pDevice->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
	pDevice->DrawIndexed(numFaces*3, 0, 0);
}


