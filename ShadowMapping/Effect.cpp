#include "Effect.h"
#include <sstream>

FX::FX():
pEffect(0), pTechnique(0), pPass(0)
{
}

void FX::init(ID3D10Device* device, LPCTSTR fileName)
{
	ID3D10Blob* compileErrors = NULL;

	D3DX10CreateEffectFromFile(fileName, 0, 0, "fx_4_0", 0, 0, device, 0, 0, 
		&pEffect, &compileErrors, 0);

	if(compileErrors)
		MessageBoxA(0, (char*)compileErrors->GetBufferPointer(), 0, 0);

	pTechnique = pEffect->GetTechniqueByIndex(0);
	pPass = pTechnique->GetPassByIndex(0);
	pPass->GetDesc(&passDesc);
}

ID3D10EffectVariable* FX::getVariable(LPCSTR name)
{
	std::stringstream errorString;

	ID3D10EffectVariable* returnVal = pEffect->GetVariableByName(name);
	if(!returnVal->IsValid())
	{
		errorString << "Invalid effect variable name '" << name << "'.";
		MessageBoxA(0, errorString.str().c_str(), 0, 0);
	}

	return returnVal;
}
