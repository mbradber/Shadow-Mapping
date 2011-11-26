#include"DXApp.h"
#include"Utility.h"
#include"Camera.h"
#include"Cube.h"
#include"Plane.h"
#include"Tetrahedron.h"
#include"Lights.h"
#include"LightEye.h"
#include"Effect.h"
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
	FX geoFX;
	FX lightEyeFX;
	FX shadowsFX;

	//The scene objects
	Cube cube1;
	Cube cube2;
	Plane plane;
	Tetrahedron tet1;

	//Matrices...
	D3DXMATRIX worldMatrix, viewMatrix, projMatrix, wvpMatrix, 
		lightEyeWVP, lightEyeViewMatrix, lightProjMatrix;

	//Handles to the Geometry.fx vars
	ID3D10EffectMatrixVariable* pWVP;
	ID3D10EffectMatrixVariable* pWorldMatrix;
	ID3D10EffectMatrixVariable* fxLightWVP;
	ID3D10EffectVariable* fxEyePos;
	ID3D10EffectVariable* fxLightSource;
	ID3D10EffectShaderResourceVariable* fxShadowMap;
	
	//Handles to the LightEye.fx vars
	ID3D10EffectMatrixVariable* fxLightEyeWVP;
	ID3D10EffectMatrixVariable* fxLightEyeWorld;
	ID3D10EffectShaderResourceVariable* fxLightEyeDiffuse;

	//Handles to the Depth.fx vars
	ID3D10EffectMatrixVariable* shadowsWVP;

	//Input layouts
	ID3D10InputLayout* pnVertexLayout;
	ID3D10InputLayout* ptVertexLayout;
	ID3D10InputLayout* pcVertexLayout;
	ID3D10InputLayout* smVertexLayout;

	//Usable textures
	ID3D10ShaderResourceView* boxDiffuseMapRV;
	ID3D10ShaderResourceView* grassDiffuseMapRV;

	//The camera
	Camera camera;

	//Light source variables
	LightEye lightEye;
	Light lightSource;
	float lightTheta;

	//shadow mapping variables
	ID3D10ShaderResourceView* depthMapRV;
	ID3D10DepthStencilView* depthMap;
	D3D10_VIEWPORT smViewport;
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
	cube1.init(mDevice, 0.5f);
	cube2.init(mDevice, 0.8f);
	plane.init(mDevice, 8.0f);
	tet1.init(mDevice, 1);
	lightEye.init(mDevice, 1);
	//camera.init(D3DXVECTOR3(5, 10, -8), D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(0, 1, 0));
	camera.init(D3DXVECTOR3(0, 3, -6), D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(0, 1, 0));

	this->handleResize();
	gameTimer.start();
}

void ShadowMapping::setupShadowMap()
{
	//fill out a texture description
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

	//create a texture to serve as our new depth map
	mDevice->CreateTexture2D(&desc, 0, &dm);

	//fill out a depth-stencil view description
	D3D10_DEPTH_STENCIL_VIEW_DESC dsDesc;
	dsDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	dsDesc.Texture2D.MipSlice = 0;
	
	//create a depth-stencil view to the depth map texture
	mDevice->CreateDepthStencilView(dm, &dsDesc, &depthMap);

	//fill out a shader resource view so that we can bind the depth map
	D3D10_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MipLevels = desc.MipLevels;
	srDesc.Texture2D.MostDetailedMip = 0;

	//create the shader resource to bind to the pipeline
	mDevice->CreateShaderResourceView(dm, &srDesc, &depthMapRV);

	dm->Release();
	dm = NULL;
}

void ShadowMapping::handleResize()
{
	DXApp::handleResize();
	D3DXMatrixPerspectiveFovLH(&projMatrix, 0.25 * PI, (float)CLIENT_WIDTH/CLIENT_HEIGHT, 1.0f, 1000.0f);
	D3DXMatrixPerspectiveFovLH(&lightProjMatrix, PI / 4, (float)CLIENT_WIDTH/CLIENT_HEIGHT, 1.0f, 1000.0f);
}

