#pragma once
#include "Utility.h"

class FX
{
public:
	FX(void);
	~FX(void){}

	void init(ID3D10Device* device, LPCTSTR fileName);
	ID3D10Effect* getEffect() const { return pEffect; }
	ID3D10EffectPass* getPass() const { return pPass; }
	ID3D10EffectVariable* getVariable(LPCSTR name);
	BYTE* getSignature() const { return passDesc.pIAInputSignature; }
	SIZE_T getSigSize() const { return passDesc.IAInputSignatureSize; }

private:
	ID3D10Effect* pEffect;
	ID3D10EffectTechnique* pTechnique;
	ID3D10EffectPass* pPass;
	D3D10_PASS_DESC passDesc;
};