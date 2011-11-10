#include "Cube.h"

Cube::Cube(void):
numVerts(0), numFaces(0), pDevice(0), vb(0), ib(0)
{
}

Cube::~Cube(void)
{

}

void Cube::deallocate()
{
	pDevice = NULL;
}

void Cube::init(ID3D10Device* device, float scale)
{
	/*pDevice = device;
	numVerts = 24;
	numFaces = 12;

	//fill in the front face vertex data.
 	v[0] = PosTexVertex(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR2(0, 1));
	v[1] = PosTexVertex(D3DXVECTOR3(-1.0f,  1.0f, -1.0f), D3DXVECTOR2(0, 0));
	v[2] = PosTexVertex(D3DXVECTOR3(1.0f,  1.0f, -1.0f), D3DXVECTOR2(1, 0));
	v[3] = PosTexVertex(D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR2(1, 1));

	//fill in the back face PosTexVertex data.
	v[4] = PosTexVertex(D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR2(1, 1));
	v[5] = PosTexVertex(D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR2(0, 1));
	v[6] = PosTexVertex(D3DXVECTOR3(1.0f,  1.0f, 1.0f), D3DXVECTOR2(0, 0));
	v[7] = PosTexVertex(D3DXVECTOR3(-1.0f,  1.0f, 1.0f), D3DXVECTOR2(1, 0));

	//fill in the top face PosTexVertex data.
	v[8]  = PosTexVertex(D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR2(0, 1));
	v[9]  = PosTexVertex(D3DXVECTOR3(-1.0f, 1.0f,  1.0f), D3DXVECTOR2(0, 0));
	v[10] = PosTexVertex(D3DXVECTOR3(1.0f, 1.0f,  1.0f), D3DXVECTOR2(1, 0));
	v[11] = PosTexVertex(D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR2(1, 1));

	//fill in the bottom face PosTexVertex data.
	v[12] = PosTexVertex(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR2(1, 1));
	v[13] = PosTexVertex(D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR2(0, 1));
	v[14] = PosTexVertex(D3DXVECTOR3(1.0f, -1.0f,  1.0f), D3DXVECTOR2(0, 0));
	v[15] = PosTexVertex(D3DXVECTOR3(-1.0f, -1.0f,  1.0f), D3DXVECTOR2(1, 0));

	//fill in the left face PosTexVertex data.
	v[16] = PosTexVertex(D3DXVECTOR3(-1.0f, -1.0f,  1.0f), D3DXVECTOR2(0, 1));
	v[17] = PosTexVertex(D3DXVECTOR3(-1.0f,  1.0f,  1.0f), D3DXVECTOR2(0, 0));
	v[18] = PosTexVertex(D3DXVECTOR3(-1.0f,  1.0f, -1.0f), D3DXVECTOR2(1, 0));
	v[19] = PosTexVertex(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR2(1, 1));

	//fill in the right face PosTexVertex data.
	v[20] = PosTexVertex(D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR2(0, 1));
	v[21] = PosTexVertex(D3DXVECTOR3(1.0f,  1.0f, -1.0f), D3DXVECTOR2(0, 0));
	v[22] = PosTexVertex(D3DXVECTOR3(1.0f,  1.0f,  1.0f), D3DXVECTOR2(1, 0));
	v[23] = PosTexVertex(D3DXVECTOR3(1.0f, -1.0f,  1.0f), D3DXVECTOR2(1, 1));


	// Create the index buffer
	DWORD i[36];

	//fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	//fill in the back face index data
	i[6] = 4; i[7]  = 5; i[8]  = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	//fill in the top face index data
	i[12] = 8; i[13] =  9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	//fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	//fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	//fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;*/

	pDevice = device;
 
	numVerts = 8;
	numFaces = 12;

    PosColVertex vertices[] =
    {
	    PosColVertex(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), WHITE),
		PosColVertex(D3DXVECTOR3(-1.0f, +1.0f, -1.0f), BLACK),
		PosColVertex(D3DXVECTOR3(+1.0f, +1.0f, -1.0f), RED),
		PosColVertex(D3DXVECTOR3(+1.0f, -1.0f, -1.0f), GREEN),
		PosColVertex(D3DXVECTOR3(-1.0f, -1.0f, +1.0f), BLUE),
		PosColVertex(D3DXVECTOR3(-1.0f, +1.0f, +1.0f), YELLOW),
		PosColVertex(D3DXVECTOR3(+1.0f, +1.0f, +1.0f), CYAN),
		PosColVertex(D3DXVECTOR3(+1.0f, -1.0f, +1.0f), MAGENTA),
    };

	for(DWORD i = 0; i < numVerts; ++i)
		vertices[i].pos *= scale;

	DWORD indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3, 
		4, 3, 7
	};

	D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(PosColVertex) * numVerts;
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
	UINT stride = sizeof(PosColVertex);
    UINT offset = 0;
    pDevice->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	pDevice->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
	pDevice->DrawIndexed(numFaces*3, 0, 0);
}
