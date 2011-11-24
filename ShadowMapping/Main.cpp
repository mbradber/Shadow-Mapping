#include"DXApp.h"
#include"Utility.h"
#include"Camera.h"
#include"Cube.h"
#include"Plane.h"
#include"Lights.h"
#include"LightEye.h"
#include<sstream>

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
	void setupShadowMap();
	void drawToSM();

	HINSTANCE hInstance;
	
	//Effect variables for geo FX
	D3DXMATRIX worldMatrix, viewMatrix, projMatrix, wvpMatrix;
	ID3D10EffectMatrixVariable* pWVP;
	ID3D10EffectMatrixVariable* pWorldMatrix;
	ID3D10EffectVariable* fxEyePos;
	ID3D10EffectVariable* fxLightSource;

	//Effect variables for light eye FX
	ID3D10EffectMatrixVariable* fxLightEyeWVP;
	ID3D10EffectMatrixVariable* fxLightEyeWorld;
	ID3D10EffectShaderResourceVariable* fxLightEyeDiffuse;

	ID3D10Effect* pGeoFX;
	ID3D10EffectTechnique* pGeoTechnique;
	ID3D10EffectPass* pGeoPass;
	D3D10_PASS_DESC geoPassDesc;

	ID3D10Effect* pLightEyeFX;
	ID3D10EffectTechnique* pLightEyeTechnique;
	ID3D10EffectPass* pLightEyePass;
	D3D10_PASS_DESC lightEyePassDesc;

	//input layouts
	ID3D10InputLayout* pnVertexLayout;
	ID3D10InputLayout* ptVertexLayout;
	ID3D10InputLayout* pcVertexLayout;
	ID3D10InputLayout* smVertexLayout;

	ID3D10ShaderResourceView* boxDiffuseMapRV;

	Camera camera;
	Cube cube;
	Cube cube1;
	Plane plane;
	LightEye lightEye;
	Light lightSource;
	float lightTheta;

	//shadow mapping variables
	ID3D10ShaderResourceView* colorMapRV;
	ID3D10RenderTargetView* colorMap;

	ID3D10ShaderResourceView* depthMapRV;
	ID3D10DepthStencilView* depthMap;

	D3D10_VIEWPORT smViewport;

	D3DXMATRIX lightEyeWVP, lightEyeViewMatrix;

	ID3D10Effect* shadowsFX;
	ID3D10EffectTechnique* shadowsTechnique;
	ID3D10EffectPass* shadowsPass;
	D3D10_PASS_DESC shadowsDesc;
	ID3D10EffectMatrixVariable* shadowsWVP;

	ID3D10EffectMatrixVariable* fxLightWVP;
	ID3D10EffectShaderResourceVariable* fxShadowMap;
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

	//init the shadow map viewport
	ZeroMemory(&smViewport, sizeof(D3D10_VIEWPORT));

	smViewport.TopLeftX = 0;
	smViewport.TopLeftY = 0;
	smViewport.Width = 800;
	smViewport.Height = 600;;
	smViewport.MinDepth = 0;
	smViewport.MaxDepth = 1;
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
	setupShadowMap();
	cube.init(mDevice, 1.0f);
	cube1.init(mDevice, 2.0f);
	plane.init(mDevice, 10.0f);
	lightEye.init(mDevice, 2.0f);
	//camera.init(D3DXVECTOR3(5, 10, -8), D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(0, 1, 0));
	camera.init(D3DXVECTOR3(0, 0, -10), D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(0, 1, 0));

	this->handleResize();
	gameTimer.start();
}

