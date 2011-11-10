#include"DXApp.h"
#include"Utility.h"
#include"Camera.h"
#include"Cube.h"

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
	
	D3DXMATRIX worldMatrix, viewMatrix, projMatrix, wvpMatrix;
	ID3D10EffectMatrixVariable * pWVP;

	ID3D10InputLayout* pcVertexLayout;

	ID3D10Effect* pEffect;
	ID3D10EffectTechnique* pTechnique;
	ID3D10EffectPass* pPass;
	D3D10_PASS_DESC passDesc;

	Camera camera;
	Cube cube;
};

ShadowMapping::ShadowMapping(HINSTANCE hi):
hInstance(hi)
{

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
}

void ShadowMapping::buildLayout()
{
    D3D10_INPUT_ELEMENT_DESC Layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT,
            D3D10_INPUT_PER_VERTEX_DATA, 0},

        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT,
            D3D10_INPUT_PER_VERTEX_DATA, 0}
    };

    mDevice->CreateInputLayout(Layout, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,&pcVertexLayout);
}

void ShadowMapping::updateScene(float delta)
{
	DXApp::updateScene(delta);

	detectInput();
	camera.Update(keystate, mouseState, delta);
	viewMatrix = camera.GetCameraView();
}

void ShadowMapping::draw()
{
	DXApp::draw();

	mDevice->IASetInputLayout(pcVertexLayout);
	mDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3DXMatrixIdentity(&worldMatrix);
	wvpMatrix = worldMatrix * viewMatrix * projMatrix;

	pWVP->SetMatrix((float*)&wvpMatrix);

	pPass->Apply(0);
	cube.draw();

	mSwapChain->Present(0, 0);
}


int WINAPI WinMain(HINSTANCE hi, HINSTANCE hpi, PSTR pcl, int sc)
{
	ShadowMapping sm(hi);
	sm.initWindow(hi, sc);
	sm.init();
	return sm.run();
}