void ShadowMapping::initFX()
{
	//initialize all of the effect files
	geoFX.init(mDevice, L"Geometry.fx");
	lightEyeFX.init(mDevice, L"LightEye.fx");
	shadowsFX.init(mDevice, L"Depth.fx");

	//set up a handle to the geometry FX file vars
	pWVP = geoFX.getVariable("wvp")->AsMatrix();
	pWorldMatrix = geoFX.getVariable("worldMatrix")->AsMatrix();
	fxEyePos = geoFX.getVariable("eyePos");
	fxLightSource = geoFX.getVariable("lightSource");
	fxLightWVP = geoFX.getVariable("lwvp")->AsMatrix();
	fxShadowMap = geoFX.getVariable("shadowMap")->AsShaderResource();

	//set up a handle to the light eye FX file vars
	fxLightEyeWVP = lightEyeFX.getVariable("wvpMatrix")->AsMatrix();
	fxLightEyeWorld = lightEyeFX.getVariable("worldMatrix")->AsMatrix();
	fxLightEyeDiffuse = lightEyeFX.getVariable("diffuseMap")->AsShaderResource();

	//set up handle to the depth FX file vars
	shadowsWVP = shadowsFX.getVariable("wvp")->AsMatrix();
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

	mDevice->CreateInputLayout(posNormLayout, 4, geoFX.getSignature(),
		geoFX.getSigSize(), &pnVertexLayout);

	mDevice->CreateInputLayout(posColLayout, 2, geoFX.getSignature(),
		geoFX.getSigSize(), &pcVertexLayout);

	mDevice->CreateInputLayout(posTexLayout, 2, lightEyeFX.getSignature(),
		lightEyeFX.getSigSize(), &ptVertexLayout);

	mDevice->CreateInputLayout(posNormLayout, 4, shadowsFX.getSignature(),
		shadowsFX.getSigSize(), &smVertexLayout);
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
	D3DXVECTOR3 lightPos(lightRadius * xCoord, lightHeight, lightRadius * zCoord);
	lightPos.x *= -1;
	lightPos.z *= -1;

	D3DXVECTOR3 lightUpPoint((lightRadius - 2) * xCoord, lightHeight + 1, (lightRadius - 2) * zCoord);
	lightUpPoint = lightUpPoint - lightPos;
	D3DXVec3Normalize(&lightUpPoint, &lightUpPoint);
	//build the view matrix of the light source
	D3DXMatrixLookAtLH(&lightEyeViewMatrix, &lightPos, &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(lightUpPoint));
}