void ShadowMapping::setupShadowMap()
{
	ID3D10Texture2D* dm = NULL;
	D3D10_TEXTURE2D_DESC desc;
	desc.Width = 800;
	desc.Height = 600;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D10_USAGE_DEFAULT;
	desc.BindFlags = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	mDevice->CreateTexture2D(&desc, 0, &dm);

	D3D10_DEPTH_STENCIL_VIEW_DESC dsDesc;
	dsDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	dsDesc.Texture2D.MipSlice = 0;
	
	mDevice->CreateDepthStencilView(dm, &dsDesc, &depthMap);

	D3D10_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MipLevels = desc.MipLevels;
	srDesc.Texture2D.MostDetailedMip = 0;

	mDevice->CreateShaderResourceView(dm, &srDesc, &depthMapRV);

	dm->Release();
	dm = NULL;
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
		&pGeoFX, &shaderErrors, 0);

	if(shaderErrors)
		MessageBoxA(0, (char*)shaderErrors->GetBufferPointer(), 0, 0);

	D3DX10CreateEffectFromFile(L"LightEye.fx", 0, 0, "fx_4_0", 0, 0, mDevice, 0, 0, 
		&pLightEyeFX, &shaderErrors, 0);

	if(shaderErrors)
		MessageBoxA(0, (char*)shaderErrors->GetBufferPointer(), 0, 0);

	D3DX10CreateEffectFromFile(L"Depth.fx", 0, 0, "fx_4_0", 0, 0, mDevice, 0, 0, 
		&shadowsFX, &shaderErrors, 0);

	if(shaderErrors)
		MessageBoxA(0, (char*)shaderErrors->GetBufferPointer(), 0, 0);

	pGeoTechnique = pGeoFX->GetTechniqueByIndex(0);
	pGeoPass = pGeoTechnique->GetPassByIndex(0);
	pGeoPass->GetDesc(&geoPassDesc);

	pLightEyeTechnique = pLightEyeFX->GetTechniqueByIndex(0);
	pLightEyePass = pLightEyeTechnique->GetPassByIndex(0);
	pLightEyePass->GetDesc(&lightEyePassDesc);

	//set up a handle to the geometry FX file vars
	pWVP = pGeoFX->GetVariableByName("wvp")->AsMatrix();
	pWorldMatrix = pGeoFX->GetVariableByName("worldMatrix")->AsMatrix();
	fxEyePos = pGeoFX->GetVariableByName("eyePos");
	fxLightSource = pGeoFX->GetVariableByName("lightSource");
	fxLightWVP = pGeoFX->GetVariableByName("lwvp")->AsMatrix();
	fxShadowMap = pGeoFX->GetVariableByName("shadowMap")->AsShaderResource();

	//set up a handle to the light eye FX file vars
	fxLightEyeWVP = pLightEyeFX->GetVariableByName("wvpMatrix")->AsMatrix();
	fxLightEyeWorld = pLightEyeFX->GetVariableByName("worldMatrix")->AsMatrix();
	fxLightEyeDiffuse = pLightEyeFX->GetVariableByName("diffuseMap")->AsShaderResource();

	shadowsTechnique = shadowsFX->GetTechniqueByIndex(0);
	shadowsPass = shadowsTechnique->GetPassByIndex(0);
	shadowsPass->GetDesc(&shadowsDesc);
	shadowsWVP = shadowsFX->GetVariableByName("wvp")->AsMatrix();

	//create a resource view from the box image
	HRESULT hr = D3DX10CreateShaderResourceViewFromFile(mDevice, L"WoodCrate02.dds", 0, 0, &boxDiffuseMapRV, 0);
	if(FAILED(hr))
		MessageBoxA(0, "Failed to load texture.", 0, 0);
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

	D3D10_INPUT_ELEMENT_DESC posTexLayout[] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	D3D10_INPUT_ELEMENT_DESC posNormLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"DIFFUSE",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"SPECULAR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

    mDevice->CreateInputLayout(posNormLayout, 4, geoPassDesc.pIAInputSignature,
		geoPassDesc.IAInputSignatureSize, &pnVertexLayout);

	mDevice->CreateInputLayout(posColLayout, 2, geoPassDesc.pIAInputSignature,
		geoPassDesc.IAInputSignatureSize, &pcVertexLayout);

    mDevice->CreateInputLayout(posTexLayout, 2, lightEyePassDesc.pIAInputSignature,
		lightEyePassDesc.IAInputSignatureSize, &ptVertexLayout);

	mDevice->CreateInputLayout(posNormLayout, 4, shadowsDesc.pIAInputSignature,
		shadowsDesc.IAInputSignatureSize, &smVertexLayout);
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
	if(lightTheta > 2*PI)
		lightTheta = 0;
	//normalize the result
	D3DXVec4Normalize(&lightSource.direction, &lightSource.direction);

//BUILD THE LIGHT SOURCE VIEW MATRIX

	//first set the light height and radius
	float lightHeight = 5;
	float lightRadius = 10;
	
	//calculat the light source position and normalize the result
	D3DXVECTOR3 lightUnit;
	D3DXVECTOR3 lightPos(lightRadius * xCoord, lightHeight, lightRadius * zCoord);
	D3DXVec3Normalize(&lightUnit, &lightPos);

	//generate the vector to rotate about (the side vector of the light source)
	D3DXVECTOR3 upVec(0, 1, 0);
	D3DXVECTOR3 outVec(xCoord, 0, zCoord);
	D3DXVECTOR3 sideVec;
	D3DXVec3Cross(&sideVec, &upVec, &outVec);

	//generate the matrix transform (the matrix to rotate about the side vector)
	D3DXMATRIX rotationMat;
	D3DXMatrixRotationAxis(&rotationMat, &sideVec, 3 * PI / 2);

	//calculate the light source up vector and normalize the result
	D3DXVECTOR4 lightUp;
	D3DXVec3Transform(&lightUp, &lightPos, &rotationMat);
	//lightUp *= -1;
	D3DXVec4Normalize(&lightUp, &lightUp);

	lightPos.x *= -1;
	lightPos.z *= -1;
	//build the view matrix
	D3DXMatrixLookAtLH(&lightEyeViewMatrix, &lightPos, &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(lightUp));
}

