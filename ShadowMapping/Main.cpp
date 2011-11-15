#include"DXApp.h"
#include"Utility.h"
#include"Camera.h"
#include"Cube.h"
#include"Lights.h"

class ShadowMapping: public DXApp
{
public:
	ShadowMapping(HINSTANCE hi);
	~ShadowMapping();
	void init();

private:
	void handleResize();
	void updateScene(float delta);
	void draw();
	void initFX();
	void buildLayout();

	HINSTANCE hInstance;
	
	//Effect variables
	D3DXMATRIX worldMatrix, viewMatrix, projMatrix, wvpMatrix;
	ID3D10EffectMatrixVariable* pWVP;
	ID3D10EffectMatrixVariable* pWorldMatrix;
	ID3D10EffectVariable* fxEyePos;
	ID3D10EffectVariable* fxLightSource;

	ID3D10Effect* pEffect;
	ID3D10EffectTechnique* pTechnique;
	ID3D10EffectPass* pPass;
	D3D10_PASS_DESC passDesc;

	ID3D10InputLayout* pcVertexLayout;

	Camera camera;
	Cube cube;
	Light lightSource;
	float lightTheta;
};

ShadowMapping::ShadowMapping(HINSTANCE hi):
hInstance(hi)
{
	//clear the memory for the light source
	ZeroMemory(&lightSource, sizeof(Light));

	//set the light source attributes
	lightSource.direction = D3DXVECTOR4(0, 0, 1, NULL);
	lightSource.ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
	lightSource.diffuse = WHITE;
	lightSource.specular = WHITE;

	lightTheta = 0;
}

ShadowMapping::~ShadowMapping()
{

}

void ShadowMapping::init()
{
	DXApp::initD3D();

	initFX();
	buildLayout();
	initDInput(hInstance, mainWindow);
	cube.init(mDevice, 1.0f);
	camera.init(D3DXVECTOR3(0, 0, -10), D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(0, 1, 0));

	this->handleResize();
	gameTimer.start();
}

void ShadowMapping::handleResize()
{
	DXApp::handleResize();
	D3DXMatrixPerspectiveFovLH(&projMatrix, 0.25f*PI, (float)CLIENT_WIDTH/CLIENT_HEIGHT, 1.0f, 1000.0f);
}

void ShadowMapping::initFX()
{
	ID3D10Blob* shaderErrors = 0;
	D3DX10CreateEffectFromFile(L"Geometry.fx", 0, 0, "fx_4_0", 0, 0, mDevice, 0, 0, 
		&pEffect, &shaderErrors, 0);

	if(shaderErrors)
		MessageBoxA(0, (char*)shaderErrors->GetBufferPointer(), 0, 0);

	pTechnique = pEffect->GetTechniqueByIndex(0);
	pPass = pTechnique->GetPassByIndex(0);
	pPass->GetDesc(&passDesc);

	pWVP = pEffect->GetVariableByName("wvp")->AsMatrix();
	pWorldMatrix = pEffect->GetVariableByName("worldMatrix")->AsMatrix();
	fxEyePos = pEffect->GetVariableByName("eyePos");
	fxLightSource = pEffect->GetVariableByName("lightSource");
}

void ShadowMapping::buildLayout()
{
    D3D10_INPUT_ELEMENT_DESC posColLayout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT,
            D3D10_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT,
            D3D10_INPUT_PER_VERTEX_DATA, 0}
    };

	D3D10_INPUT_ELEMENT_DESC posNormLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"DIFFUSE",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"SPECULAR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

    mDevice->CreateInputLayout(posNormLayout, 4, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,&pcVertexLayout);
}

void ShadowMapping::updateScene(float delta)
{
	DXApp::updateScene(delta);

	//detect user input
	detectInput();
	//update the camera based on the input
	camera.Update(keystate, mouseState, delta);
	//retrieve the view matrix from the camera object
	viewMatrix = camera.GetCameraView();

	//rotate the direction of the light
	float xCoord = cosf(lightTheta);
	float zCoord = sinf(lightTheta);
	lightSource.direction = D3DXVECTOR4(xCoord, -1, zCoord, NULL);
	lightTheta += delta;
	//normalize the result
	D3DXVec4Normalize(&lightSource.direction, &lightSource.direction);
}

void ShadowMapping::draw()
{
	DXApp::draw();

	//set up the input assembler
	mDevice->IASetInputLayout(pcVertexLayout);
	mDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//calculate the world-view-projection matrix
	D3DXMatrixIdentity(&worldMatrix);
	wvpMatrix = worldMatrix * viewMatrix * projMatrix;

	//set the shader variables
	fxEyePos->SetRawValue(&camera.GetCameraPosition(), 0, sizeof(D3DXVECTOR3));
	fxLightSource->SetRawValue(&lightSource, 0, sizeof(Light));
	pWorldMatrix->SetMatrix((float*)&worldMatrix);
	pWVP->SetMatrix((float*)&wvpMatrix);

	//apply the shader pass and draw the the cube
	pPass->Apply(0);
	cube.draw();

	//present the back buffer
	mSwapChain->Present(0, 0);
}


int WINAPI WinMain(HINSTANCE hi, HINSTANCE hpi, PSTR pcl, int sc)
{
	ShadowMapping sm(hi);
	sm.initWindow(hi, sc);
	sm.init();
	return sm.run();
}