void ShadowMapping::draw()
{
	DXApp::draw();

	//switch the render target to draw to the depth map (shadow map)
	ID3D10RenderTargetView* renderTargets[1] = {NULL};
	mDevice->OMSetRenderTargets(1, renderTargets, depthMap);
	mDevice->OMSetDepthStencilState(0, 0);
	mDevice->RSSetViewports(1, &smViewport);
	mDevice->ClearDepthStencilView(depthMap, D3D10_CLEAR_DEPTH, 1, 0);

	drawToSM();

	//reset the render target to the back buffer
	resetTargets();
	mDevice->OMSetDepthStencilState(0, 0);

	//set up the input assembler
	mDevice->IASetInputLayout(pnVertexLayout);
	mDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//pass the light world-view-projection matrix and the shadow map to the shader
	D3DXMatrixIdentity(&worldMatrix);
	D3DXMATRIX lightEyeWVP = worldMatrix * lightEyeViewMatrix * lightProjMatrix;
	fxLightWVP->SetMatrix((float*)&lightEyeWVP);
	fxShadowMap->SetResource(depthMapRV);

	//calculate and update the world-view-projection matrix for the plane (ground)
	D3DXMatrixIdentity(&worldMatrix);
	wvpMatrix = worldMatrix * viewMatrix * projMatrix;
	fxEyePos->SetRawValue(&camera.GetCameraPosition(), 0, sizeof(D3DXVECTOR3));
	fxLightSource->SetRawValue(&lightSource, 0, sizeof(Light));
	pWorldMatrix->SetMatrix((float*)&worldMatrix);
	pWVP->SetMatrix((float*)&wvpMatrix);

	//draw the scene's plane
	geoFX.getPass()->Apply(0);
	plane.draw();

	//For the first cube...
		//translate it up so that it is on top of the plane
	D3DXMatrixTranslation(&worldMatrix, -3, 2, 0);

	    //calculate and update the wvp matrix of this cube for the light source
	lightEyeWVP = worldMatrix * lightEyeViewMatrix * lightProjMatrix;
	fxLightWVP->SetMatrix((float*)&lightEyeWVP);

	    //calculate and update the wvp matrix of this cube for the camera
	wvpMatrix = worldMatrix * viewMatrix * projMatrix;
	pWorldMatrix->SetMatrix((float*)&worldMatrix);
	pWVP->SetMatrix((float*)&wvpMatrix);

	//draw the first cube
	geoFX.getPass()->Apply(0);
	cube1.draw();

	//For the second cube...
		//translate it up so that it is on top of the plane
	D3DXMatrixTranslation(&worldMatrix, 3, 1, 0);

		//calculate and update the wvp matrix of this cube for the light source
	lightEyeWVP = worldMatrix * lightEyeViewMatrix * lightProjMatrix;
	fxLightWVP->SetMatrix((float*)&lightEyeWVP);

		//calculate and update the wvp matrix of this cube for the camera
	wvpMatrix = worldMatrix * viewMatrix * projMatrix;
	pWorldMatrix->SetMatrix((float*)&worldMatrix);
	pWVP->SetMatrix((float*)&wvpMatrix);

	//draw the second cube
	geoFX.getPass()->Apply(0);
	cube2.draw();

	//build a transformation matrix for the tetrahedron
	D3DXMatrixTranslation(&worldMatrix, 0, 1, 0);

	//calculate and update the wvp matrix of this tetrahedron for the light source
	lightEyeWVP = worldMatrix * lightEyeViewMatrix * lightProjMatrix;
	fxLightWVP->SetMatrix((float*)&lightEyeWVP);

	//calculate and update the wvp matrix of this tetrahedron for the camera
	wvpMatrix = worldMatrix * viewMatrix * projMatrix;
	pWorldMatrix->SetMatrix((float*)&worldMatrix);
	pWVP->SetMatrix((float*)&wvpMatrix);

	//draw the tetrahedron
	geoFX.getPass()->Apply(0);
	tet1.draw();

	//switch the input layout so we can use textures
	mDevice->IASetInputLayout(ptVertexLayout);

	D3DXMatrixTranslation(&worldMatrix, 7, 1, -8);
	wvpMatrix = worldMatrix * viewMatrix * projMatrix;

	fxLightEyeWVP->SetMatrix((float*)wvpMatrix);
	fxLightEyeWorld->SetMatrix((float*)worldMatrix);
	fxLightEyeDiffuse->SetResource(depthMapRV);

	//draw the light's perspective
	lightEyeFX.getPass()->Apply(0);
	lightEye.draw();

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
	wvpMatrix = worldMatrix * lightEyeViewMatrix * lightProjMatrix;
	shadowsWVP->SetMatrix((float*)&wvpMatrix);

	//draw the scene's plane
	shadowsFX.getPass()->Apply(0);
	plane.draw();

	//translate the cube up and to the left
	D3DXMatrixTranslation(&worldMatrix, -3, 1.0f, 0);
	wvpMatrix = worldMatrix * lightEyeViewMatrix * lightProjMatrix;
	shadowsWVP->SetMatrix((float*)&wvpMatrix);

	//draw the first cube
	shadowsFX.getPass()->Apply(0);
	cube1.draw();

	//translate the cube up and to the right
	D3DXMatrixTranslation(&worldMatrix, 3, 1.0f, 0);
	wvpMatrix = worldMatrix * lightEyeViewMatrix * lightProjMatrix;
	shadowsWVP->SetMatrix((float*)&wvpMatrix);

	//draw the second cube
	shadowsFX.getPass()->Apply(0);
	cube2.draw();

	//translate the tetrahedron up
	D3DXMatrixTranslation(&worldMatrix, 0, 1, 0);
	wvpMatrix = worldMatrix * lightEyeViewMatrix * lightProjMatrix;
	shadowsWVP->SetMatrix((float*)&wvpMatrix);

	//draw the tetrahedron
	shadowsFX.getPass()->Apply(0);
	tet1.draw();
}

int WINAPI WinMain(HINSTANCE hi, HINSTANCE hpi, PSTR pcl, int sc)
{
	ShadowMapping sm(hi);
	sm.initWindow(hi, sc);
	sm.init();
	return sm.run();
}