void ShadowMapping::draw()
{
	DXApp::draw();

	//start drawing to shadow map
	ID3D10RenderTargetView* renderTargets[1] = {NULL};
	mDevice->OMSetRenderTargets(1, renderTargets, depthMap);
	mDevice->OMSetDepthStencilState(0, 0);
	mDevice->RSSetViewports(1, &smViewport);
	mDevice->ClearDepthStencilView(depthMap, D3D10_CLEAR_DEPTH, 1, 0);

	drawToSM();

	//reset the render target
	resetTargets();
	mDevice->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	mDevice->OMSetBlendState(0, blendFactors, 0xffffffff);

	//set up the input assembler
	mDevice->IASetInputLayout(pnVertexLayout);
	mDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//pass the light world-view-projection matrix and the shadow map to the shader
	D3DXMatrixIdentity(&worldMatrix);
	D3DXMATRIX lightEyeWVP = worldMatrix * lightEyeViewMatrix * projMatrix;
	fxLightWVP->SetMatrix((float*)&lightEyeWVP);
	fxShadowMap->SetResource(depthMapRV);

	//calculate the world-view-projection matrix
	D3DXMatrixIdentity(&worldMatrix);
	wvpMatrix = worldMatrix * viewMatrix * projMatrix;
	fxEyePos->SetRawValue(&camera.GetCameraPosition(), 0, sizeof(D3DXVECTOR3));
	fxLightSource->SetRawValue(&lightSource, 0, sizeof(Light));
	pWorldMatrix->SetMatrix((float*)&worldMatrix);
	pWVP->SetMatrix((float*)&wvpMatrix);

	//draw the scene's plane
	pGeoPass->Apply(0);
	plane.draw();

	//translate the cube up, so that it is on top of the plane
	D3DXMatrixTranslation(&worldMatrix, -3, 1.0f, 0);

	lightEyeWVP = worldMatrix * lightEyeViewMatrix * projMatrix;
	fxLightWVP->SetMatrix((float*)&lightEyeWVP);

	wvpMatrix = worldMatrix * viewMatrix * projMatrix;
	pWorldMatrix->SetMatrix((float*)&worldMatrix);
	pWVP->SetMatrix((float*)&wvpMatrix);

	//draw the cube
	pGeoPass->Apply(0);
	cube.draw();

	//set up the second cube
	D3DXMatrixTranslation(&worldMatrix, 3, 1.0f, 0);

	lightEyeWVP = worldMatrix * lightEyeViewMatrix * projMatrix;
	fxLightWVP->SetMatrix((float*)&lightEyeWVP);

	wvpMatrix = worldMatrix * viewMatrix * projMatrix;
	pWorldMatrix->SetMatrix((float*)&worldMatrix);
	pWVP->SetMatrix((float*)&wvpMatrix);

	//draw the second cube
	pGeoPass->Apply(0);
	cube1.draw();

	//switch the input layout so we can use textures
	//mDevice->IASetInputLayout(ptVertexLayout);

	//D3DXMatrixTranslation(&worldMatrix, 5, 2, 0);
	//wvpMatrix = worldMatrix * viewMatrix * projMatrix;

	//fxLightEyeWVP->SetMatrix((float*)wvpMatrix);
	//fxLightEyeWorld->SetMatrix((float*)worldMatrix);
	//fxLightEyeDiffuse->SetResource(depthMapRV);

	////draw the light's perspective
	//pLightEyePass->Apply(0);
	//lightEye.draw();

	//present the back buffer
	mSwapChain->Present(0, 0);
}

void ShadowMapping::drawToSM()
{
	//set up the input assembler
	mDevice->IASetInputLayout(smVertexLayout);
	mDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//calculate the world-view-projection matrix
	D3DXMatrixIdentity(&worldMatrix);
	wvpMatrix = worldMatrix * lightEyeViewMatrix * projMatrix;
	shadowsWVP->SetMatrix((float*)&wvpMatrix);

	//draw the scene's plane
	shadowsPass->Apply(0);
	plane.draw();

	//translate the cube up, so that it is on top of the plane
	D3DXMatrixTranslation(&worldMatrix, -3, 1.0f, 0);
	wvpMatrix = worldMatrix * lightEyeViewMatrix * projMatrix;
	shadowsWVP->SetMatrix((float*)&wvpMatrix);

	//draw the cube
	shadowsPass->Apply(0);
	cube.draw();

	D3DXMatrixTranslation(&worldMatrix, 3, 1.0f, 0);
	wvpMatrix = worldMatrix * lightEyeViewMatrix * projMatrix;
	shadowsWVP->SetMatrix((float*)&wvpMatrix);

	//draw the second cube
	shadowsPass->Apply(0);
	cube1.draw();
}


int WINAPI WinMain(HINSTANCE hi, HINSTANCE hpi, PSTR pcl, int sc)
{
	ShadowMapping sm(hi);
	sm.initWindow(hi, sc);
	sm.init();
	return sm.run();
}