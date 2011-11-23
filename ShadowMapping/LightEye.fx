
float4x4 worldMatrix;
float4x4 wvpMatrix;
float4x4 texMatrix;

Texture2D diffuseMap;

SamplerState triLinearSample
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VSOut
{
	float4 posH: SV_POSITION;
	float4 posW: POSITION;
	float2 texC: TEXCOORD;
};

VSOut VS(float3 posL:POSITION, float2 texC:TEXCOORD)
{
	VSOut vOut;

	vOut.posH = mul(float4(posL, 1), wvpMatrix);
	vOut.posW = mul(float4(posL, 1), worldMatrix);
	vOut.texC = texC;

	return vOut;
}

float4 PS(VSOut pIn):SV_Target
{
	float4 diffuse = diffuseMap.Sample(triLinearSample, pIn.texC);
	return diffuse;
}

technique10 T0
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
}